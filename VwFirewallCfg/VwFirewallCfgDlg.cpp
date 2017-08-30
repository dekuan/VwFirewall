// VwFirewallCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "VwFirewallCfgDlg.h"
#include "process.h"
#include "DeMemDC.h"

#include "ProcFunc.h"
#include "ProcVwAd.h"


#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgDlg dialog

CVwFirewallCfgDlg::CVwFirewallCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVwFirewallCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVwFirewallCfgDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	//
	//	参数初始化
	//
	m_dwDlgAutoHideCount	= 0;
	m_hTreeSetting		= NULL;

	m_dwSelButton	= IDC_BUTTON_STATUS;
	m_clrBtnText	= RGB( 0xFF, 0xFF, 0xFF );
	m_clrGray	= RGB( 0x3D, 0x3D, 0x3D );
	m_clrBack	= RGB( 0x00, 0x00, 0x00 );
	m_clrHover	= RGB( 0x63, 0x63, 0x63 );
	m_clrDarkRed	= RGB( 0x75, 0x00, 0x00 );	//0x9b0000

	//
	//	按钮参数初始化
	//
	m_dwBottomAdLineHeight	= 28;
	m_dwButtonTopOffset	= 76;
	m_dwButtonWidth		= 90;
	m_dwButtonHeight	= 26;

	m_pArrButtons[ 0 ]	= &m_btnStatus;
	m_pArrButtons[ 1 ]	= &m_btnSecurityCfg;
	m_pArrButtons[ 2 ]	= &m_btnLicense;
	m_pArrButtons[ 3 ]	= &m_btnUpdate;

	m_dwArrButtonsID[ 0 ]	= IDC_BUTTON_STATUS;
	m_dwArrButtonsID[ 1 ]	= IDC_BUTTON_SECURITYCFG;
	m_dwArrButtonsID[ 2 ]	= IDC_BUTTON_LICENSE;
	m_dwArrButtonsID[ 3 ]	= IDC_BUTTON_UPDATE;
}

CVwFirewallCfgDlg::~CVwFirewallCfgDlg()
{
	if ( m_cThreadAd.m_hThread )
	{
		m_cThreadAd.EndSleep();
		m_cThreadAd.EndThread( &m_cThreadAd.m_hThread );
	}
}


void CVwFirewallCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVwFirewallCfgDlg)
	DDX_Control(pDX, IDC_BUTTON_SECURITYCFG, m_btnSecurityCfg);
	DDX_Control(pDX, IDC_STATIC_TITIMAGE, m_stcTitImage);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_btnUpdate);
	DDX_Control(pDX, IDC_BUTTON_LICENSE, m_btnLicense);
	DDX_Control(pDX, IDC_BUTTON_STATUS, m_btnStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVwFirewallCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CVwFirewallCfgDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_CLOSE,OnMyClose)
	ON_MESSAGE(UM_SHOWTASK,OnShowTask)
	ON_MESSAGE(UM_OPENHTMLHELP, OnOpenHtmlHelp)
	ON_COMMAND(IDM_MENU_SYSTRAY_EXIT, OnMenuSystrayExit)
	ON_COMMAND(IDM_MENU_SYSTRAY_DEHOMEPAGE, OnMenuSystrayDehomepage)
	ON_COMMAND(IDM_MENU_SYSTRAY_OPEN, OnMenuSystrayOpen)
	ON_COMMAND(IDM_MENU_SYSTRAY_OPENWORKINGDIR, OnMenuSystrayOpenworkingdir)
	ON_COMMAND(IDM_MENU_SYSTRAY_STARTAUTO, OnMenuSystrayStartauto)
	ON_WM_TIMER()
	ON_COMMAND(IDM_MENU_SYSTRAY_OPENHELPDOC, OnMenuSystrayOpenhelpdoc)
	ON_BN_CLICKED(IDC_BUTTON_STATUS, OnButtonStatus)
	ON_BN_CLICKED(IDC_BUTTON_LICENSE, OnButtonLicense)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SECURITYCFG, OnButtonSecuritycfg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgDlg message handlers

