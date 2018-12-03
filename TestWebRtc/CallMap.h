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

#ifndef _CALL_MAP_H_
#define _CALL_MAP_H_

#include "SipUserAgent.h"
#include <string>
#include <map>
#include "SipMutex.h"

/**
 * @ingroup TestWebRtc
 * @brief ��ȭ ���� ���� Ŭ����
 */
class CCallInfo
{
public:
	CCallInfo();

	// WebRTC ����� ���̵�
	std::string m_strUserId;

	std::string	m_strPbxRtpIp;

	int					m_iPbxRtpPort;
};

// key = SIP Call-ID
typedef std::map< std::string, CCallInfo > CALL_MAP;

/**
 * @ingroup TestWebRtc
 * @brief ��ȭ ���� ���� �ڷᱸ�� Ŭ����
 */
class CCallMap
{
public:
	CCallMap();
	~CCallMap();

	bool Insert( const char * pszCallId, const char * pszUserId );
	bool Update( const char * pszCallId, const char * pszPbxRtpIp, int iPbxRtpPort );

	bool Select( const char * pszCallId, CCallInfo & clsCallInfo );
	bool SelectUserId( const char * pszUserId );
	bool SelectUserId( const char * pszUserId, std::string & strCallId );

	bool Delete( const char * pszCallId );
	bool DeleteUserId( const char * pszUserId, SIP_CALL_ID_LIST & clsCallIdList );

private:
	CALL_MAP	m_clsMap;
	CSipMutex	m_clsMutex;
};

extern CCallMap gclsCallMap;

#endif
