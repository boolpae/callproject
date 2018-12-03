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

#ifndef _SIP_STACK_VERSION_H_
#define _SIP_STACK_VERSION_H_

#define SIP_STACK_VERSION "0.29"

/* ���� ����

= ���� 0.29 ( 2017�� 3�� 17�� ) =
 * ICT ���� SIP ������ ��� �����Ͽ��� Ring timeout �ð��� �� ���� �����ȴ�.

= ���� 0.28 ( 2017�� 1�� 3�� ) =
 * m_iTcpCallBackThreadCount ���� ����� �߰��� - �ϳ��� TCP ���ǿ� ���ؼ� N ���� callback ������� ������ �� �ִ�.

= ���� 0.27 ( 2016�� 9�� 20�� ) =
 * TLS ���� ���� ũ�⸦ 16384 �� ������
 * TCP/TLS SIP �޽��� ���� ũ�⸦ 16384 �� ������

= ���� 0.26 ( 2015�� 8�� 18�� ) =
 * stateful stateless ����� ������ �� �ִ�.
 * ������ TLS �������� TLS �޽����� �������� �� �ϵ��� ������

= ���� 0.25 ( 2015�� 8�� 11�� ) =
 * UDP ��Ʈ�� �������� �ʰ� ����� �� �ִ� ����� �߰���

= ���� 0.24 ( 2015�� 7�� 28�� ) =
 * Stack Stop �� ���� TCP ���� �� �ڷᱸ���� �ʱ�ȭ��Ų��.
 * Stack Stop �� ���� TCP ������ �ڷᱸ���� �ʱ�ȭ��Ų��.

= ���� 0.23 ( 2015�� 6�� 8�� ) =
 * TCP/TLS ���� timeout �� ���Ŀ� TCP/TLS ���� �ڷᱸ������ �����ϴ� ����� �߰���
 * TCP/TLS ���� ���� �����忡 ������ ����� ����Ǵ� ����� �߰���

= ���� 0.22 ( 2014�� 10�� 29�� ) =
 * IPv6 ����� �߰���

= ���� 0.21 ( 2014�� 6�� 10�� ) =
 * TCP / TLS Ŭ���̾�Ʈ�� �����ϴ� ����� �߰���
 * ����͸� ���ڿ��� CMonitorString �� ����ϵ��� ������

= ���� 0.20 ( 2014�� 4�� 22�� ) =
 * timer D, J ��� �ð� ���� ����� �߰���
 * ��� �Ϸ�� SIP �޽����� CSipDeleteQueue �� �������� �ʴ� ����� �߰���

= ���� 0.19 ( 2014�� 4�� 19�� ) =
 * SipStack ���� �ֱ⸦ ������ �� �ִ� ����� �߰���
 * callback �������̽� ���� ����� �߰���
 * IST �� 200 ������ ����� ��, 180 ������ ����� �� �ִ� ���׸� ��ġ��
 * IST ����Ʈ�� ACK �޽����� ������ ���� SIP Call-ID �� CSCeq �� ��ġ�ϴ��� �˻��ϴ� ����� �߰���

= ���� 0.18 ( 2013�� 10�� 31�� ) =
 * ThreadEnd �� ���� ���� �Լ����� ���� �Լ��� ������
 * SipStack.Stop �޼ҵ带 ȣ���ϸ� SipStack callback ����Ʈ�� �ʱ�ȭ��Ų��.

= ���� 0.16 ( 2013�� 9�� 4�� ) =
 * SIP stack �� ������ ��, ��� SIP �޽��� ť�� SIP �޽����� �����ϴ� ����� �߰���
 * callback �����尡 ������ ���� �̺�Ʈ �޼ҵ带 �߰���
 * SIP stack �� ������� ���� Ŭ���̾�Ʈ IP �ּ� ���͸� ����� �߰���
 * ���� UserAgent ����� security �������̽��� ������
 * openssl ���� ������ �� �ִ� ����� �߰���

= ���� 0.13 ( 2013�� 5�� 10�� ) =
 * SIP transaction list �� �������� ���� SIP User Agent ����Ʈ ���� ����� �߰���
 * SIP �޽����� ������ ���� compact form ���� �������� �����ϴ� ����� �߰���

= ���� 0.12 ( 2013�� 4�� 27�� ) =
 * ACK ���� timeout callback ����� �߰���
 * SipStack �� stack ���� ���ŵ� ���� ���������� ���� ���ŵǾ �߻��ϴ� ������ ���� ���׸� ��ġ��
 * ���� �������ݿ� ���� ���� ��Ʈ ��ȣ�� �������� �޼ҵ带 �߰���
 * Via ����� �߰��� ���� ���� �������ݿ� ���� ���� ��Ʈ�� ��Ȯ�ϰ� �Է��ϵ��� ������.
 * IST, NICT, NIST �� ���Ե� call-id ���ڿ��� �������� ����� �߰���
 * SIP Reason ��� �ļ�/���� Ŭ������ �߰���
 * SIP �޽����� ������ Ŭ���̾�Ʈ IP / Port �� SIP �޽����� �����ϴ� ����� �߰���
 * AddSipParameter �޼ҵ带 InsertSipParameter �޼ҵ�� �̸��� ������
 * UpdateSipParameter �޼ҵ带 �߰���
 * TLS ������� SIP ��û�� ���� ���� �޽����� ���۵��� �ʴ� ���׸� ��ġ��
 * TLS ������ ����� �� �ֵ��� ������

= ���� 0.07 ( 2012�� 12�� 04�� ) =
 * TCP �������ݷ� SIP �޽��� ����/���� ��� �߰� �Ϸ�

= ���� 0.06 ( 2012�� 10�� 30�� ) =
 * TCP �������ݷ� SIP �޽��� ����/���� ��� �߰���

= ���� 0.05 ( 2012�� 10�� 21�� ) =
 * VIA branch �� time ������ �־ SIP �޽����� ��ġ�� Ȯ���� ����.
 * VIA branch �� Call-Id �� system unique string �� �ִ� ����� �߰���.

= ���� 0.04 ( 2012�� 08�� 21�� ) =
 * SendSipMessage �޼ҵ忡�� SIP stack �� CSipMessage �� �Է����� �� �ϸ� CSipMessage �� �����ϵ��� ������.
 * SendSipMessage �޼ҵ� ȣ�� ���нÿ� CSipMessage �� �����ϴ� �ڵ带 ������.

= ���� 0.03 ( 2012�� 08�� 18�� ) =
 * UserAgent ����� ���� ���α׷����� ������ �� �ִ� ����� �߰���
 * SIP �޽����� �����ϱ� ���� 2�� SIP �޽����� �����ϴ� ����� 1���� �����ϵ��� ������.

*/

#endif