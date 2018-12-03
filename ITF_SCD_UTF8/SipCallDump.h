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

#ifndef _SIP_CALL_DUMP_H_
#define _SIP_CALL_DUMP_H_

// MS 윈도우 서비스 선언
#define SERVICE_NAME								"ITF_SCD"		// 서비스 이름
#define SERVICE_DISPLAY_NAME				"ITF_SCD"
#define SERVICE_DESCRIPTION_STRING	"SIP Call & Voice Dump , ITFACT"
#define SERVER_VERSION							"0.01"

#define CONFIG_FILENAME						"scd.xml"

#include "SipCallDumpSetup.h"
#include "Log.h"

// PacketDumpThread.cpp
bool StartPacketDumpThread( );

// PacketDivideThread.cpp for ITF_SCD
bool StartPacketDivideThread( );

#endif
