// TabSecurityCfg.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfg dialog


CTabSecurityCfg::CTabSecurityCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSecurityCfg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSecurityCfg)
	//}}AFX_DATA_INIT

	m_uShowTabIndex		= 0;

	m_hIconInfo_16x16	= NULL;
	m_hIconAlert_16x16	= NULL;
	m_hIconOk_16x16		= NULL;
	m_hIconX_16x16		= NULL;
}


void CTabSecurityCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSecurityCfg)
	DDX_Control(pDX, IDC_STATIC_INFO, m_stcInfo);
	DDX_Control(pDX, IDC_STATIC_INFOICON, m_stcInfoIcon);
	DDX_Control(pDX, IDC_RADIO_ACLS_FILE, m_roAclsFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSecurityCfg, CDialog)
	//{{AFX_MSG_MAP(CTabSecurityCfg)
	ON_BN_CLICKED(IDC_RADIO_ACLS_FILE, OnRadioAclsFile)
	ON_BN_CLICKED(IDC_RADIO_ACLS_FOLDER, OnRadioAclsFolder)
	ON_BN_CLICKED(IDC_RADIO_ACLS_ANTIVIRUS, OnRadioAclsAntivirus)
	ON_BN_CLICKED(IDC_RADIO_SERVICE, OnRadioService)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_RADIO_OBJECT, OnRadioObject)
	ON_MESSAGE(UM_DATACHANGE, OnDataChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfg message handlers

BOOL CTabSecurityCfg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcTableRect;	

	GetClientRect( &rcTableRect );
	rcTableRect.top  += 9;
	rcTableRect.left += 142;

	m_cTabSecurityCfgACLSFile.Create( IDD_TAB_SECURITYCFG_ACLS_FILE, this );
	m_cTabSecurityCfgACLSFile.MoveWindow( &rcTableRect );

	m_cTabSecurityCfgACLSFolder.Create( IDD_TAB_SECURITYCFG_ACLS_FOLDER, this );
	m_cTabSecurityCfgACLSFolder.MoveWindow( &rcTableRect );

	m_cTabSecurityCfgACLSAntiVirus.Create( IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS, this );
	m_cTabSecurityCfgACLSAntiVirus.MoveWindow( &rcTableRect );

	m_cTabSecurityCfgService.Create( IDD_TAB_SECURITYCFG_SERVICE, this );
	m_cTabSecurityCfgService.MoveWindow( &rcTableRect );

	m_cTabSecurityCfgObject.Create( IDD_TAB_SECURITYCFG_OBJECT, this );
	m_cTabSecurityCfgObject.MoveWindow( &rcTableRect );

	m_roAclsFile.SetCheck( TRUE );
	OnRadioAclsFile();


	//
	//	Load Icons
	//
	m_hIconInfo_16x16	= (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
	m_hIconAlert_16x16	= (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_ALERT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
	m_hIconOk_16x16		= (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_OK), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
	m_hIconX_16x16		= (HICON)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_X), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );

	m_stcInfoIcon.SetIcon( NULL );

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTabSecurityCfg::PreTranslateMessage(MSG* pMsg) 
{
	//	拦截用户按下的 ESC 键，强迫用户点 X 退出
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CTabSecurityCfg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//
	//	处理任何位置的拖拽效果
	//
	HWND hParent = ::GetParent( m_hWnd );
	if ( hParent )
	{
		::PostMessage( hParent, WM_NCLBUTTONDOWN, HTCAPTION,MAKELPARAM( point.x, point.y ) );
	}
	
	//	向系统发送 HTCAPTION 消息，让系统以为鼠标点在标题栏上
	CDialog::OnLButtonDown(nFlags, point);
}

VOID CTabSecurityCfg::ShowTabWindow( UINT uIndex )
{
	m_uShowTabIndex = uIndex;

	m_cTabSecurityCfgACLSFile.ShowWindow( IDD_TAB_SECURITYCFG_ACLS_FILE == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabSecurityCfgACLSFolder.ShowWindow( IDD_TAB_SECURITYCFG_ACLS_FOLDER == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabSecurityCfgACLSAntiVirus.ShowWindow( IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabSecurityCfgService.ShowWindow( IDD_TAB_SECURITYCFG_SERVICE == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabSecurityCfgObject.ShowWindow( IDD_TAB_SECURITYCFG_OBJECT == uIndex ? SW_SHOW : SW_HIDE );
}


void CTabSecurityCfg::OnRadioAclsFile() 
{
	ShowTabWindow( IDD_TAB_SECURITYCFG_ACLS_FILE );
	OnDataChange( WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
	m_cTabSecurityCfgACLSFile.InitList();	
}

void CTabSecurityCfg::OnRadioAclsFolder() 
{
	ShowTabWindow( IDD_TAB_SECURITYCFG_ACLS_FOLDER );
	OnDataChange( WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
	m_cTabSecurityCfgACLSFolder.InitList();
}

void CTabSecurityCfg::OnRadioAclsAntivirus() 
{
	ShowTabWindow( IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS );
	OnDataChange( WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
	m_cTabSecurityCfgACLSAntiVirus.InitList();
}

void CTabSecurityCfg::OnRadioService() 
{
	ShowTabWindow( IDD_TAB_SECURITYCFG_SERVICE );
	OnDataChange( WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
	m_cTabSecurityCfgService.InitList();
}


void CTabSecurityCfg::OnRadioObject() 
{
	ShowTabWindow( IDD_TAB_SECURITYCFG_OBJECT );
	OnDataChange( WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
	m_cTabSecurityCfgObject.InitList();
}


/**
 *	@ Public
 *	修改 ICON 和提示信息
 */
LRESULT CTabSecurityCfg::OnDataChange( WPARAM wParam, LPARAM lParam )
{
	//
	//	wParam	- [in] sub type
	//	lParam	- [in] icon
	//	SendMessage( hWnd, UM_DATACHANGE, (WPARAM)WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, (LPARAM)CTABSECURITYCFG_ICONTYPE_INFO );
	//
	//
	DWORD dwIconType;

	switch ( wParam )
	{
	case WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE:
		{
			dwIconType = 0;

			//
			//	set info
			//
			switch ( m_uShowTabIndex )
			{
			case IDD_TAB_SECURITYCFG_ACLS_FILE:
				{
					dwIconType = m_cTabSecurityCfgACLSFile.m_dwInfoType;
					m_stcInfo.SetWindowText( m_cTabSecurityCfgACLSFile.m_szInfo );
				}
				break;
			case IDD_TAB_SECURITYCFG_ACLS_FOLDER:
				{
					dwIconType = m_cTabSecurityCfgACLSFolder.m_dwInfoType;
					m_stcInfo.SetWindowText( m_cTabSecurityCfgACLSFolder.m_szInfo );
				}
				break;
			case IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS:
				{
					dwIconType = m_cTabSecurityCfgACLSAntiVirus.m_dwInfoType;
					m_stcInfo.SetWindowText( m_cTabSecurityCfgACLSAntiVirus.m_szInfo );
				}
				break;
			case IDD_TAB_SECURITYCFG_SERVICE:
				{
					dwIconType = m_cTabSecurityCfgService.m_dwInfoType;
					m_stcInfo.SetWindowText( m_cTabSecurityCfgService.m_szInfo );
				}
				break;
			case IDD_TAB_SECURITYCFG_OBJECT:
				{
					dwIconType = m_cTabSecurityCfgObject.m_dwInfoType;
					m_stcInfo.SetWindowText( m_cTabSecurityCfgObject.m_szInfo );
				}
				break;
			}

			//
			//	info icon
			//
			switch ( dwIconType )
			{
			case CTABSECURITYCFG_ICONTYPE_INFO:
				{
					m_stcInfoIcon.SetIcon( m_hIconInfo_16x16 );
				}
				break;
			case CTABSECURITYCFG_ICONTYPE_ALERT:
				{
					m_stcInfoIcon.SetIcon( m_hIconAlert_16x16 );
				}
				break;
			case CTABSECURITYCFG_ICONTYPE_OK:
				{
					m_stcInfoIcon.SetIcon( m_hIconOk_16x16 );
				}
				break;
			case CTABSECURITYCFG_ICONTYPE_X:
				{
					m_stcInfoIcon.SetIcon( m_hIconX_16x16 );
				}
				break;
			default:
				{
					m_stcInfoIcon.SetIcon( NULL );
				}
				break;
			}
		}
		break;
	}
	
	return S_OK;
}