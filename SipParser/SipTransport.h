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

#ifndef _SIP_TRANSPORT_H_
#define _SIP_TRANSPORT_H_

enum ESipTransport
{
	E_SIP_UDP = 0,
	E_SIP_TCP,
	E_SIP_TLS
};

#define S_SIP_UDP		"UDP"
#define S_SIP_TCP		"TCP"	
#define S_SIP_TLS		"TLS"

const char * SipGetTransport( ESipTransport eTransport );
const char * SipGetProtocol( ESipTransport eTransport );

#endif
