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
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "PacketQueue.h"
#include "VoiceMap.h"

// #include "MemoryDebug.h"

/**
 * @ingroup SipCallDump
 * @brief Queue를 주기적으로 모니터링하는 thread
 * @param lpParameter 
 * @returns 0 을 리턴한다.
 */
THREAD_API QueueMonitorThread( LPVOID lpParameter )
{
    // PacketQueue gclsPacketQueue;
    // PacketQueue gclsSignalPacketQueue;
    // PacketQueue gclsVoicePacketQueue;
    // VoiceMap gclsVoiceMap;
    int iMQsize, iSQsize, iVQsize, iVMsize;

	while( gbStop == false )
	{
            iMQsize = gclsPacketQueue.getSize();
            iSQsize = gclsSignalPacketQueue.getSize();
            iVQsize = gclsVoicePacketQueue.getSize();
            iVMsize = gclsVoiceMap.getSize();

            CLog::Print( LOG_INFO, "[%s] MainQ(%d), SigQ(%d), VceQ(%d), VceMap(%d)", __FUNCTION__ , iMQsize, iSQsize, iVQsize, iVMsize);

			MiliSleep(1000);
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
bool StartQueueMonitorThread( )
{
	return StartThread( "StartQueueMonitorThread", QueueMonitorThread, NULL );
}