BOOL CVwFirewallCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here


	//	App name
	memset( m_szAppName, 0, sizeof(m_szAppName) );
	memset( m_szAppCaption, 0, sizeof(m_szAppCaption) );
	memset( m_szStartupInfo, 0, sizeof(m_szStartupInfo) );

	LoadString( AfxGetInstanceHandle(), IDS_APP_NAME, m_szAppName, sizeof(m_szAppName) );
	LoadString( AfxGetInstanceHandle(), IDS_APP_CAPTION, m_szAppCaption, sizeof(m_szAppCaption) );
	LoadString( AfxGetInstanceHandle(), IDS_APP_STARTUP, m_szStartupInfo, sizeof(m_szStartupInfo) );
	
	//	设置窗口标题
	::SetWindowText( m_hWnd, m_szAppCaption );


	//
	//	初始化所有按钮
	//
	InitAllButtons();


	//
	//	创建左侧树菜单
	//
	//CreateTreeMenu();



	//
	//	初始化 SysTray
	//
	m_xballoon.SetTrayiconClassName( "CVwFirewallCfgDlgTrayIcon" );
	m_xballoon.CreateTrayIcon
	(
		AfxGetInstanceHandle(),
		m_hWnd,
		UM_SHOWTASK,		// 这个消息是给这个 proc 处理
		m_szAppName,
		LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SYSTRAY_STOPPED) ),
		0,
		TRUE
	);
	m_xballoon.ShowTrayIcon();
	m_xballoon.ShowBalloon( m_szStartupInfo, m_szAppName, NIIF_INFO, 10*1000 );

	//
	//	初始化 caption
	//
	/*
	m_stcCap.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_stcCap.m_fontWeight  = FW_BOLD;
	m_stcCap.m_fontSize    = 14;
	m_stcCap.m_csFontName  = "verdana";
	m_stcCap.SetConstantText( m_csCapText );
	*/





	//
	//	如果是自动启动的话，那么自动隐藏窗口
	//
	SetTimer( CVWFIREWALLCFGDLG_TIMER_AUTOHIDDEN, 1, NULL );


	//
	//	创建广告线程
	//
	m_cThreadAd.m_hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			&_threadVwStaticAd,
			(void*)this,
			0,
			&m_cThreadAd.m_uThreadId
		);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVwFirewallCfgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVwFirewallCfgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		//CDialog::OnPaint();
		OnMyPaint();
	}
}

/**
 *	@ Private
 *	自画所有的区域
 */
VOID CVwFirewallCfgDlg::OnMyPaint()
{
	CPaintDC dc( this ); // device context for painting
	CRect rect;
	CRect rectButton;
	CRect rectTopRight;
	CRect rectBottomLine;

	GetClientRect( &rect );
	rectButton	= rect;
	rectTopRight	= rect;
	rectBottomLine	= rect;
	

	//	顶部按钮区域
	INT   i;

	for ( i = 0; i < CVWFIREWALLCFGDLG_BUTTON_COUNT; i ++ )
	{
		rectButton.top		= m_dwButtonTopOffset;
		rectButton.bottom	= m_dwButtonTopOffset + m_dwButtonHeight;
		rectButton.left		= i * m_dwButtonWidth + i;
		rectButton.right	= rectButton.left + m_dwButtonWidth;

		CDeMemDC memDcBtn( &dc, &rectButton );
		memDcBtn.FillSolidRect( &rectButton, m_clrGray );
	}

	//
	//	按钮右边的区域
	//
	rectTopRight.top	= m_dwButtonTopOffset;
	rectTopRight.bottom	= rectTopRight.top + m_dwButtonHeight;
	rectTopRight.left	= 0;	//	CVWFIREWALLCFGDLG_BUTTON_COUNT * m_dwButtonWidth + CVWFIREWALLCFGDLG_BUTTON_COUNT;
	CDeMemDC memDcTopRight( &dc, &rectTopRight );
	memDcTopRight.FillSolidRect( &rectTopRight, m_clrGray );


	//	底部黑背景条的区域
	rectBottomLine.top	= rectBottomLine.bottom - m_dwBottomAdLineHeight;
	
	//	#8A6D00
	CDeMemDC memDcBottom( &dc, &rectBottomLine );
	memDcBottom.FillSolidRect( &rectBottomLine, m_clrBack );
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVwFirewallCfgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CVwFirewallCfgDlg::PreTranslateMessage(MSG* pMsg) 
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


void CVwFirewallCfgDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//	处理任何位置的拖拽效果
	
	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION,MAKELPARAM( point.x, point.y ) );
	
	//	向系统发送 HTCAPTION 消息，让系统以为鼠标点在标题栏上
	CDialog::OnLButtonDown(nFlags, point);
}

