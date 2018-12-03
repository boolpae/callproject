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

#include "SipQueue.h"

CSipQueue gclsSipQueue;

CSipQueue::CSipQueue()
{
}

CSipQueue::~CSipQueue()
{
}

/**
 * @ingroup SipStack
 * @brief SIP ���� �޽����� �����Ѵ�.
 * @param pszBuf			SIP ���� �޽���
 * @param iBufLen			SIP ���� �޽��� ����
 * @param pszIp				SIP ���� IP �ּ�
 * @param sPort				SIP ���� ��Ʈ ��ȣ
 * @param eTransport	SIP ���� ��������
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CSipQueue::Insert( const char * pszBuf, int iBufLen, const char * pszIp, unsigned short sPort, ESipTransport eTransport )
{
	CSipQueueEntry * pclsEntry = new CSipQueueEntry();
	if( pclsEntry == NULL ) return false;

	pclsEntry->m_strBuf.append( pszBuf, iBufLen );
	pclsEntry->m_strIp = pszIp;
	pclsEntry->m_sPort = sPort;
	pclsEntry->m_eTransport = eTransport;

	m_clsMutex.acquire();
	m_clsList.push_back( pclsEntry );
	m_clsMutex.signal();
	m_clsMutex.release();

	return true;
}

/**
 * @ingroup SipStack
 * @brief SIP ���� �޽����� �����´�.
 * @param ppclsEntry SIP ���� �޽����� ������ ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CSipQueue::Select( CSipQueueEntry ** ppclsEntry )
{
	bool bRes = false;

	m_clsMutex.acquire();
	if( m_clsList.empty() )
	{
		m_clsMutex.wait();
	}
	
	if( m_clsList.empty() == false )
	{
		*ppclsEntry = m_clsList.front();
		m_clsList.pop_front();
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup SipStack
 * @brief SIP ���� �޽����� ����ϴ� �����带 ��� �����.
 */
void CSipQueue::BroadCast( )
{
	m_clsMutex.broadcast();
}
