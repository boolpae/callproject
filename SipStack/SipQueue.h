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

#ifndef _SIP_QUEUE_H_
#define _SIP_QUEUE_H_

#include "SipTransport.h"
#include "SipMutex.h"
#include <list>
#include <string>

/**
 * @ingroup SipStack
 * @brief SIP ���� �޽����� �����ϴ� Ŭ����
 */
class CSipQueueEntry
{
public:
	/** SIP ���� �޽��� */
	std::string m_strBuf;

	/** SIP �޽��� ���� IP �ּ� */
	std::string m_strIp;

	/** SIP �޽��� ���� IP �ּ� */
	unsigned short m_sPort;

	/** SIP �޽��� ���� �������� */
	ESipTransport m_eTransport;
};

typedef std::list< CSipQueueEntry * > SIP_QUEUE;

/**
 * @ingroup SipStack
 * @brief SIP ���� �޽��� ����Ʈ�� �����ϴ� �ڷᱸ�� Ŭ����
 */
class CSipQueue
{
public:
	CSipQueue();
	~CSipQueue();

	bool Insert( const char * pszBuf, int iBufLen, const char * pszIp, unsigned short sPort, ESipTransport eTransport );
	bool Select( CSipQueueEntry ** ppclsEntry );
	void BroadCast( );

private:
	SIP_QUEUE	m_clsList;
	CSipMutexSignal m_clsMutex;
};

extern CSipQueue gclsSipQueue;

#endif