void CVwFirewallCfgDlg::OnTimer(UINT nIDEvent) 
{
	if ( CVWFIREWALLCFGDLG_TIMER_AUTOHIDDEN == nIDEvent )
	{
		KillTimer( CVWFIREWALLCFGDLG_TIMER_AUTOHIDDEN );

		TCHAR szCmdParam[ MAX_PATH ]	= {0};
		_sntprintf( szCmdParam, sizeof(szCmdParam)-sizeof(TCHAR), _T("%s"), AfxGetApp()->m_lpCmdLine );
		if ( 0 != _tcsicmp( szCmdParam, _T("-auto") ) )
		{
			this->ShowWindow( SW_SHOW );	//简单的显示主窗口完事儿
			ShowWindow( SW_SHOWDEFAULT );

			//	Dirty hack to bring the window to the foreground
			::SetWindowPos( this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
			::SetWindowPos( this->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
			this->SetFocus();
			//::PostMessage( m_hWnd, WM_SHOWWINDOW, 0, 0 );
		}
	}

	CDialog::OnTimer(nIDEvent);
}

//DEL void CVwFirewallCfgDlg::OnSelchangedTreeMenu(NMHDR* pNMHDR, LRESULT* pResult) 
//DEL {
//DEL 	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
//DEL /*
//DEL 	HTREEITEM hTreeItem = m_treeMenu.GetSelectedItem();
//DEL 	TVITEM tvItem;
//DEL 	INT i;
//DEL 	TCHAR szText[1024] = {0};
//DEL 
//DEL 	
//DEL 	if ( hTreeItem )
//DEL 	{
//DEL 		tvItem.hItem = hTreeItem;
//DEL 		tvItem.pszText = szText;
//DEL 		tvItem.cchTextMax = sizeof(szText);
//DEL 		
//DEL 		if ( m_treeMenu.GetItem( &tvItem ) )
//DEL 		{
//DEL 			for ( i = 0; i < g_nMenuTreeCount; i ++ )
//DEL 			{
//DEL 				if ( tvItem.lParam == g_stMenuTree[ i ].lParam )
//DEL 				{
//DEL 					//m_stcCap.SetWindowText( g_stMenuTree[ i ].lpszCaption );
//DEL 					//ShowTreeTab( tvItem.lParam );
//DEL 
//DEL 					if ( g_stMenuTree[ i ].nDlgId > 0 )
//DEL 					{
//DEL 						ShowTabWindow( g_stMenuTree[ i ].nDlgId );
//DEL 					}
//DEL 
//DEL 					break;
//DEL 				}
//DEL 			}
//DEL 		}
//DEL 	}
//DEL */
//DEL 	*pResult = 0;
//DEL }

/**
 *	显示 Tab 界面
 */
VOID CVwFirewallCfgDlg::ShowTabWindow( UINT uIndex )
{
	TCHAR szCaption[ MAX_PATH ]	= {0};
	UINT  uMsgRet			= 0;

	m_cTabSecurityCfg.ShowWindow( IDD_TAB_SECURITYCFG == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabLicense.ShowWindow( IDD_TAB_LICENSE == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabUpdate.ShowWindow( IDD_TAB_UPDATE == uIndex ? SW_SHOW : SW_HIDE );
	m_cTabSetting.ShowWindow( IDD_TAB_SETTING == uIndex ? SW_SHOW : SW_HIDE );
}

void CVwFirewallCfgDlg::OnMyClose( WPARAM wParam, LPARAM lParam ) 
{
	if ( 200500 == wParam && 200500 == lParam )
	{
		//	这个消息是 卸载程序 或者 LiveUpdate 发来的
		CDialog::OnClose();
		DestroyWindow();
	}
	else
	{
		ShowWindow( SW_HIDE );
	}
	
	//CDialog::OnClose();
}

LRESULT CVwFirewallCfgDlg::OnOpenHtmlHelp( WPARAM wParam, LPARAM lParam )
{
	CProcFunc cProcFunc;
	cProcFunc.OpenChmHelpPage( wParam );
	return S_OK;
}


/**
 *	SysTray
 */
LRESULT CVwFirewallCfgDlg::OnShowTask( WPARAM wParam, LPARAM lParam )
{
	//
	//	wParam	- [in] 接收的是图标的ID
	//	lParam	- [in] 接收的是鼠标的行为
	//
	//	if ( wParam != IDR_MAINFRAME )
	//		return 1;

	switch ( lParam )
	{
	case WM_RBUTTONUP:	//右键起来时弹出快捷菜单，这里只有一个“关闭”
		{
			LPPOINT lpoint = new tagPOINT;
			if ( lpoint )
			{
				::GetCursorPos(lpoint);	//得到鼠标位置
				SetForegroundWindow();
				
				CMenu menu;
				VERIFY( menu.LoadMenu(IDR_MENU_SYSTRAY) );
				CMenu * pPopup = menu.GetSubMenu(0);
				
				if ( pPopup && pPopup->m_hMenu )
				{
					ASSERT( pPopup != NULL );
					CWnd * pWndPopupOwner = this;
					while (pWndPopupOwner->GetStyle() & WS_CHILD)
					{
						pWndPopupOwner = pWndPopupOwner->GetParent();
					}
					
					if ( IsStartAuto() )
					{
						pPopup->CheckMenuItem( IDM_MENU_SYSTRAY_STARTAUTO, MF_CHECKED );
					}
					else
					{
						pPopup->CheckMenuItem( IDM_MENU_SYSTRAY_STARTAUTO, MF_UNCHECKED );
					}

					//	设置“打开配置工具”项目的字体为粗体
					::SetMenuDefaultItem( pPopup->m_hMenu, IDM_MENU_SYSTRAY_OPEN, FALSE );

					//	..
					pPopup->TrackPopupMenu
					(
						TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
						lpoint->x,
						lpoint->y,
						pWndPopupOwner
					);
					delete lpoint;
					
					// Fix for Notification Icons Don't Work Correctly
					PostMessage( WM_NULL, 0, 0 );
				}
			}
		}
		break;
		
	case WM_LBUTTONDBLCLK:		//双击左键的处理
		{
			this->ShowWindow( SW_SHOW );	//简单的显示主窗口完事儿
			ShowWindow( SW_SHOWDEFAULT );

			//	Dirty hack to bring the window to the foreground
			::SetWindowPos( this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
			::SetWindowPos( this->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
			this->SetFocus();
		}
		break;
		
	case NIN_BALLOONSHOW:
		//	气球出现
		{
		}
		break;
		
	case NIN_BALLOONHIDE:
		//	气球被隐藏
		{
		}
		break;
		
	case NIN_BALLOONUSERCLICK:
		//	左键单击气球
		{
			//	this->ShowWindow( SW_SHOW );	//简单的显示主窗口完事儿
			//	ShowWindow( SW_SHOWDEFAULT );
		}
		break;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	Private



/**
 *	@ Private
 *	初始化所有按钮
 */
VOID CVwFirewallCfgDlg::InitAllButtons()
{
	CRect rect;
	CRect rectTitImage;
	CRect rectTitImageNew;
	CRect rectLink;
	TCHAR szDecString[ MAX_PATH ]	= {0};
	

	//
	//	初始化标题图片的位置
	//
	GetClientRect( &rect );
	::GetClientRect( m_stcTitImage.m_hWnd, &rectTitImage );
	rectTitImageNew.left	= 0;
	rectTitImageNew.top	= 0;
	rectTitImageNew.right	= rect.right;
	rectTitImageNew.bottom	= ( ( rectTitImage.Height() * rect.Width() ) / rectTitImage.Width() );
	::SetWindowPos( m_stcTitImage.m_hWnd, NULL, 0, 0, rectTitImageNew.Width(), rectTitImageNew.Height(), SWP_NOMOVE );

	m_dwButtonTopOffset	= rectTitImageNew.Height();
	if ( rectTitImageNew.Width() > 680 )
	{
		m_dwButtonWidth		= ( rect.Width() * m_dwButtonWidth ) / rectTitImage.Width();
		m_dwButtonHeight	= ( rect.Width() * m_dwButtonHeight ) / rectTitImage.Width();
		m_dwBottomAdLineHeight	= ( rect.Width() * m_dwBottomAdLineHeight ) / rectTitImage.Width();
	}

	//	计算 TAB 的空间
	m_rcTableRect.left	= 0;
	m_rcTableRect.top	= m_dwButtonTopOffset + m_dwButtonHeight + 3;
	m_rcTableRect.right	= rect.right;
	m_rcTableRect.bottom	= rect.bottom - m_dwBottomAdLineHeight - 3;

	//
	//	初始化 按钮位置和
	//
	OnNavButtonClick( m_dwSelButton, TRUE );


	//
	//	设置底部广告和连接
	//
	memcpy( szDecString, g_szUrlVwHomepage, min( sizeof(g_szUrlVwHomepage), sizeof(szDecString) ) );
	delib_xorenc( szDecString );
	m_LinkAd.SubclassDlgItem( IDC_STATIC_LINK_AD, this );
	m_LinkAd.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkAd.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkAd.SetUnderline( FALSE );
	m_LinkAd.SetVisited( FALSE );
	m_LinkAd.SetWindowText( "" );
	m_LinkAd.SetURL( szDecString );

	memcpy( szDecString, g_szUrlVwHomepage, min( sizeof(g_szUrlVwHomepage), sizeof(szDecString) ) );
	delib_xorenc( szDecString );
	m_LinkVw.SubclassDlgItem( IDC_STATIC_LINK_VW, this );
	m_LinkVw.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkVw.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkVw.SetUnderline( FALSE );
	m_LinkVw.SetVisited( FALSE );
	m_LinkVw.SetURL( szDecString );

	//	split line
	m_LinkSplit1.SubclassDlgItem( IDC_STATIC_SPLIT1, this );
	m_LinkSplit1.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkSplit1.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkSplit1.SetUnderline( FALSE );
	m_LinkSplit1.SetVisited( FALSE );
	m_LinkSplit1.SetStaticOnly( TRUE );
	m_LinkSplit1.SetURL( "" );
	m_LinkSplit1.SetHint( "" );
	m_LinkSplit1.SetLinkCursor( ::LoadCursor( NULL, MAKEINTRESOURCE( IDC_ARROW ) ) );

	m_LinkSplit2.SubclassDlgItem( IDC_STATIC_SPLIT2, this );
	m_LinkSplit2.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkSplit2.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkSplit2.SetUnderline( FALSE );
	m_LinkSplit2.SetVisited( FALSE );
	m_LinkSplit2.SetStaticOnly( TRUE );
	m_LinkSplit2.SetURL( "" );
	m_LinkSplit2.SetHint( "" );
	m_LinkSplit2.SetLinkCursor( ::LoadCursor( NULL, MAKEINTRESOURCE( IDC_ARROW ) ) );

	m_LinkDiscuss.SubclassDlgItem( IDC_STATIC_LINK_DISCUSS, this );
	m_LinkDiscuss.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkDiscuss.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkDiscuss.SetUnderline( FALSE );
	m_LinkDiscuss.SetVisited( FALSE );
	m_LinkDiscuss.SetURL( "http://rd.vidun.net/?4503" );

	m_LinkHelpDoc.SubclassDlgItem( IDC_STATIC_LINK_HELPDOC, this );
	m_LinkHelpDoc.SetColours( RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) );
	m_LinkHelpDoc.SetBackColor( RGB(0xFF,0xFF,0xFF) );
	m_LinkHelpDoc.SetUnderline( FALSE );
	m_LinkHelpDoc.SetVisited( FALSE );
	m_LinkHelpDoc.SetCallbackMessage( m_hWnd, UM_OPENHTMLHELP, WPARAM_HEMLHELP_VWFIREWALL_OVERVIEW, 0 );



	//
	//	初始化 Tab
	//
	m_cTabSecurityCfg.Create( IDD_TAB_SECURITYCFG, this );
	m_cTabSecurityCfg.MoveWindow( &m_rcTableRect );
	m_cTabSecurityCfg.m_pxballoon	= & m_xballoon;

	m_cTabLicense.Create( IDD_TAB_LICENSE, this );
	m_cTabLicense.MoveWindow( &m_rcTableRect );
	m_cTabLicense.m_pxballoon = & m_xballoon;

	m_cTabUpdate.Create( IDD_TAB_UPDATE, this );
	m_cTabUpdate.MoveWindow( &m_rcTableRect );
	m_cTabUpdate.m_pxballoon = & m_xballoon;

	m_cTabSetting.Create( IDD_TAB_SETTING, this );
	m_cTabSetting.MoveWindow( &m_rcTableRect );
	m_cTabSetting.m_pxballoon	= & m_xballoon;
	m_cTabSetting.m_hParentWind	= m_hWnd;

	ShowTabWindow( IDD_TAB_SETTING );
}

VOID CVwFirewallCfgDlg::OnNavButtonClick( DWORD dwButton, BOOL bInitPos /* = FALSE */ )
{
	//COLORREF clrBg;
	UINT  i;
	CRect rectButton;

	for ( i = 0; i < CVWFIREWALLCFGDLG_BUTTON_COUNT; i ++ )
	{
		if ( dwButton == m_dwArrButtonsID[i] )
		{
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_IN, m_clrDarkRed );
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_OUT, m_clrDarkRed );
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_FOCUS, m_clrDarkRed );
		}
		else
		{
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_IN, m_clrHover );
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_OUT, m_clrGray );
			m_pArrButtons[ i ]->SetColor( CButtonST::BTNST_COLOR_BK_FOCUS, m_clrGray );
		}

		//
		//	初始化定位按钮的位置
		//
		if ( bInitPos )
		{
			//	设置按钮样式
			m_pArrButtons[i]->SetColor( CButtonST::BTNST_COLOR_FG_IN, m_clrBtnText );		//	文字颜色
			m_pArrButtons[i]->SetColor( CButtonST::BTNST_COLOR_FG_OUT, m_clrBtnText );	//	文字颜色
			m_pArrButtons[i]->SetColor( CButtonST::BTNST_COLOR_FG_FOCUS, m_clrBtnText );	//	文字颜色
			m_pArrButtons[i]->SetBtnCursor( IDC_CURSOR_HAND );
			m_pArrButtons[i]->DrawBorder( FALSE );
			m_pArrButtons[i]->SetAlwaysTrack( FALSE );
			//m_pArrButtons[i]->SetBitmaps( IDB_BITMAP_BTN_FILELOCK_, RGB(0xFF,0x00,0xFF) );
			//m_pArrButtons[i]->SetWindowText( "" );

			rectButton.top		= m_dwButtonTopOffset;
			rectButton.bottom	= m_dwButtonTopOffset + m_dwButtonHeight;
			rectButton.left		= i * m_dwButtonWidth + i;
			rectButton.right	= rectButton.left + m_dwButtonWidth;

			::SetWindowPos( m_pArrButtons[i]->m_hWnd, NULL, rectButton.left, rectButton.top, m_dwButtonWidth, m_dwButtonHeight, 0 );
		}
	}
}



/**
 *	从注册表中读取，是否自动重启
 */
BOOL CVwFirewallCfgDlg::IsStartAuto( BOOL bCheckExist /*FALSE*/ )
{
	BOOL  bRet			= FALSE;
	TCHAR szKeyValue[ MAX_PATH ]	= {0};
	DWORD dwSize			= sizeof(szKeyValue);
	DWORD dwType			= REG_SZ;
	DWORD dwRet			= 0;
	
	dwRet = SHGetValue
	(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		"VwFirewallCfgAuto",
		&dwType, szKeyValue, &dwSize
	);
	if ( ERROR_SUCCESS == dwRet )
	{
		if ( bCheckExist )
		{
			//	如果是为了检查该键是否存在
			//	只要读取成功就返回真
			bRet = TRUE;
		}
		else
		{
			if ( dwSize > 0 && NULL == _tcsstr(_tcslwr(szKeyValue),_T("rem ")) )
			{
				bRet = TRUE;
			}
		}
	}
	else
	{
		//	键值不存在
	}
	
	return bRet;
}

/**
 *	设置自动启动
 */
VOID CVwFirewallCfgDlg::SetStartAuto( BOOL bStartAuto )
{
	//
	//	bSetStartAuto	- [in] 用户设置要求，是否自动启动
	//

	TCHAR szKeyValue[ MAX_PATH ]	= {0};
	DWORD dwSize			= sizeof(szKeyValue);

	if ( TRUE == bStartAuto )
	{
		_sntprintf( szKeyValue, sizeof(szKeyValue)-sizeof(TCHAR), _T("%s"), m_cModuleInfo.mb_szModFileAuto );
	}
	else
	{
		_sntprintf( szKeyValue, sizeof(szKeyValue)-sizeof(TCHAR), _T("rem %s"), m_cModuleInfo.mb_szModFileAuto );
	}

	SHSetValue( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "VwFirewallCfgAuto", REG_SZ, szKeyValue, _tcslen(szKeyValue) );
}
void CVwFirewallCfgDlg::OnMenuSystrayExit() 
{
	//CDialog::OnClose();
	CDialog::OnCancel();
	DestroyWindow();
}

void CVwFirewallCfgDlg::OnMenuSystrayDehomepage() 
{
	CHyperLink clink;
	TCHAR szUrlHomepage[ MAX_PATH ]	= {0};

	memcpy( szUrlHomepage, g_szUrlVwHomepage, min( sizeof(g_szUrlVwHomepage), sizeof(szUrlHomepage) ) );
	delib_xorenc( szUrlHomepage );
	clink.LauchIEBrowser( szUrlHomepage );
}

void CVwFirewallCfgDlg::OnMenuSystrayOpen() 
{
	ShowWindow( SW_SHOW );
}

void CVwFirewallCfgDlg::OnMenuSystrayOpenworkingdir() 
{
	//	打开工作目录
	TCHAR szCmdLine[ MAX_PATH ]	= {0};
	_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), "explorer %s", m_cModuleInfo.mb_szModPath );
	WinExec( szCmdLine, SW_SHOW );	
}

