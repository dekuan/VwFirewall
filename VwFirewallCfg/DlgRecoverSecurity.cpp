// DlgRecoverSecurity.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgRecoverSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverSecurity dialog


CDlgRecoverSecurity::CDlgRecoverSecurity(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRecoverSecurity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRecoverSecurity)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CDlgRecoverSecurity::~CDlgRecoverSecurity()
{
	m_cThSleepRecoverSecurity.EndSleep();
	m_cThSleepRecoverSecurity.EndThread();
}


void CDlgRecoverSecurity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRecoverSecurity)
	DDX_Control(pDX, IDC_STATIC_HINT, m_stcHint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRecoverSecurity, CDialog)
	//{{AFX_MSG_MAP(CDlgRecoverSecurity)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverSecurity message handlers

BOOL CDlgRecoverSecurity::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	MakeAlphaDlg( m_hWnd, 90 );
	
	//
	//	开启修复线程
	//
	m_cThSleepRecoverSecurity.m_hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			&_threadRecoverSecurity,
			(void*)this,
			0,
			&m_cThSleepRecoverSecurity.m_uThreadId
		);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





unsigned __stdcall CDlgRecoverSecurity::_threadRecoverSecurity( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CDlgRecoverSecurity * pThis = (CDlgRecoverSecurity*)arglist;
		if ( pThis )
		{
			pThis->RecoverSecurityProc();
		}

		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CDlgRecoverSecurity::RecoverSecurityProc()
{
	TCHAR szCmdLine[ 1024 ];
	TCHAR szHint[ MAX_PATH ];

	//	还原中
	_sntprintf( szHint, sizeof(szHint)/sizeof(TCHAR)-1, _T("正在还原C盘NTFS权限为默认，请稍候...") );
	m_stcHint.SetWindowText( szHint );

	//	...
	_sntprintf
	(
		szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1,
		_T("%s /configure /db \"%ssecurity\\database\\cvtfs.sdb\" /Cfg \"%ssecurity\\templates\\setup security.inf\" /areas filestore"),
		m_cVwFirewallConfigFile.mb_szSysSecEditFile,
		m_cVwFirewallConfigFile.mb_szSysDir,
		m_cVwFirewallConfigFile.mb_szSysDir
	);
	delib_run_block_process_ex( szCmdLine, FALSE );

	//	...
	SetTimer( 1000, 3000, NULL );
}

void CDlgRecoverSecurity::OnTimer( UINT nIDEvent ) 
{
	if ( 1000 == nIDEvent )
	{
		KillTimer( 1000 );
		
		//	...
		MessageBox( "恭喜，已经完成还原。新的配置会在重启计算机后生效。", "还原完成", MB_ICONINFORMATION );

		OnOK();
	}
	CDialog::OnTimer(nIDEvent);
}



/**
*	@ public
*	设置窗口透明度
*/
BOOL CDlgRecoverSecurity::MakeAlphaDlg( HWND hWnd, INT nAlphaPercent )
{
	if ( NULL == hWnd )
	{
		return FALSE;
	}
	
	typedef BOOL (FAR PASCAL * FUNC1)
		(
			HWND hwnd,           // handle to the layered window
			COLORREF crKey,      // specifies the color key
			BYTE bAlpha,         // value for the blend function
			DWORD dwFlags        // action
		);
	
	//	第二步 实现代码
	HMODULE hModule = GetModuleHandle( _T("user32.dll") );
	
	if ( hModule )
	{
		FUNC1 SetLayeredWindowAttributes = (FUNC1)GetProcAddress( hModule, ("SetLayeredWindowAttributes") );
		
		if ( SetLayeredWindowAttributes )
		{
			// 设置分层扩展标记
			SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | 0x80000L );
			// 70% alpha
			SetLayeredWindowAttributes( hWnd, 0, ( 255 * nAlphaPercent ) / 100, 0x2 );
			
			// 第三步：如何除去透明选项？
			// 除去分层扩展标记
			//	SetWindowLong(hWnd,, GWL_EXSTYLE,
			//	GetWindowLong(hWnd, GWL_EXSTYLE) & ~ 0x80000L);
			// 重画窗口
			//	RedrawWindow();
		}
	}
	
	return TRUE;
}
