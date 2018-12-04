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
 * @brief SignalQueue로부터 item을 가져와 처리한다. Invite 시작 시 전용 Thread 생성
 * @param lpParameter 
 * @returns 0 을 리턴한다.
 */
THREAD_API SignalHandleThread( LPVOID lpParameter )
{
	PacketItem *item = nullptr;
	uint8_t *pszData = nullptr;
	int			iIpHeaderLen, iUdpBodyLen, iIpPos;
	char		* pszUdpBody;
	Ip4Header		* psttIp4Header;	// IPv4 Header
	UdpHeader		* psttUdpHeader;	// UDP Header

	CLog::Print( LOG_INFO, "%s is started", __FUNCTION__ );

	while( gbStop == false )
	{
		// gclsSignalPacketQueue에서 패킷을 가져온다.
		// SIP패킷 분석 및 신규 호 시작 시엔 전용 쓰레드 생성

		if ( item = gclsSignalPacketQueue.pop() )
		{
			pszData = item->getData();
			
			if( pszData[12] == 0x81 )
			{
				iIpPos = 18;		// VLAN
			}
			else if( pszData[12] == 0x08 )
			{
				iIpPos = 14;		// IP
			}
			else
			{
				continue;
			}

			psttIp4Header = ( Ip4Header * )( pszData + iIpPos );
			psttUdpHeader = ( UdpHeader * )( pszData + iIpPos + iIpHeaderLen );
			pszUdpBody = ( char * )( pszData + iIpPos + iIpHeaderLen + 8 );
			iUdpBodyLen = item->getLen() - ( iIpPos + iIpHeaderLen + 8 );

            // 이미 처리할 item은 SIP Packet으로 간주해야한다.

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
bool StartSignalHandleThread( )
{
	return StartThread( "StartSignalHandleThread", SignalHandleThread, NULL );
}