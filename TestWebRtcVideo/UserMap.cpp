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

#include "SipPlatformDefine.h"
#include "UserMap.h"

CUserMap gclsUserMap;

CUserInfo::CUserInfo() : m_iPort(0), m_pclsRtpArg(NULL)
{
}

CUserMap::CUserMap()
{
}

CUserMap::~CUserMap()
{
}

/**
 * @ingroup TestWebRtc
 * @brief ����� ������ �����Ѵ�.
 * @param pszUserId				����� ���̵�
 * @param pszPassWord			SIP ���� �α��� ��й�ȣ
 * @param pszSipServerIp	SIP ���� IP �ּ�
 * @param pszIp						WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iPort						WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 * @param pszUserAgent		HTTP User-Agent
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CUserMap::Insert( const char * pszUserId, const char * pszPassWord, const char * pszSipServerIp, const char * pszIp, int iPort, const char * pszUserAgent )
{
	bool bRes = false;
	std::string strKey;
	USER_MAP::iterator itMap;
	USER_KEY_MAP::iterator itKeyMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itKeyMap = m_clsKeyMap.find( strKey );
	if( itKeyMap == m_clsKeyMap.end() )
	{
		itMap = m_clsMap.find( pszUserId );
		if( itMap == m_clsMap.end() )
		{
			CUserInfo clsInfo;

			clsInfo.m_strPassWord = pszPassWord;
			clsInfo.m_strSipServerIp = pszSipServerIp;
			clsInfo.m_strIp = pszIp;
			clsInfo.m_iPort = iPort;
			clsInfo.m_strUserAgent = pszUserAgent;

			m_clsMap.insert( USER_MAP::value_type( pszUserId, clsInfo ) );
			m_clsKeyMap.insert( USER_KEY_MAP::value_type( strKey, pszUserId ) );

			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief ����� RTP ���� ������ �����Ѵ�.
 * @param pszUserId		����� ���̵�
 * @param pclsRtpArg	RTP ������ ���� ���� ��ü
 * @param bSetNull		RTP ���� ������ NULL �� ������ ���ΰ�?
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CUserMap::Update( const char * pszUserId, CRtpThreadArg * pclsRtpArg, bool bSetNull )
{
	bool bRes = false;
	USER_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		if( bSetNull )
		{
			if( itMap->second.m_pclsRtpArg == pclsRtpArg )
			{
				itMap->second.m_pclsRtpArg = NULL;
				bRes = true;
			}
		}
		else
		{
			itMap->second.m_pclsRtpArg = pclsRtpArg;
			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief ����� ���̵�� ����� ������ �����Ѵ�.
 * @param pszUserId ����� ���̵�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CUserMap::Delete( const char * pszUserId )
{
	bool bRes = false;
	std::string strKey;
	USER_MAP::iterator itMap;
	USER_KEY_MAP::iterator itKeyMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		GetKey( itMap->second.m_strIp.c_str(), itMap->second.m_iPort, strKey );

		itKeyMap = m_clsKeyMap.find( strKey );
		if( itKeyMap != m_clsKeyMap.end() )
		{
			m_clsKeyMap.erase( itKeyMap );
		}

		m_clsMap.erase( itMap );

		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief ����� ������ �˻��Ѵ�.
 * @param pszUserId		����� ���̵�
 * @param clsUserInfo ����� ���� ���� ��ü
 * @returns �˻��Ǹ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CUserMap::Select( const char * pszUserId, CUserInfo & clsUserInfo )
{
	bool bRes = false;
	USER_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		clsUserInfo = itMap->second;

		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket Ŭ���̾�Ʈ ������ ����� ���̵� �˻��Ѵ�.
 * @param pszIp			WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iPort			WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 * @param strUserId ����� ���̵�
 * @returns �˻��Ǹ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CUserMap::SelectUserId( const char * pszIp, int iPort, std::string & strUserId )
{
	bool bRes = false;
	std::string strKey;
	USER_KEY_MAP::iterator itKeyMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itKeyMap = m_clsKeyMap.find( strKey );
	if( itKeyMap != m_clsKeyMap.end() )
	{
		strUserId = itKeyMap->second;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief �ڷᱸ���� ����� Ű�� �����Ѵ�.
 * @param pszIp		WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iPort		WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 * @param strKey	�ڷᱸ�� Ű
 */
void CUserMap::GetKey( const char * pszIp, int iPort, std::string & strKey )
{
	char szPort[11];

	snprintf( szPort, sizeof(szPort), "%d", iPort );

	strKey = pszIp;
	strKey.append( "_" );
	strKey.append( szPort );
}
