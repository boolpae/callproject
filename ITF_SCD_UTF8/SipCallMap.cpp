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

#include "SipCallMap.h"
#include "Log.h"
#include "RtpMap.h"
#include "SipCallDumpSetup.h"
#include "TimeString.h"
#include "MemoryDebug.h"

CSipCallMap gclsCallMap;

bool StartVoiceHandleThread( std::string &strCallId );	// VoiceHandleThread.cpp for ITF_CRD

CSipRtpInfo::CSipRtpInfo() : m_iPort(0)
{
}

CSipCallInfo::CSipCallInfo() : m_psttPcap(NULL), m_iInviteTime(0), m_iCancelTime(0), m_iStartTime(0), m_iByeTime(0)
{
}

/**
 * @ingroup SipCallDump
 * @brief SDP 메시지를 RTP 정보에 저장한다.
 * @param pclsSdp			SDP 메시지
 * @param clsRtpInfo	RTP 정보
 */
static void SetRtpInfo( CSdpMessage * pclsSdp, CSipRtpInfo & clsRtpInfo )
{
	// bool bFound = false;
	CSdpMedia * pclsMedia;

	clsRtpInfo.m_strIp = pclsSdp->m_clsConnection.m_strAddr;

	if( pclsSdp->SelectMedia( "audio", &pclsMedia ) == false )
	{
		CLog::Print( LOG_ERROR, "%s audio media is not found in SDP", __FUNCTION__ );
		return;
	}

	clsRtpInfo.m_iPort = pclsMedia->m_iPort;
	
	// media 에 connection 정보가 존재하면 이를 RTP 세션의 IP 주소로 설정한다.
	if( pclsMedia->m_clsConnection.Empty() == false )
	{
		clsRtpInfo.m_strIp = pclsMedia->m_clsConnection.m_strAddr;
	}
}

/**
 * @ingroup SipCallDump
 * @brief SIP 메시지에서 SDP 메시지를 가져온다.
 * @param clsMessage	SIP 메시지
 * @param clsSdp			[out] SDP 메시지
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
static bool GetSdp( CSipMessage & clsMessage, CSdpMessage & clsSdp )
{
	if( clsMessage.m_iContentLength > 0 )
	{
		if( clsMessage.m_clsContentType.IsEqual( "application", "sdp" ) )
		{
			if( clsSdp.Parse( clsMessage.m_strBody.c_str(), clsMessage.m_strBody.length() ) > 0 )
			{
				return true;
			}
		}
		else
		{
			CLog::Print( LOG_ERROR, "%s Content-Type is not application/sdp", __FUNCTION__ );
		}
	}

	return false;
}

CSipCallMap::CSipCallMap()
{
}

CSipCallMap::~CSipCallMap()
{
}

/**
 * @ingroup SipCallDump
 * @brief SIP 패킷을 저장한다.
 * @param psttPcap		패킷 캡처 중인 pcap 구조체의 포인터
 * @param psttHeader	패킷 캡처한 헤더
 * @param pszData			패킷 캡처한 패킷
 * @param pszUdpBody	UDP body
 * @param iUdpBodyLen UDP body 길이
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::Insert( pcap_t * psttPcap, struct pcap_pkthdr * psttHeader, const	u_char * pszData, const char * pszUdpBody, int iUdpBodyLen )
{
	CSipMessage clsMessage;

	CLog::Print( LOG_NETWORK, "Sip[%.*s]", iUdpBodyLen, pszUdpBody );

	if( clsMessage.Parse( pszUdpBody, iUdpBodyLen ) == false )
	{
		CLog::Print( LOG_ERROR, "sip parse error" );
		return false;
	}

	if( clsMessage.IsRequest() )
	{
		if( clsMessage.IsMethod( "INVITE" ) )
		{
			CSdpMessage clsSdp;

			if( GetSdp( clsMessage, clsSdp ) == false )
			{
				CLog::Print( LOG_ERROR, "%s INVITE no SDP", __FUNCTION__ );
				return false;
			}

			InsertInvite( psttPcap, psttHeader, pszData, &clsMessage, &clsSdp );
		}
		else
		{
			Insert( psttHeader, pszData, &clsMessage );
		}
	}
	else
	{
		if( clsMessage.IsMethod( "INVITE" ) )
		{
			CSdpMessage clsSdp;

			if( GetSdp( clsMessage, clsSdp ) == false )
			{
				Insert( psttHeader, pszData, &clsMessage );
			}
			else
			{
				InsertInviteResponse( psttHeader, pszData, &clsMessage, &clsSdp );
			}
		}
		else
		{
			Insert( psttHeader, pszData, &clsMessage );
		}
	}

	return true;
}

/**
 * @ingroup SipCallDump
 * @brief RTP 패킷을 저장한다.
 * @param pszCallId		SIP Call-ID
 * @param psttHeader	패킷 캡처한 헤더
 * @param pszData			패킷 캡처한 패킷
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::Insert( const char * pszCallId, struct pcap_pkthdr * psttHeader, const u_char * pszData )
{
	SIP_CALL_MAP::iterator itMap;
	bool bRes = false;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszCallId );
	if( itMap != m_clsMap.end() )
	{
		pcap_dump( (u_char *)itMap->second.m_psttPcap, psttHeader, pszData );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipCallDump
 * @brief 통화 정보를 삭제한다.
 * @param pszCallId SIP Call-ID
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::Delete( const char * pszCallId )
{
	SIP_CALL_MAP::iterator itMap;
	bool bRes = false;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszCallId );
	if( itMap != m_clsMap.end() )
	{
		Erase( itMap );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipCallDump
 * @brief 모든 통화 정보를 삭제한다.
 */
