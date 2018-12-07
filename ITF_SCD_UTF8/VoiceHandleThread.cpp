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

#include "ServerService.h"
#include "Log.h"
#include "PacketHeader.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "PacketQueue.h"
#include "VoiceMap.h"
#include "SipCallDumpSetup.h"
#include "TimeString.h"
#include "SipCallMap.h"
#include "g711.h"

#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// #include "MemoryDebug.h"

#define RTP_HEAD_SIZE 12

/**
 * @ingroup SipCallDump
 * @brief VoiceQueue로부터 item을 가져와 처리한다. 이 쓰레드는 Call-ID를 키로 생성되는 쓰레드이다.
 *        큐에서 item을 가져와 decoding 및 파일에 저장한다.
 * @param lpParameter 
 * @returns 0 을 리턴한다.
 */
THREAD_API VoiceHandleThread( LPVOID lpParameter )
{
	PacketItem *item = nullptr;
    PacketQueue *que = nullptr;
	std::string strCallId( (char *)lpParameter );

	que = gclsVoiceMap.createQueue( strCallId );

	if ( !que ) {
		// 큐 생성 실패, 전용 쓰레드 종료
		return nullptr;
	}

    time_t	iTime;
    struct tm	sttTm;
    char szTemp[21];
    uint32_t mask = inet_addr(gclsSetup.m_strCSMask.c_str());

    time( &iTime );

    LocalTime( iTime, sttTm );

    std::string strFileName_r;
    std::string strFileName_l = gclsSetup.m_strVoiceFolder;
    snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday );
    CDirectory::AppendName( strFileName_l, szTemp );
    CDirectory::Create( strFileName_l.c_str() );
    
    snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d_", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec );
    CDirectory::AppendName( strFileName_l, szTemp );
    strFileName_l.append( strCallId );
    strFileName_r = strFileName_l;

    strFileName_l.append( "_l.pcm" );
    strFileName_r.append( "_r.pcm" );

    std::ofstream pcmFile_l;
    std::ofstream pcmFile_r;

    pcmFile_l.open(strFileName_l.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!pcmFile_l.is_open())
    {
		CLog::Print( LOG_ERROR, "%s(%s) failed to open file(%s)", __FUNCTION__, strCallId.c_str(), strFileName_l.c_str() );
        goto FUNC_END;
    }

    pcmFile_r.open(strFileName_r.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!pcmFile_r.is_open())
    {
		pcmFile_l.close();
		CLog::Print( LOG_ERROR, "%s(%s) failed to open file(%s)", __FUNCTION__, strCallId.c_str(), strFileName_r.c_str() );
        goto FUNC_END;
    }

	CLog::Print( LOG_INFO, "%s(%s) is started", __FUNCTION__, strCallId.c_str() );
	short linearData[1024];
	int vdLen;
	unsigned char *vceData;

	while( gbStop == false )
	{
		// gclsSignalPacketQueue에서 패킷을 가져온다.
		// SIP패킷 분석 및 신규 호 시작 시엔 전용 쓰레드 생성
		item = que->pop();
		if ( item )
		{
			vdLen = item->m_iUdpBodyLen - RTP_HEAD_SIZE;
			vceData = (unsigned char *)item->m_pszUdpBody + RTP_HEAD_SIZE;
			for (int i=0; i<vdLen; i++) {
				linearData[i] = alaw2linear(vceData[i]);
			}

			if ( mask != (item->m_psttIp4Header->daddr & mask) ) {
				// 상담원 목소리 저장(right)
				pcmFile_r.write((const char*)linearData, vdLen*(sizeof(short)));
			}
			else
			{
				// 고객 목소리 저장(left)
				pcmFile_l.write((const char*)linearData, vdLen*(sizeof(short)));
			}

			// 이 곳에서 처리한 item은 삭제한다.
			delete item;
			item = nullptr;

		}
		else
		{
			MiliSleep(1);

            // search Call-ID at SIP Call MAP
			if ( !gclsCallMap.FindCall(strCallId) ) break;
            // if not exist break.
		}
	}

	pcmFile_r.close();
	pcmFile_l.close();

FUNC_END:

	CLog::Print( LOG_INFO, "%s(%s) is stoped", __FUNCTION__, strCallId.c_str() );

	gclsVoiceMap.deleteQueue( strCallId );

	return 0;
}

/**
 * @ingroup SipCallDump
 * @brief 패킷 덤프 쓰레드를 시작한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool StartVoiceHandleThread( std::string &strCallId )
{
	return StartThread( "StartVoiceHandleThread", VoiceHandleThread, (void *)strCallId.c_str() );
}
