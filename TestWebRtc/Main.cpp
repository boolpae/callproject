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
#include "SipCallBack.h"
#include "Directory.h"
#include "Log.h"
#include "RtpThread.h"
#include "MemoryDebug.h"

int main( int argc, char * argv[] )
{
	if( argc < 3 )
	{
		printf( "[Usage] %s {local ip} {Document root path} {RSA key/cert pem file}\n", argv[0] );
		return 0;
	}

#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	CTcpStackSetup clsHttpSetup;
	CSipStackSetup clsSipSetup;

	const char * pszLocalIp = argv[1];
	const char * pszDocumentRoot = argv[2];
	const char * pszPemFile = NULL;

	if( argc >= 4 )
	{
		pszPemFile = argv[3];
	}

#ifdef WIN32
	CLog::SetDirectory( "c:\\temp\\http" );
#ifdef _DEBUG
	CLog::SetLevel( LOG_INFO | LOG_DEBUG | LOG_NETWORK );
#endif
#else
	CLog::SetDirectory( "/tmp/http" );
	CLog::SetLevel( LOG_INFO | LOG_DEBUG | LOG_NETWORK );
#endif

	InitNetwork();
	InitDtls();

	// HTTP ���� ��Ʈ ��ȣ�� �����Ѵ�.
	clsHttpSetup.m_iListenPort = 8080;
	clsHttpSetup.m_iMaxSocketPerThread = 1;
	clsHttpSetup.m_iThreadMaxCount = 0;
	clsHttpSetup.m_bUseThreadPipe = false;

	if( pszPemFile )
	{
		clsHttpSetup.m_iListenPort = 443;
		clsHttpSetup.m_bUseTls = true;
		clsHttpSetup.m_strCertFile = pszPemFile;
	}

	// HTTP �������� ����� Document root ������ �����Ѵ�.
	gclsHttpCallBack.m_strDocumentRoot = pszDocumentRoot;

	if( CDirectory::IsDirectory( gclsHttpCallBack.m_strDocumentRoot.c_str() ) == false )
	{
		printf( "[%s] is not directory\n", gclsHttpCallBack.m_strDocumentRoot.c_str() );
		return 0;
	}

	// HTTP ������ �����Ѵ�. HTTP ��û�� ���ŵǸ� �̿� ���� �̺�Ʈ�� CSimpleHttpServer ��ü�� �����Ѵ�.
	if( gclsHttpStack.Start( &clsHttpSetup, &gclsHttpCallBack ) == false )
	{
		printf( "gclsHttpStack.Start error\n" );
		return 0;
	}

	// 2�� �̻��� IP �ּҸ� ������ ���� �� �����Ƿ� SIP ��ſ� ����� IP �ּҸ� ���� ���ڷ� �޾Ƶ��̰� �ش� IP �ּҿ� ���� �뿪�� IP �ּҸ� RTP ������ ����Ѵ�.
	clsSipSetup.m_strLocalIp = pszLocalIp;
	clsSipSetup.m_iLocalUdpPort = 5085;
	clsSipSetup.m_iLocalTcpPort = 5085;
	gstrLocalIp = clsSipSetup.m_strLocalIp;

	if( gclsSipStack.Start( clsSipSetup, &gclsSipCallBack ) == false )
	{
		printf( "gclsSipStack.Start error\n" );
		return 0;
	}

	while( gclsHttpCallBack.m_bStop == false )
	{
		sleep(1);
	}

	FinalDtls();
	gclsHttpStack.Stop();
	gclsSipStack.Stop();
	SSLFinal();

	// ��� �����尡 ����� ������ ����Ѵ�.
	sleep(2);

#ifdef WIN32
	CLog::Release();
#endif

	return 0;
}
