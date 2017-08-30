// DlgSetRDPPort.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgSetRDPPort.h"
#include "ProcWinFirewall.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetRDPPort dialog


CDlgSetRDPPort::CDlgSetRDPPort(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetRDPPort::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetRDPPort)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSetRDPPort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetRDPPort)
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_CHECK_ADDTO_FW_EXCEPTION, m_chkAddToFwException);
	DDX_Control(pDX, IDC_EDIT_NEW_PORT, m_editNewPort);
	DDX_Control(pDX, IDC_EDIT_ORG_PORT, m_editOrgPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetRDPPort, CDialog)
	//{{AFX_MSG_MAP(CDlgSetRDPPort)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_EN_CHANGE(IDC_EDIT_NEW_PORT, OnChangeEditNewPort)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetRDPPort message handlers

BOOL CDlgSetRDPPort::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	//	装载配置
	//
	LoadConfig();


	//	1 毫秒后设置 新端口 输入框焦点
	SetTimer( 1000, 1, NULL );


	
	return TRUE;
}

void CDlgSetRDPPort::OnButtonSave() 
{
	if ( SaveConfig() )
	{
		m_strCaption.Format( IDS_MSG_CAPTION_CONGLT );
		m_strString.Format( IDS_DLGSETRDPPORT_MODIFY_SUCC );
		MessageBox( m_strString, m_strCaption, MB_ICONINFORMATION );
		OnOK();
	}
}

void CDlgSetRDPPort::OnChangeEditNewPort() 
{
	TCHAR szOrgPort[ MAX_PATH ];
	TCHAR szNewPort[ MAX_PATH ];

	memset( szOrgPort, 0, sizeof(szOrgPort) );
	memset( szNewPort, 0, sizeof(szNewPort) );

	m_editOrgPort.GetWindowText( szOrgPort, sizeof(szOrgPort) );
	m_editNewPort.GetWindowText( szNewPort, sizeof(szNewPort) );

	StrTrim( szNewPort, "\r\n\t " );

	if ( CProcFunc::IsValidPortNumber( szNewPort ) && 0 != _tcsicmp( szOrgPort, szNewPort ) )
	{
		m_btnSave.EnableWindow( TRUE );
	}
	else
	{
		m_btnSave.EnableWindow( FALSE );
	}
}


void CDlgSetRDPPort::OnTimer( UINT nIDEvent )
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		m_editNewPort.SetFocus();
	}

	CDialog::OnTimer(nIDEvent);
}






BOOL CDlgSetRDPPort::LoadConfig()
{
	BOOL  bRet		= FALSE;
	DWORD dwPortNumber	= 0;
	DWORD dwType;
	DWORD dwSize;
	CString strTemp;

	dwType	= REG_DWORD;
	dwSize	= sizeof(dwPortNumber);
	if ( ERROR_SUCCESS == 
		SHGetValue( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp"), _T("PortNumber"), &dwType, &dwPortNumber, &dwSize ) )
	{
		strTemp.Format( "%d", dwPortNumber );
		m_editOrgPort.SetWindowText( strTemp );
	}	

	//	...
	m_chkAddToFwException.SetCheck( TRUE );


	return bRet;
}

BOOL CDlgSetRDPPort::SaveConfig()
{
	BOOL bRet	= FALSE;
	TCHAR szNewPort[ MAX_PATH ];
	DWORD dwNewPort;
	BOOL bAddToFwException;
	CProcWinFirewall cProcFw;
	HRESULT hr;
	BOOL  bFwIsOn;
	DWORD dwSetValue;

	//	...
	bRet = FALSE;

	memset( szNewPort, 0, sizeof(szNewPort) );
	m_editNewPort.GetWindowText( szNewPort, sizeof(szNewPort) );
	StrTrim( szNewPort, "\r\n\t " );
	dwNewPort = (DWORD)StrToInt( szNewPort );

	if ( CProcFunc::IsValidPortNumber( szNewPort ) &&
		dwNewPort > 1024 && dwNewPort < 65535 )
	{
		bAddToFwException = FALSE;

		//	添加端口到 Windows 防火墙的例外
		if ( m_chkAddToFwException.GetCheck() )
		{
			//	确认防火墙服务存在
			if ( cProcFw.IsFwSrvExist() &&
				SUCCEEDED( cProcFw.IsOn( &bFwIsOn ) ) )
			{
				hr = cProcFw.PortAdd( dwNewPort, NET_FW_IP_PROTOCOL_TCP, L"Remote Desktop(VwFirewall)" );
				if ( SUCCEEDED( hr ) )
				{
					bAddToFwException = TRUE;
				}
				else
				{
					//	将端口(%d)添加到 Windows 防火墙时失败，请您手动添加后再试。
					m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
					m_strString.Format( IDS_DLGSETRDPPORT_ERR_ADDPORT_EXP1, dwNewPort );
					MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
				}
			}
			else
			{
				//	防火墙不存，无需添加，所以算成功
				bAddToFwException = TRUE;
			}	
		}
		else
		{
			//	用户未选择，算成功
			bAddToFwException = TRUE;
		}

		if ( bAddToFwException )
		{
			//
			//	保存到注册表
			//
			dwSetValue = SHSetValue
				(
					HKEY_LOCAL_MACHINE,
					_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp"),
					_T("PortNumber"),
					REG_DWORD,
					(LPCVOID)(&dwNewPort),
					sizeof(dwNewPort)
				);
			if ( ERROR_SUCCESS == dwSetValue )
			{
				dwSetValue = SHSetValue
					(
						HKEY_LOCAL_MACHINE,
						_T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp"),
						_T("PortNumber"),
						REG_DWORD,
						(LPCVOID)(&dwNewPort),
						sizeof(dwNewPort)
					);
				if ( ERROR_SUCCESS == dwSetValue )
				{
					bRet = TRUE;
				}
			}

			if ( ! bRet )
			{
				//	保存端口(%d)时失败，请确认注册表允许修改，或者关闭相关病毒保护软件后再试。
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( IDS_DLGSETRDPPORT_ERR_ADDPORT_EXP2, dwNewPort );
				MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
			}	
		}
	}
	else
	{
		//	请输入正确的端口号，必须是 1024 至 65535 之间的数字。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_DLGSETRDPPORT_ERR_ADDPORT_EXP3 );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}	


	return bRet;
}


