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

#include "TestOpenssl.h"

int giTcpServerConnectCount = 1;
int giTcpServerLoopCount = 1000;
int giTcpSendPeriod = 10000;
char * gpszServerIp = "127.0.0.1";
bool gbTcpClientRenegotiate = true;
bool gbTcpServerRenegotiate = false;

int main( int argc, char * argv[] )
{
#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( argc == 2 )
	{
		if( !strcmp( argv[1], "AES256" ) )
		{
			AES256();
			return 0;
		}
	}
	else if( argc == 1 )
	{
		printf( "[Usage] %s {pem filename} {tcpserver|tcpclient|tcp|udp} {server ip}\n", argv[0] );
		return 0;
	}

	InitNetwork();

	if( argc >= 4 )
	{
		gpszServerIp = argv[3];
	}

	if( !strcmp( argv[2], "tcpserver" ) )
	{
		// TLS ������ �����Ѵ�.
		TcpServer( argv[1] );
	}
	else if( !strcmp( argv[2], "tcpclient" ) )
	{
		// TLS Ŭ���̾�Ʈ�� �����Ѵ�.
		TcpClient( argv[1] );
	}
	else if( !strcmp( argv[2], "tcp" ) )
	{
		// TLS ����/Ŭ���̾�Ʈ�� �����Ѵ�.
		TcpServerClient( argv[1] );
	}
	else if( strcmp( argv[2], "dtls" ) )
	{
		// DTLS ����/Ŭ���̾�Ʈ�� �����Ѵ�.
		UdpServer( argv[1] );
	}

	return 0;
}
