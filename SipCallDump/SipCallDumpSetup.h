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

#ifndef _SIP_CALL_DUMP_SETUP_H_
#define _SIP_CALL_DUMP_SETUP_H_

#define DEFAULT_PACKET_SNAP_LEN					2048
#define DEFAULT_PACKET_READ_TIMEOUT			200

#include "XmlElement.h"

/**
 * @ingroup SipCallDump
 * @brief ���� ������ �о �����ϴ� Ŭ����
 */
class CSipCallDumpSetup
{
public:
	CSipCallDumpSetup();
	~CSipCallDumpSetup();

	bool Read( const char * pszFileName );
	bool Read( );

	bool IsChange();

	// ��Ŷ ���� ����
	std::string	m_strPacketDevice;	// pcap_open �� 1��° ���ڰ� - ��Ŷ ĸó ����̽� �̸�
	int		m_iPacketSnapLen;					// pcap_open �� 2��° ���ڰ�
	int		m_iPacketReadTimeout;			// pcap_open �� 4��° ���ڰ�
	std::string m_strPacketFolder;	// pcap ������ ������ ���� full path

	// RTP
	int		m_iRtpRecvTimeout;				// RTP ���� timeout �ð� - ���⿡ ������ �ð����� RTP �� ���ŵ��� �ʾҴٸ� ��ȭ ���� ó���Ѵ�.

private:
	bool Read( CXmlElement & clsXml );
	void SetFileSizeTime( );

	std::string	m_strFileName;	// ���� ���� �̸�
	time_t			m_iFileTime;		// ���� ���� ���� �ð�
	int					m_iFileSize;		// ���� ���� ũ��
};

extern CSipCallDumpSetup gclsSetup;

#endif