void CSipCallMap::DeleteAll( )
{
	SIP_CALL_MAP::iterator itMap, itNext;

	m_clsMutex.acquire();
	for( itMap = m_clsMap.begin(); itMap != m_clsMap.end(); ++itMap )
	{
		gclsRtpMap.Delete( itMap->second.m_clsFromRtp.m_strIp.c_str(), itMap->second.m_clsFromRtp.m_iPort );
		gclsRtpMap.Delete( itMap->second.m_clsToRtp.m_strIp.c_str(), itMap->second.m_clsToRtp.m_iPort );
		pcap_dump_close( itMap->second.m_psttPcap );
	}
	m_clsMap.clear();
	m_clsMutex.release();
}

/**
 * @ingroup SipCallDump
 * @brief SIP INVITE 요청 메시지 정보를 저장한다.
 * @param psttPcap		패킷 캡처 중인 pcap 구조체의 포인터
 * @param psttHeader	패킷 캡처한 헤더
 * @param pszData			패킷 캡처한 패킷
 * @param pclsMessage SIP 메시지
 * @param pclsSdp			SDP 메시지
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::InsertInvite( pcap_t * psttPcap, struct pcap_pkthdr * psttHeader, const u_char * pszData, CSipMessage * pclsMessage, CSdpMessage * pclsSdp )
{
	SIP_CALL_MAP::iterator itMap;
	std::string strCallId;
	bool bRes = false;

	if( pclsMessage->GetCallId( strCallId ) == false )
	{
		CLog::Print( LOG_ERROR, "%s pclsMessage->GetCallId() error", __FUNCTION__ );
		return false;
	}

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strCallId );
	if( itMap == m_clsMap.end() )
	{
		CSipCallInfo clsCallInfo;

		SetRtpInfo( pclsSdp, clsCallInfo.m_clsFromRtp );

		m_clsMap.insert( SIP_CALL_MAP::value_type( strCallId, clsCallInfo ) );

		if ( !StartVoiceHandleThread( strCallId ) )
		{
			CLog::Print( LOG_ERROR, "%s failed to start VoiceHandleThread(%s) error", __FUNCTION__, strCallId.c_str() );
		}

		time_t	iTime;
		struct tm	sttTm;
		char szTemp[21];

		time( &iTime );

		LocalTime( iTime, sttTm );

		std::string strFileName = gclsSetup.m_strPacketFolder;
		snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday );
		CDirectory::AppendName( strFileName, szTemp );
		CDirectory::Create( strFileName.c_str() );
		
		snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d_", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec );
		CDirectory::AppendName( strFileName, szTemp );
		strFileName.append( strCallId );
		strFileName.append( ".pcap" );

		itMap = m_clsMap.find( strCallId );

		itMap->second.m_psttPcap = pcap_dump_open( psttPcap, strFileName.c_str() );
		if( itMap->second.m_psttPcap == NULL )
		{
			CLog::Print( LOG_ERROR, "%s pcap_dump_open(%s) error", __FUNCTION__, strFileName.c_str() );
			m_clsMap.erase( itMap );
		}
		else
		{
			itMap->second.m_iInviteTime = iTime;
			pcap_dump( (u_char *)itMap->second.m_psttPcap, psttHeader, pszData );
		}
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipCallDump
 * @brief SIP INVITE 응답 메시지 정보를 저장한다.
 * @param psttHeader	패킷 캡처한 헤더
 * @param pszData			패킷 캡처한 패킷
 * @param pclsMessage SIP 메시지
 * @param pclsSdp			SDP 메시지
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::InsertInviteResponse( struct pcap_pkthdr * psttHeader, const u_char * pszData, CSipMessage * pclsMessage, CSdpMessage * pclsSdp )
{
	SIP_CALL_MAP::iterator itMap;
	std::string strCallId;
	bool bRes = false;

	if( pclsMessage->GetCallId( strCallId ) == false )
	{
		CLog::Print( LOG_ERROR, "%s pclsMessage->GetCallId() error", __FUNCTION__ );
		return false;
	}

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strCallId );
	if( itMap != m_clsMap.end() )
	{
		time( &itMap->second.m_iStartTime );

		if( itMap->second.m_clsToRtp.m_iPort )
		{
			gclsRtpMap.Delete( itMap->second.m_clsToRtp.m_strIp.c_str(), itMap->second.m_clsToRtp.m_iPort );
		}

		SetRtpInfo( pclsSdp, itMap->second.m_clsToRtp );

		gclsRtpMap.Insert( itMap->second.m_clsFromRtp.m_strIp.c_str(), itMap->second.m_clsFromRtp.m_iPort, strCallId.c_str() );
		gclsRtpMap.Insert( itMap->second.m_clsToRtp.m_strIp.c_str(), itMap->second.m_clsToRtp.m_iPort, strCallId.c_str() );
		pcap_dump( (u_char *)itMap->second.m_psttPcap, psttHeader, pszData );

		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipCallDump
 * @brief INVITE 요청/응답을 제외한 SIP 패킷을 저장한다.
 * @param psttHeader	패킷 캡처한 헤더
 * @param pszData			패킷 캡처한 패킷
 * @param pclsMessage SIP 메시지
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CSipCallMap::Insert( struct pcap_pkthdr * psttHeader, const u_char * pszData, CSipMessage * pclsMessage )
{
	SIP_CALL_MAP::iterator itMap;
	std::string strCallId;
	bool bRes = false;

	if( pclsMessage->GetCallId( strCallId ) == false )
	{
		CLog::Print( LOG_ERROR, "%s pclsMessage->GetCallId() error", __FUNCTION__ );
		return false;
	}

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strCallId );
	if( itMap != m_clsMap.end() )
	{
		bool bDelete = false;

		if( pclsMessage->IsRequest() )
		{
			if( pclsMessage->IsMethod( "CANCEL" ) )
			{
				time( &itMap->second.m_iCancelTime );
			}
			else if( pclsMessage->IsMethod( "ACK" ) )
			{
				if( itMap->second.m_iCancelTime )
				{
					bDelete = true;
				}
			}
			else if( pclsMessage->IsMethod( "BYE" ) )
			{
				time( &itMap->second.m_iByeTime );
			}
		}
		else
		{
			if( pclsMessage->IsMethod( "BYE" ) )
			{
				bDelete = true;
			}
		}
		
		pcap_dump( (u_char *)itMap->second.m_psttPcap, psttHeader, pszData );

		if( bDelete )
		{
			Erase( itMap );
		}

		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipCallDump
 * @brief 자료구조에서 삭제한다.
 * @param itMap 삭제 대상 iterator
 */
void CSipCallMap::Erase( SIP_CALL_MAP::iterator & itMap )
{
	gclsRtpMap.Delete( itMap->second.m_clsFromRtp.m_strIp.c_str(), itMap->second.m_clsFromRtp.m_iPort );
	gclsRtpMap.Delete( itMap->second.m_clsToRtp.m_strIp.c_str(), itMap->second.m_clsToRtp.m_iPort );

	pcap_dump_close( itMap->second.m_psttPcap );

	m_clsMap.erase( itMap );
}

bool CSipCallMap::FindCall( std::string &strCallId )
{
	SIP_CALL_MAP::iterator itMap;
	bool bRes = false;

	itMap = m_clsMap.find( strCallId );
	if( itMap != m_clsMap.end() ) bRes = true;

	return bRes;
}
