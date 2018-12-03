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

#include "SipStackThread.h"
#include "SipQueue.h"
#include "TimeUtility.h"
#include "ServerUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

/**
 * @ingroup SipStack
 * @brief SIP ť���� SIP �޽����� �����ͼ� �Ľ� �� callback ó���ϴ� ������
 * @param lpParameter SIP stack ��ü
 * @returns 0 �� �����Ѵ�.
 */
THREAD_API SipQueueThread( LPVOID lpParameter )
{
	CSipStack * pclsSipStack = (CSipStack *)lpParameter;
	CSipQueueEntry * pclsEntry = NULL;
	int		iThreadId;

	pclsSipStack->IncreateTcpThreadCount( iThreadId );

	while( pclsSipStack->m_bStopEvent == false )
	{
		if( gclsSipQueue.Select( &pclsEntry ) )
		{
			pclsSipStack->RecvSipMessage( iThreadId, pclsEntry->m_strBuf.c_str(), (int)pclsEntry->m_strBuf.length()
				, pclsEntry->m_strIp.c_str(), pclsEntry->m_sPort, pclsEntry->m_eTransport );

			delete pclsEntry;
			pclsEntry = NULL;
		}
	}

	pclsSipStack->ThreadEnd( -1 );
	pclsSipStack->DecreateTcpThreadCount();

	return 0;
}

/**
 * @ingroup SipStack
 * @brief SIP ť���� SIP �޽����� �����ͼ� �Ľ� �� callback ó���ϴ� �����带 �����Ѵ�.
 * @param pclsSipStack SIP stack ��ü
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool StartSipQueueThread( CSipStack * pclsSipStack )
{
	return StartThread( "SipQueueThread", SipQueueThread, pclsSipStack );
}
