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

#include "SipCallDumpSetup.h"
#include "Log.h"
#include <sys/stat.h>
#include "MemoryDebug.h"

CSipCallDumpSetup gclsSetup;

CSipCallDumpSetup::CSipCallDumpSetup() : m_iPacketSnapLen(DEFAULT_PACKET_SNAP_LEN), m_iPacketReadTimeout(DEFAULT_PACKET_READ_TIMEOUT), m_iRtpRecvTimeout(30)
{
}

CSipCallDumpSetup::~CSipCallDumpSetup()
{
}


/**
 * @ingroup SipCallDump
 * @brief ���� ������ �о ��� ������ �����Ѵ�.
 * @param pszFileName ���� ���� full path
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CSipCallDumpSetup::Read( const char * pszFileName )
{
	CXmlElement clsXml, * pclsElement;

	if( clsXml.ParseFile( pszFileName ) == false ) return false;

	pclsElement = clsXml.SelectElement( "PacketDump" );
	if( pclsElement == NULL )
	{
		printf( "Setup PacketDump is not defined\n" );
		return false;
	}

	pclsElement->SelectElementTrimData( "Device", m_strPacketDevice );
	pclsElement->SelectElementTrimData( "Folder", m_strPacketFolder );

	if( m_strPacketDevice.empty() )
	{
		printf( "Setup PacketDump > Device is not defined\n" );
		return false;
	}

	if( m_strPacketFolder.empty() )
	{
		printf( "Setup PacketDump > Folder is not defined\n" );
		return false;
	}

	CDirectory::Create( m_strPacketFolder.c_str() );

	pclsElement->SelectElementData( "PacketSnapLen", m_iPacketSnapLen );
	pclsElement->SelectElementData( "PacketReadTimeout", m_iPacketReadTimeout );

	Read( clsXml );

	m_strFileName = pszFileName;
	SetFileSizeTime();

	return true;
}

/**
 * @ingroup SipCallDump
 * @brief ������ ���� ������ �д´�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CSipCallDumpSetup::Read( )
{
	if( m_strFileName.length() == 0 ) return false;

	CXmlElement clsXml;

	if( clsXml.ParseFile( m_strFileName.c_str() ) == false ) return false;

	Read( clsXml );
	SetFileSizeTime();

	return true;
}

/**
 * @ingroup SipCallDump
 * @brief ���� ������ ���� �߿��� �ǽð����� ���� ������ �׸��� �ٽ� �����Ѵ�.
 * @param clsXml ���� ������ ������ ������ ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CSipCallDumpSetup::Read( CXmlElement & clsXml )
{
	CXmlElement * pclsElement;

	// RTP
	pclsElement = clsXml.SelectElement( "Rtp" );
	if( pclsElement )
	{
		pclsElement->SelectElementData( "RecvTimeout", m_iRtpRecvTimeout );
	}

	// �α�
	pclsElement = clsXml.SelectElement( "Log" );
	if( pclsElement )
	{
		int iLogLevel = 0, iLogMaxSize = 0;
		std::string strLogFolder;

		pclsElement->SelectElementTrimData( "Folder", strLogFolder );
		if( strLogFolder.empty() )
		{
			printf( "Setup Log > Folder is not defined\n" );
			return false;
		}

		CLog::SetDirectory( strLogFolder.c_str() );

		CXmlElement * pclsClient = pclsElement->SelectElement( "Level" );
		if( pclsClient )
		{
			bool bTemp;

			pclsClient->SelectAttribute( "Debug", bTemp );
			if( bTemp ) iLogLevel |= LOG_DEBUG;

			pclsClient->SelectAttribute( "Info", bTemp );
			if( bTemp ) iLogLevel |= LOG_INFO;

			pclsClient->SelectAttribute( "Network", bTemp );
			if( bTemp ) iLogLevel |= LOG_NETWORK;

			pclsClient->SelectAttribute( "Sql", bTemp );
			if( bTemp ) iLogLevel |= LOG_SQL;
		}

		pclsElement->SelectElementData( "MaxSize", iLogMaxSize );

		CLog::SetLevel( iLogLevel );
		CLog::SetMaxLogSize( iLogMaxSize );
	}

	return true;
}

/**
 * @ingroup SipCallDump
 * @brief ���������� �����Ǿ����� Ȯ���Ѵ�.
 * @returns ���������� �����Ǿ����� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CSipCallDumpSetup::IsChange()
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		if( m_iFileSize != clsStat.st_size || m_iFileTime != clsStat.st_mtime ) return true;
	}

	return false;
}

/**
 * @ingroup SipCallDump
 * @brief ���������� ���� �ð��� �����Ѵ�.
 */
void CSipCallDumpSetup::SetFileSizeTime( )
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		m_iFileSize = clsStat.st_size;
		m_iFileTime = clsStat.st_mtime;
	}
}