void CVwFirewallCfgDlg::OnMenuSystrayOpenhelpdoc() 
{
	OnOpenHtmlHelp( WPARAM_HEMLHELP_VWFIREWALL_OVERVIEW, 0 );	
}

void CVwFirewallCfgDlg::OnMenuSystrayStartauto() 
{
	if ( IsStartAuto() )
	{
		SetStartAuto( FALSE );
	}
	else
	{
		SetStartAuto( TRUE );
	}
}

/**
 *	广告 StaticAd
 */
unsigned __stdcall CVwFirewallCfgDlg::_threadVwStaticAd( PVOID arglist )
{
	//
	//	arglist		- [in]
	//
	
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CVwFirewallCfgDlg * pThis = (CVwFirewallCfgDlg*)arglist;
		if ( pThis )
		{
			pThis->VwStaticAdProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CVwFirewallCfgDlg::VwStaticAdProc()
{
	CProcVwAd cProcVwAd;
	vector<STPROCVWADITEM> vcAdList;
	vector<STPROCVWADITEM>::iterator it;
	STPROCVWADITEM * pstItem;
	STPROCVWADINFO stAdInfo;

	//
	//	Loop
	//
	while( m_cThreadAd.IsContinue() )
	{
		memset( &stAdInfo, 0, sizeof(stAdInfo) );
		if ( cProcVwAd.QueryAdInfo( _T("VwFirewall"), _T("chs"), 4510, &vcAdList, &stAdInfo ) )
		{
			//m_cTabSetting.OutputDebugStringFormat( "CVwFirewallCfgDlg::VwStaticAdProc dwLoopSpan=%d, dwSingleSpan=%d", 
			//	stAdInfo.dwLoopSpan, stAdInfo.dwSingleSpan );

			for ( it = vcAdList.begin(); it != vcAdList.end(); it ++ )
			{
				pstItem = it;

				//	set info
				m_LinkAd.SetWindowText( pstItem->szTxt );
				m_LinkAd.SetURL( pstItem->szUrl );
				Invalidate();

				//	等待 30 秒
				Sleep( stAdInfo.dwSingleSpan );	
			}
		}

		//	等待 1 分钟
		Sleep( stAdInfo.dwLoopSpan );
	}
}







void CVwFirewallCfgDlg::OnButtonStatus() 
{
	m_dwSelButton	= IDC_BUTTON_STATUS;
	OnNavButtonClick( m_dwSelButton );
	ShowTabWindow( IDD_TAB_SETTING );
}

void CVwFirewallCfgDlg::OnButtonSecuritycfg() 
{
	m_dwSelButton	= IDC_BUTTON_SECURITYCFG;
	OnNavButtonClick( m_dwSelButton );
	ShowTabWindow( IDD_TAB_SECURITYCFG );
}


//DEL void CVwFirewallCfgDlg::OnButtonFile() 
//DEL {
//DEL 	m_dwSelButton	= IDC_BUTTON_FILE;
//DEL 	OnNavButtonClick( m_dwSelButton );
//DEL 	ShowTabWindow( IDD_TAB_FILE );
//DEL }

//DEL void CVwFirewallCfgDlg::OnButtonRdp() 
//DEL {
//DEL 	m_dwSelButton	= IDC_BUTTON_RDP;
//DEL 	OnNavButtonClick( m_dwSelButton );
//DEL 	ShowTabWindow( IDD_TAB_RDP );
//DEL }

//DEL void CVwFirewallCfgDlg::OnButtonDomain() 
//DEL {
//DEL 	m_dwSelButton	= IDC_BUTTON_DOMAIN;
//DEL 	OnNavButtonClick( m_dwSelButton );
//DEL 	ShowTabWindow( IDD_TAB_DOMAIN );
//DEL }

void CVwFirewallCfgDlg::OnButtonLicense() 
{
	m_dwSelButton	= IDC_BUTTON_LICENSE;
	OnNavButtonClick( m_dwSelButton );
	ShowTabWindow( IDD_TAB_LICENSE );
}

void CVwFirewallCfgDlg::OnButtonUpdate() 
{
	m_dwSelButton	= IDC_BUTTON_UPDATE;
	OnNavButtonClick( m_dwSelButton );
	ShowTabWindow( IDD_TAB_UPDATE );
}

void CVwFirewallCfgDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	return;
	
	if ( GetStyle() & WS_VISIBLE )
	{
		CDialog::OnShowWindow( bShow, nStatus );
	}
	else
	{
			long Style = ::GetWindowLong( *this, GWL_STYLE );
			::SetWindowLong( *this, GWL_STYLE, Style | WS_VISIBLE );
			CDialog::OnShowWindow( SW_HIDE, nStatus );
/*
		if ( m_dwDlgAutoHideCount < 1 )
		{
			long Style = ::GetWindowLong( *this, GWL_STYLE );
			::SetWindowLong( *this, GWL_STYLE, Style | WS_VISIBLE );
			CDialog::OnShowWindow( SW_HIDE, nStatus );

			m_dwDlgAutoHideCount ++;
		}
		else
		{
			CDialog::OnShowWindow( bShow, nStatus );
		}*/
	}
}
