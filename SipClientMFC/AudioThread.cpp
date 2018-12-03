/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "stdafx.h"
#include "SipPlatformDefine.h"
#include "AudioThread.h"
#include "ServerUtility.h"
#include "RtpHeader.h"
#include "G711.h"

/**
 * @ingroup SipClientMFC
 * @brief RTP audio ���� ������
 * @param lpParameter CAudioThread ��ü�� ������
 * @returns 0 �� �����Ѵ�.
 */
THREAD_API AudioRtpRecvThread( LPVOID lpParameter )
{
	CAudioThread * pclsThread = (CAudioThread *)lpParameter;
	pollfd sttPoll[1];
	char	szPacket[320], szPCM[320], szIp[21];
	int		iPacketLen;
	unsigned short sPort;

	TcpSetPollIn( sttPoll[0], pclsThread->m_hSocket );

	pclsThread->m_bThreadRun = true;
	while( pclsThread->m_bStop == false )
	{
		if( poll( sttPoll, 1, 200 ) <= 0 )
		{
			continue;
		}

		iPacketLen = sizeof(szPacket);
		if( UdpRecv( pclsThread->m_hSocket, szPacket, &iPacketLen, szIp, sizeof(szIp), &sPort ) == false )
		{
			continue;
		}

		if( iPacketLen == 160 + sizeof(RtpHeader) )
		{
			UlawToPcm( szPacket + sizeof(RtpHeader), 160, szPCM, sizeof(szPCM) );
			pclsThread->m_clsAudio.OutPcm( (int16_t *)szPCM, 160 );
		}
	}
	pclsThread->m_bThreadRun = false;

	return 0;
}

CAudioThread::CAudioThread() : m_iPort(0), m_hSocket(INVALID_SOCKET), m_iDestPort(0), m_sSeq(0), m_iTimeStamp(0), m_bStop(false), m_bThreadRun(false)
{
}

CAudioThread::~CAudioThread()
{
}

/**
 * @ingroup SipClientMFC
 * @brief ����� ����̽��� �����ϰ� ����� RTP ��Ʈ ���� �� ����� RTP ���� �����带 �����Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CAudioThread::Start()
{
	if( m_iPort > 0 )
	{
		return false;
	}

	if( m_clsAudio.Open( this ) == false )
	{
		return false;
	}

	for( int i = 10000; i < 11000; i += 2 )
	{
		m_hSocket = UdpListen( i, NULL );
		if( m_hSocket != INVALID_SOCKET )
		{
			m_iPort = i;
			break;
		}
	}

	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}

	if( StartThread( "AudioRtpRecvThread", AudioRtpRecvThread, this ) == false )
	{
		Stop();
		return false;
	}

	return true;
}

/**
 * @ingroup SipClientMFC
 * @brief ����� RTP ���� �����带 �����ϰ� RTP ��Ʈ�� ������ ��, ����� ����̽��� �����Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CAudioThread::Stop()
{
	if( m_bStop ) return true;

	m_bStop = true;

	// RTP ���� �����尡 ������ ������ 1�� ���� ����Ѵ�.
	for( int i = 0; i < 50; ++i )
	{
		if( m_bThreadRun == false ) break;
		Sleep(20);
	}

	if( m_hSocket != INVALID_SOCKET )
	{
		closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}

	m_clsAudio.Close();

	m_strDestIp.clear();
	m_iDestPort = 0;

	m_iPort = 0;
	m_bStop = false;

	return true;
}

/**
 * @ingroup SipClientMFC
 * @brief RTP ��Ŷ�� ������ ������ ������ �����Ѵ�.
 * @param pszDestIp ������ IP �ּ�
 * @param iDestPort ������ ��Ʈ ��ȣ
 */
void CAudioThread::SetDestIpPort( const char * pszDestIp, int iDestPort )
{
	m_strDestIp = pszDestIp;
	m_iDestPort = iDestPort;

#ifdef RECORD_RAW
	m_clsAudio.Record( "c:\\temp\\out.raw", "c:\\temp\\in.raw" );
#endif
}

/**
 * @ingroup SipClientMFC
 * @brief PC ����ũ���� ������ PCM ������ ���� �̺�Ʈ callback
 * @param parrPcm PCM ����
 * @param iPcmLen PCM ���� ũ��
 */
void CAudioThread::EventInPcm( const int16_t * parrPcm, int iPcmLen )
{
	if( m_strDestIp.empty() == false && m_iDestPort > 0 )
	{
		char szPacket[320];
		RtpHeader * psttRtpHeader = (RtpHeader *)szPacket;

		psttRtpHeader->SetVersion(2);
		psttRtpHeader->SetPadding(0);
		psttRtpHeader->SetExtension(0);
		psttRtpHeader->SetCC(0);
		psttRtpHeader->SetMarker(0);
		psttRtpHeader->ssrc = htonl( 200 );

		psttRtpHeader->SetPT(0);
		psttRtpHeader->SetSeq( m_sSeq );
		psttRtpHeader->SetTimeStamp( m_iTimeStamp );

		++m_sSeq;
		m_iTimeStamp += 160;

		PcmToUlaw( (char *)parrPcm, 320, szPacket + sizeof(RtpHeader), 160 );

		UdpSend( m_hSocket, szPacket, 160 + sizeof(RtpHeader), m_strDestIp.c_str(), m_iDestPort );
	}
}
