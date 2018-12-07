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

#include "SipCallDump.h"
#include "ServerService.h"
#include "ServerUtility.h"
#include "PcapUtility.h"
#include "SipCallMap.h"
#include "RtpMap.h"
#include "MemoryDebug.h"

#include <unistd.h>

/**
 * @ingroup SipCallDump
 * @brief SIP 통화별 패킷 덤프하는 서비스
 * @returns 정상 종료하면 0 을 리턴하고 오류가 발생하면 -1 를 리턴한다.
 */
int ServiceMain( )
{
#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( gclsSetup.Read( GetConfigFileName() ) == false && gclsSetup.Read( CONFIG_FILENAME ) == false )
	{
		printf( "config filename(%s) read error\n", GetConfigFileName() );
		return -1;
	}

	CLog::Print( LOG_SYSTEM, "SipCallDump is started" );

	Fork( true );
	SetCoreDumpEnable();
	ServerSignal();

	int iSecond = 0;
	
	StartPacketDumpThread();

	// for ITF_SCD
	StartPacketDivideThread();	// Main Queue에서 item을 SIGNAL, RTP 로 분리하여 각 Queue로 나눈다.
	StartSignalHandleThread();	// Signal Queue 내 item을 처리한다.
	StartVoicePacketDivideThread();	// RTP Queue 내 item을 Call-ID 별 Queue로 다시 나누어 분배한다.
	StartQueueMonitorThread();	// 어플리케이션에서 사용되는 Queue, Map의 현재 사이즈에 대한 모니터링

	while( gbStop == false )
	{
		++iSecond;

		if( iSecond == 60 )
		{
			// timeout 된 RTP 세션이 존재하면 해당 통화 정보를 삭제한다.
			STRING_LIST clsSipCallIdList;
			STRING_LIST::iterator itSL;

			gclsRtpMap.SelectTimeout( clsSipCallIdList );

			for( itSL = clsSipCallIdList.begin(); itSL != clsSipCallIdList.end(); ++itSL )
			{
				gclsCallMap.Delete( itSL->c_str() );
			}

			iSecond = 0;
		}

		if( gclsSetup.IsChange() )
		{
			gclsSetup.Read();
		}

		sleep(1);
	}

	// PacketDumpThread 가 종료할 때까지 대기한다.
	sleep(1);

	gclsCallMap.DeleteAll();

	CLog::Print( LOG_SYSTEM, "SipCallDump is terminated" );
	CLog::Release();

	return 0;
}

/**
 * @ingroup SipCallDump
 * @brief SIP 통화별 패킷 덤프하는 서비스
 * @param argc 
 * @param argv 
 * @returns 정상 종료하면 0 을 리턴하고 오류가 발생하면 -1 를 리턴한다.
 */
int main( int argc, char * argv[] )
{
	CServerService clsService;

	clsService.m_strName = SERVICE_NAME;
	clsService.m_strDisplayName = SERVICE_DISPLAY_NAME;
	clsService.m_strDescription = SERVICE_DESCRIPTION_STRING;
	clsService.m_strConfigFileName = CONFIG_FILENAME;
	clsService.m_strVersion = SERVER_VERSION;
	clsService.SetBuildDate( __DATE__, __TIME__ );

	if( argc == 2 )
	{
		if( !strcmp( argv[1], "-l" ) )
		{
			PrintDeviceName();
			return 0;
		}
	}

	ServerMain( argc, argv, clsService, ServiceMain );

	return 0;
}
