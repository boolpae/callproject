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

#include "TestStunParser.h"

/**
 * @ingroup TestStunParser
 * @brief STUN ��Ŷ�� �Ľ��ϰ� �ٽ� �����Ͽ��� ������ �߻��ϴ��� �˻��Ѵ�.
 * @param pszInput	HEX ���ڿ��� ����� STUN ��Ŷ
 * @param pszPw			STUN �����ڿ��� ������ ice-pwd
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
static bool TestStunMessage( const char * pszInput, const char * pszPw )
{
	CStunMessage clsMessage;
	std::string strPacket, strOutput;
	char szPacket[1500];
	int iPacketLen;

	memset( szPacket, 0, sizeof(szPacket) );

	HexToString( pszInput, strPacket );

	if( clsMessage.Parse( strPacket.c_str(), strPacket.length() ) == -1 ) 
	{
		printf( "%s clsMessage.Parse(%s) error\n", __FUNCTION__, pszInput );
		return false;
	}

	clsMessage.m_strPassword = pszPw;
	iPacketLen = clsMessage.ToString( szPacket, sizeof(szPacket) );
	if( iPacketLen == -1 ) 
	{
		printf( "%s clsMessage.ToString(%s) error\n", __FUNCTION__, pszInput );
		return false;
	}

	StringToHex( szPacket, iPacketLen, strOutput );

	if( strcmp( pszInput, strOutput.c_str() ) )
	{
		printf( "encode error\n" );
		printf( "input [%s]\n", pszInput );
		printf( "output[%s]\n", strOutput.c_str() );
		return false;
	}

	return true;
}

bool TestStunMessage()
{
	// A -> B �� WebRTC ��ȭ ��û�� �����Ͽ��� ���� SDP �� (A), (B) ��� ǥ���Ѵٸ�
	// A -> B �� ������ STUN ��û ��Ŷ�� B -> A �� ������ STUN ���� ��Ŷ�� MESSAGE-INTEGRITY ����� ���ؼ� (B) �� ice-pwd �� ����Ѵ�.
	// B -> A �� ������ STUN ��û ��Ŷ�� A -> B �� ������ STUN ���� ��Ŷ�� MESSAGE-INTEGRITY ����� ���ؼ� (A) �� ice-pwd �� ����Ѵ�.

	// Binding Request User ( A -> B )
	if( TestStunMessage( "000100542112a4428e24a0e78f0a741a2ae42777000600096c4d52623a6c4d526b000000002400046efffaff802a0008000000000044d73a80540001330000008070000400000003000800140f2da18f9c1c439cd217ee85dfd8e15f25bfb283802800041e370fad"
		, "FNPRfT4qUaVOKa0ivkn64mMY" ) == false ) return false;

	// Binding Success Response ( B -> A )
	if( TestStunMessage( "010100342112a4428e24a0e78f0a741a2ae427770020000800010720e1baa46280700004000000030008001411d451d31710a08026478975871baf48e001ef428028000446cfd3d1"
		, "FNPRfT4qUaVOKa0ivkn64mMY" ) == false ) return false;

	// Binding Request User ( B -> A )
	if( TestStunMessage( "000100542112a442cec325aeb2bb95978bb36156000600096c4d526b3a6c4d5262000000002400046efffeff802900080000000038b4c8538054000131000000807000040000000300080014d7f156b39b8a01a3302cd4ae8bb1a874d3548d8880280004240f309b"
		, "nFNRfT4UabEOKa00ivn64MtQ" ) == false ) return false;

	// Binding Success Response ( A -> B )
	if( TestStunMessage( "010100342112a442cec325aeb2bb95978bb3615600200008000167dee1baa45d807000040000000300080014494fd190005dbecaa1dc9c169e97cca06a3bbc30802800049befef2b"
		, "nFNRfT4UabEOKa00ivn64MtQ" ) == false ) return false;

	return true;
}
