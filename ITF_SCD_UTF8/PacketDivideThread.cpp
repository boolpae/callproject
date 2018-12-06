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


// #include "MemoryDebug.h"

/**
 * @ingroup SipCallDump
 * @brief 덤프된 패킷을 파싱하여 SIP, RTP 패킷을 구분지어 해당 queue로 나누어 저장한다.
 * @param lpParameter 
 * @returns 0 을 리턴한다.
 */
THREAD_API PacketDivideThread( LPVOID lpParameter )
{
	PacketItem *item = nullptr;

	CLog::Print( LOG_INFO, "%s is started", __FUNCTION__ );

	while( gbStop == false )
	{
		// gclsPacketQueue에서 패킷을 가져온다.
		// SIP 인지 RTP 인지 판단하여 Type에 해당하는 queue에 저장

		if ( item = gclsPacketQueue.pop() )
		{
			PacketItem::Init(item);	// 큐에 들어가는 item을 큐에 push하기 전에 필요한 정보들을 세팅한다.
			if( IsRtpPacket( item->m_pszUdpBody, item->m_iUdpBodyLen ) )
			{
				// gclsRtpMap.Insert( psttHeader, pszData, psttIp4Header, psttUdpHeader );
				gclsVoicePacketQueue.push( item );
			}
			else if( IsSipPacket( item->m_pszUdpBody, item->m_iUdpBodyLen ) )
			{
				// gclsCallMap.Insert( psttPcap, psttHeader, pszData, pszUdpBody, iUdpBodyLen );
				gclsSignalPacketQueue.push( item );
			}

		}
		else
		{
			MiliSleep(1);
		}
	}

FUNC_END:

	CLog::Print( LOG_INFO, "%s is stoped", __FUNCTION__ );

	return 0;
}

/**
 * @ingroup SipCallDump
 * @brief 패킷 덤프 쓰레드를 시작한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool StartPacketDivideThread( )
{
	return StartThread( "PacketDivideThread", PacketDivideThread, NULL );
}
