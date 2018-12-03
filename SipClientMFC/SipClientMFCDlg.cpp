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

#include "stdafx.h"
#include "SipClientMFC.h"
#include "SipClientMFCDlg.h"
#include "SipSetupDlg.h"
#include "Setup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSipClientMFCDlg dialog

CSipClientMFCDlg::CSipClientMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSipClientMFCDlg::IDD, pParent)
	, m_strLog(_T(""))
	, m_strCallNumber(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSipClientMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_STACK, m_btnStartStack);
	DDX_Control(pDX, IDC_STOP_STACK, m_btnStopStack);
	DDX_Control(pDX, IDC_START_CALL, m_btnStartCall);
	DDX_Control(pDX, IDC_STOP_CALL, m_btnStopCall);
	DDX_Control(pDX, IDC_ACCEPT_CALL, m_btnAcceptCall);
	DDX_Text(pDX, IDC_LOG, m_strLog);
	DDX_Text(pDX, IDC_CALL_NUMBER, m_strCallNumber);
}

BEGIN_MESSAGE_MAP(CSipClientMFCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETUP, &CSipClientMFCDlg::OnBnClickedSetup)
	ON_BN_CLICKED(IDC_START_STACK, &CSipClientMFCDlg::OnBnClickedStartStack)
	ON_BN_CLICKED(IDC_STOP_STACK, &CSipClientMFCDlg::OnBnClickedStopStack)
	ON_BN_CLICKED(IDC_START_CALL, &CSipClientMFCDlg::OnBnClickedStartCall)
	ON_BN_CLICKED(IDC_STOP_CALL, &CSipClientMFCDlg::OnBnClickedStopCall)
	ON_BN_CLICKED(IDC_ACCEPT_CALL, &CSipClientMFCDlg::OnBnClickedAcceptCall)
	ON_MESSAGE(SIP_MESSAGE_ID, &CSipClientMFCDlg::OnSipMessage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CSipClientMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	gclsSetup.Get();

	m_btnStartCall.EnableWindow( FALSE );
	m_btnStopCall.EnableWindow( FALSE );
	m_btnAcceptCall.EnableWindow( FALSE );
	m_btnStopStack.EnableWindow( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSipClientMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CSipClientMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSipClientMFCDlg::OnOK()
{

}

void CSipClientMFCDlg::OnBnClickedSetup()
{
	CSipSetupDlg clsDlg;

	clsDlg.DoModal();
}

void CSipClientMFCDlg::OnBnClickedStartStack()
{
	if( gclsSetup.IsSet() == false )
	{
		MessageBox( "Please~ click setup button", "Information", MB_OK );
		return;
	}

	m_clsSipUserAgentMFC.SetWindowHandle( GetSafeHwnd() );
	m_clsSipUserAgentMFC.SetCallBack( this );

	CSipStackSetup clsSetup;
	CSipServerInfo clsInfo;

	if( gclsSetup.m_strLocalIp.empty() )
	{
		GetLocalIp( clsSetup.m_strLocalIp );
	}
	else
	{
		clsSetup.m_strLocalIp = gclsSetup.m_strLocalIp;
	}

	m_strLocalIp = clsSetup.m_strLocalIp;
	
	clsInfo.m_strIp = gclsSetup.m_strSipServerIp;
	clsInfo.m_iPort = gclsSetup.m_iSipServerPort;
	clsInfo.m_strDomain = gclsSetup.m_strSipDomain;
	clsInfo.m_strUserId = gclsSetup.m_strUserId;
	clsInfo.m_strPassWord = gclsSetup.m_strPassWord;

	const char * pszProtocol = gclsSetup.m_strProtocol.c_str();

	if( !strcmp( pszProtocol, S_SIP_TCP ) )
	{
		clsInfo.m_eTransport = E_SIP_TCP;
	}
	else if( !strcmp( pszProtocol, S_SIP_TLS ) )
	{
		clsInfo.m_eTransport = E_SIP_TLS;
		clsSetup.m_bTlsClient = true;
	}
	else
	{
		clsInfo.m_eTransport = E_SIP_UDP;
	}

	m_clsSipUserAgent.InsertRegisterInfo( clsInfo );

	clsSetup.m_strUserAgent = gclsSetup.m_strUserAgent;

	bool bSuccess = false;

	for( int i = 0; i < 100; ++i )
	{
		clsSetup.m_iLocalUdpPort = i + 10000;

		if( clsInfo.m_eTransport == E_SIP_TCP )
		{
			clsSetup.m_iLocalTcpPort = clsSetup.m_iLocalUdpPort;
		}

		if( m_clsSipUserAgent.Start( clsSetup, &m_clsSipUserAgentMFC ) )
		{
			bSuccess = true;
			break;
		}
	}

	if( bSuccess == false )
	{
		MessageBox( "sip stack start error", "Error", MB_OK );
		return;
	}

	m_btnStartStack.EnableWindow( FALSE );
	m_btnStopStack.EnableWindow( TRUE );
}

void CSipClientMFCDlg::OnBnClickedStopStack()
{
	m_clsAudioThread.Stop();
	m_clsSipUserAgent.Stop();

	m_btnStartStack.EnableWindow( TRUE );
	m_btnStopStack.EnableWindow( FALSE );
}

void CSipClientMFCDlg::OnBnClickedStartCall()
{
	UpdateData(TRUE);

	if( m_strCallNumber.GetLength() <= 0 )
	{
		MessageBox( "Please~ insert call number", "Info", MB_OK | MB_ICONWARNING );
		return;
	}

	if( m_clsAudioThread.Start() == false )
	{
		MessageBox( "AudioThread start error", "Error", MB_OK | MB_ICONERROR );
		return;
	}

	CSipCallRtp clsRtp;
	CSipCallRoute clsRoute;

	clsRtp.m_strIp = m_strLocalIp;
	clsRtp.m_iPort = m_clsAudioThread.m_iPort;
	clsRtp.m_iCodec = 0;

	clsRoute.m_strDestIp = gclsSetup.m_strSipServerIp;
	clsRoute.m_iDestPort = gclsSetup.m_iSipServerPort;

	const char * pszProtocol = gclsSetup.m_strProtocol.c_str();

	if( !strcmp( pszProtocol, S_SIP_TCP ) )
	{
		clsRoute.m_eTransport = E_SIP_TCP;
	}
	else if( !strcmp( pszProtocol, S_SIP_TLS ) )
	{
		clsRoute.m_eTransport = E_SIP_TLS;
	}

	if( m_clsSipUserAgent.StartCall( gclsSetup.m_strUserId.c_str(), m_strCallNumber, &clsRtp, &clsRoute, m_strCallId ) == false )
	{
		MessageBox( "StartCall error", "Error", MB_OK );
		return;
	}

	m_btnStartCall.EnableWindow( FALSE );
	m_btnStopCall.EnableWindow( TRUE );
}

void CSipClientMFCDlg::OnBnClickedStopCall()
{
	m_clsAudioThread.Stop();
	m_clsSipUserAgent.StopCall( m_strCallId.c_str() );
	m_strCallId.clear();

	m_btnStartCall.EnableWindow( TRUE );
	m_btnStopCall.EnableWindow( FALSE );
	m_btnAcceptCall.EnableWindow( FALSE );
}

void CSipClientMFCDlg::OnBnClickedAcceptCall()
{
	if( m_clsAudioThread.Start() == false )
	{
		OnBnClickedStopCall();
		return;
	}

	CSipCallRtp clsRtp;

	clsRtp.m_strIp = m_strLocalIp;
	clsRtp.m_iPort = m_clsAudioThread.m_iPort;	
	clsRtp.m_iCodec = 0;

	if( m_clsSipUserAgent.AcceptCall( m_strCallId.c_str(), &clsRtp ) )
	{
		m_btnAcceptCall.EnableWindow( FALSE );
	}
}

LRESULT CSipClientMFCDlg::OnSipMessage( WPARAM wParam, LPARAM lParam )
{
	return m_clsSipUserAgentMFC.OnSipMessage( wParam, lParam );
}

void CSipClientMFCDlg::SetLog( const char * fmt, ... )
{
	va_list		ap;
	char			szBuf[8192];

	va_start( ap, fmt );
	vsnprintf( szBuf, sizeof(szBuf)-1, fmt, ap );
	va_end( ap );

	m_strLog = szBuf;
	UpdateData(FALSE);
}

/**
 * @ingroup SipClientMFC
 * @brief SIP REGISTER ���� �޽��� ���� �̺�Ʈ �ڵ鷯
 * @param pclsInfo	SIP REGISTER ���� �޽����� ������ IP-PBX ���� ���� ��ü
 * @param iStatus		SIP REGISTER ���� �ڵ�
 */
void CSipClientMFCDlg::EventRegister( CSipServerInfo * pclsInfo, int iStatus )
{
	if( pclsInfo->m_bLogin && iStatus == SIP_OK )
	{
		m_btnStartCall.EnableWindow( TRUE );
	}
	else
	{
		m_btnStartCall.EnableWindow( FALSE );
		m_btnStopCall.EnableWindow( FALSE );
		m_btnAcceptCall.EnableWindow( FALSE );
	}

	SetLog( "%s status(%d)", __FUNCTION__, iStatus );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP ��ȭ ��û ���� �̺�Ʈ �ڵ鷯
 * @param	pszCallId	SIP Call-ID
 * @param pszFrom		SIP From ����� ���̵�
 * @param pszTo			SIP To ����� ���̵�
 * @param pclsRtp		RTP ���� ���� ��ü
 */
void CSipClientMFCDlg::EventIncomingCall( const char * pszCallId, const char * pszFrom, const char * pszTo, CSipCallRtp * pclsRtp )
{
	if( m_strCallId.empty() == false )
	{
		m_clsSipUserAgent.StopCall( pszCallId, SIP_BUSY_HERE );
		return;
	}

	m_btnStartCall.EnableWindow( FALSE );
	m_btnStopCall.EnableWindow( TRUE );
	m_btnAcceptCall.EnableWindow( TRUE );

	m_strCallId = pszCallId;

	m_clsAudioThread.SetDestIpPort( pclsRtp->m_strIp.c_str(), pclsRtp->m_iPort );

	SetLog( "%s (%s)", __FUNCTION__, pszFrom );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP Ring / Session Progress ���� �̺�Ʈ �ڵ鷯
 * @param	pszCallId		SIP Call-ID
 * @param iSipStatus	SIP ���� �ڵ�
 * @param pclsRtp			RTP ���� ���� ��ü
 */
void CSipClientMFCDlg::EventCallRing( const char * pszCallId, int iSipStatus, CSipCallRtp * pclsRtp )
{
	SetLog( "%s (%d)", __FUNCTION__, iSipStatus );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP ��ȭ ���� �̺�Ʈ �ڵ鷯
 * @param	pszCallId	SIP Call-ID
 * @param pclsRtp		RTP ���� ���� ��ü
 */
void CSipClientMFCDlg::EventCallStart( const char * pszCallId, CSipCallRtp * pclsRtp )
{
	m_clsAudioThread.SetDestIpPort( pclsRtp->m_strIp.c_str(), pclsRtp->m_iPort );

	m_btnStartCall.EnableWindow( FALSE );
	m_btnStopCall.EnableWindow( TRUE );
	m_btnAcceptCall.EnableWindow( FALSE );

	SetLog( "%s", __FUNCTION__ );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP ��ȭ ���� �̺�Ʈ �ڵ鷯
 * @param	pszCallId		SIP Call-ID
 * @param iSipStatus	SIP ���� �ڵ�. INVITE �� ���� ���� �������� ��ȭ�� ����� ���, INVITE �� ���� �ڵ带 �����Ѵ�.
 */
void CSipClientMFCDlg::EventCallEnd( const char * pszCallId, int iSipStatus )
{
	m_clsAudioThread.Stop();

	m_btnStartCall.EnableWindow( TRUE );
	m_btnStopCall.EnableWindow( FALSE );
	m_btnAcceptCall.EnableWindow( FALSE );

	m_strCallId.clear();
	
	SetLog( "%s (%d)", __FUNCTION__, iSipStatus );
}

/**
 * @ingroup SipClientMFC
 * @brief SIP ReINVITE ���� �̺�Ʈ �ڵ鷯
 * @param	pszCallId	SIP Call-ID
 * @param pclsRemoteRtp		���� RTP ���� ���� ��ü
 * @param pclsLocalRtp		�� RTP ���� ���� ��ü
 */
void CSipClientMFCDlg::EventReInvite( const char * pszCallId, CSipCallRtp * pclsRemoteRtp, CSipCallRtp * pclsLocalRtp )
{
}

/**
 * @ingroup SipClientMFC
 * @brief Screened / Unscreened Transfer ��û ���� �̺�Ʈ �ڵ鷯
 * @param pszCallId					SIP Call-ID
 * @param pszReferToCallId	��ȭ�� ���޵� SIP Call-ID
 * @param bScreenedTransfer Screened Transfer �̸� true �� �Էµǰ� Unscreened Transfer �̸� false �� �Էµȴ�.
 * @returns ��û�� �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CSipClientMFCDlg::EventTransfer( const char * pszCallId, const char * pszReferToCallId, bool bScreenedTransfer )
{
	return false;
}

/**
 * @ingroup SipClientMFC
 * @brief Blind Transfer ��û ���� �̺�Ʈ �ڵ鷯
 * @param pszCallId			SIP Call-ID
 * @param pszReferToId	��ȭ�� ���޵� ����� ���̵�
 * @returns ��û�� �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CSipClientMFCDlg::EventBlindTransfer( const char * pszCallId, const char * pszReferToId )
{
	return false;
}

/**
 * @ingroup SipClientMFC
 * @brief SIP MESSAGE ���� �̺�Ʈ �ڵ鷯
 * @param pszFrom			SIP �޽��� ���� ���̵�
 * @param pszTo				SIP �޽��� ���� ���̵�
 * @param pclsMessage SIP �޽���
 * @returns ��û�� �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CSipClientMFCDlg::EventMessage( const char * pszFrom, const char * pszTo, CSipMessage * pclsMessage )
{
	return false;
}

void CSipClientMFCDlg::OnDestroy()
{
	m_clsSipUserAgentMFC.SetWindowHandle( 0 );
	m_clsAudioThread.Stop();
	m_clsSipUserAgent.Stop();

	__super::OnDestroy();
}