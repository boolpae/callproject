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

#include "HttpCallBack.h"
#include "HttpStatusCode.h"
#include "FileUtility.h"
#include "Directory.h"
#include "Log.h"
#include "UserMap.h"
#include "RtpThread.h"
#include "MemoryDebug.h"

CHttpStack gclsHttpStack;
CHttpCallBack	gclsHttpCallBack;

CHttpCallBack::CHttpCallBack() : m_bStop(false)
{
}

CHttpCallBack::~CHttpCallBack()
{
}

/**
 * @ingroup TestWebRtc
 * @brief HTTP ��û ���� �̺�Ʈ callback
 * @param pclsRequest		HTTP ��û �޽���
 * @param pclsResponse	HTTP ���� �޽��� - ���뿡�� �����Ѵ�.
 * @returns ���뿡�� HTTP ���� �޽����� ���������� �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CHttpCallBack::RecvHttpRequest( CHttpMessage * pclsRequest, CHttpMessage * pclsResponse )
{
	std::string strPath = m_strDocumentRoot;
	std::string strExt;

	//CLog::Print( LOG_DEBUG, "req uri[%s]", pclsRequest->m_strReqUri.c_str() );

	// ���Ȼ� .. �� ������ URL �� �����Ѵ�.
	if( strstr( pclsRequest->m_strReqUri.c_str(), ".." ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

#ifdef _DEBUG
	// �޸� ���� �˻縦 ���ؼ� exit.html �� �����ϸ� ���α׷��� �����Ѵ�.
	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/exit.html" ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		m_bStop = true;
		return true;
	}
#endif

	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/" ) )
	{
		CDirectory::AppendName( strPath, "index.html" );
	}
	else
	{
#ifdef WIN32
		ReplaceString( pclsRequest->m_strReqUri, "/", "\\" );
#endif

		strPath.append( pclsRequest->m_strReqUri );
	}

	if( IsExistFile( strPath.c_str() ) == false )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// ���Ϻ� Content-Type �� �����Ѵ�.
	GetFileExt( strPath.c_str(), strExt );
	const char * pszExt = strExt.c_str();
	
	if( !strcmp( pszExt, "html" ) || !strcmp( pszExt, "htm" ) )
	{
		pclsResponse->m_strContentType = "text/html";
	}
	else if( !strcmp( pszExt, "css" ) )
	{
		pclsResponse->m_strContentType = "text/css";
	}
	else if( !strcmp( pszExt, "js" ) )
	{
		pclsResponse->m_strContentType = "text/javascript";
	}
	else if( !strcmp( pszExt, "png" ) || !strcmp( pszExt, "gif" ) )
	{
		pclsResponse->m_strContentType = "image/";
		pclsResponse->m_strContentType.append( pszExt );
	}
	else if( !strcmp( pszExt, "jpg" ) || !strcmp( pszExt, "jpeg" ) )
	{
		pclsResponse->m_strContentType = "image/jpeg";
	}
	else
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// ������ �о HTTP body �� �����Ѵ�.
	FILE * fd = fopen( strPath.c_str(), "rb" );
	if( fd == NULL )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	int n;
	char szBuf[8192];

	while( ( n = fread( szBuf, 1, sizeof(szBuf), fd ) ) > 0 )
	{
		pclsResponse->m_strBody.append( szBuf, n );
	}

	fclose( fd );

	pclsResponse->m_iStatusCode = HTTP_OK;

	return true;
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket Ŭ���̾�Ʈ TCP ���� ���� �̺�Ʈ callback
 * @param pszClientIp WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iClientPort WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 */
void CHttpCallBack::WebSocketConnected( const char * pszClientIp, int iClientPort )
{
	printf( "WebSocket[%s:%d] connected\n", pszClientIp, iClientPort );
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket Ŭ���̾�Ʈ TCP ���� ���� �̺�Ʈ callback
 * @param pszClientIp WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iClientPort WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 */
void CHttpCallBack::WebSocketClosed( const char * pszClientIp, int iClientPort )
{
	printf( "WebSocket[%s:%d] closed\n", pszClientIp, iClientPort );

	std::string strUserId;
	CUserInfo clsUserInfo;

	if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) )
	{
		if( gclsUserMap.Select( strUserId.c_str(), clsUserInfo ) )
		{
			gclsUserMap.Delete( strUserId.c_str() );
		}
	}
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket Ŭ���̾�Ʈ ������ ���� �̺�Ʈ callback
 * @param pszClientIp WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iClientPort WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 * @param strData			WebSocket Ŭ���̾�Ʈ�� ������ ������
 * @param pclsSession	HTTP ���� ����
 * @returns WebSocket Ŭ���̾�Ʈ ������ �����Ϸ��� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CHttpCallBack::WebSocketData( const char * pszClientIp, int iClientPort, std::string & strData, CHttpStackSession * pclsSession )
{
	CLog::Print( LOG_NETWORK, "WebSocket[%s:%d] recv[%s]", pszClientIp, iClientPort, strData.c_str() );
	printf( "WebSocket[%s:%d] recv[%s]\n", pszClientIp, iClientPort, strData.c_str() );

	STRING_VECTOR clsList;

	SplitString( strData.c_str(), clsList, '|' );

	int iCount = clsList.size();

	if( iCount < 2 )
	{
		return false;
	}

	bool bReq = true;
	if( strcmp( clsList[0].c_str(), "req" ) ) bReq = false;

	const char * pszCommand = clsList[1].c_str();
	std::string strUserId;

	if( !strcmp( pszCommand, "register" ) )
	{
		if( iCount < 5 )
		{
			printf( "register request arg is not correct\n" );
			return false;
		}

		if( gclsUserMap.Insert( clsList[2].c_str(), clsList[3].c_str(), clsList[4].c_str(), pszClientIp, iClientPort, pclsSession->m_strUserAgent.c_str() ) == false )
		{
			Send( pszClientIp, iClientPort, "res|register|500" );
		}
		else
		{
			Send( pszClientIp, iClientPort, "res|register|200" );
		}
	}
	else if( !strcmp( pszCommand, "invite" ) )
	{
		if( bReq )
		{
			if( iCount < 4 )
			{
				printf( "invite request arg is not correct\n" );
				return false;
			}

			const char * pszToId = clsList[2].c_str();
			const char * pszSdp = clsList[3].c_str();
			CUserInfo clsUserInfo;

			CRtpThreadArg * pclsRtpArg = new CRtpThreadArg();
			if( pclsRtpArg == NULL )
			{
				Send( pszClientIp, iClientPort, "res|invite|500" );
				return true;
			}

			pclsRtpArg->m_strUserId = strUserId;
			pclsRtpArg->m_strToId = pszToId;
			pclsRtpArg->m_strSdp = pszSdp;
			pclsRtpArg->m_bStartCall = true;

			if( pclsRtpArg->CreateSocket() == false || 
					gclsUserMap.Update( strUserId.c_str(), pclsRtpArg, false ) == false ||
					StartRtpThread( pclsRtpArg ) == false )
			{
				delete pclsRtpArg;
				Send( pszClientIp, iClientPort, "res|invite|500" );
				return true;
			}
		}
		else
		{
			if( iCount < 3 )
			{
				printf( "invite response arg is not correct\n" );
				return false;
			}

			CUserInfo clsUserInfo;

			if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false || gclsUserMap.Select( strUserId.c_str(), clsUserInfo ) == false )
			{
				return true;
			}

			int iStatus = atoi( clsList[2].c_str() );

			if( iStatus == 200 && iCount >= 4 )
			{
				CRtpThreadArg * pclsRtpArg = new CRtpThreadArg();
				if( pclsRtpArg == NULL )
				{
					Send( pszClientIp, iClientPort, "req|bye" );
					return true;
				}

				const char * pszSdp = clsList[3].c_str();

				pclsRtpArg->m_strUserId = strUserId;
				pclsRtpArg->m_strSdp = pszSdp;
				pclsRtpArg->m_bStartCall = false;

				if( pclsRtpArg->CreateSocket() == false || 
						gclsUserMap.Update( strUserId.c_str(), pclsRtpArg, false ) == false ||
						StartRtpThread( pclsRtpArg ) == false )
				{
					delete pclsRtpArg;
					Send( pszClientIp, iClientPort, "req|bye" );
					return true;
				}
			}
		}
	}
	else if( !strcmp( pszCommand, "bye" ) )
	{
		std::string strUserId;
		CUserInfo clsUserInfo;

		if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false || gclsUserMap.Select( strUserId.c_str(), clsUserInfo ) == false )
		{
			Send( pszClientIp, iClientPort, "res|invite|403" );
			return true;
		}

		if( clsUserInfo.m_pclsRtpArg )
		{
			clsUserInfo.m_pclsRtpArg->m_bStop = true;
			gclsUserMap.Update( strUserId.c_str(), clsUserInfo.m_pclsRtpArg, true );
		}
	}

	return true;
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket Ŭ���̾�Ʈ�� ��Ŷ�� �����Ѵ�.
 * @param pszClientIp WebSocket Ŭ���̾�Ʈ IP �ּ�
 * @param iClientPort WebSocket Ŭ���̾�Ʈ ��Ʈ ��ȣ
 * @param fmt					���� ���ڿ�
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CHttpCallBack::Send( const char * pszClientIp, int iClientPort, const char * fmt, ... )
{
	va_list	ap;
	char		szBuf[8192];
	int			iBufLen;

	va_start( ap, fmt );
	iBufLen = vsnprintf( szBuf, sizeof(szBuf)-1, fmt, ap );
	va_end( ap );

	if( gclsHttpStack.SendWebSocketPacket( pszClientIp, iClientPort, szBuf, iBufLen ) )
	{
		printf( "WebSocket[%s:%d] send[%s]\n", pszClientIp, iClientPort, szBuf );
		CLog::Print( LOG_NETWORK, "WebSocket[%s:%d] send[%s]", pszClientIp, iClientPort, szBuf );
		return true;
	}

	return false;
}
