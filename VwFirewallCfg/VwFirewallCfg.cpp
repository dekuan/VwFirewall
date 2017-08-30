// VwFirewallCfg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
//#include "VwFirewallCfgDlg.h"
#include "TabBaseInfo.h"

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )

#include "DeHttp.h"

#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgApp

BEGIN_MESSAGE_MAP(CVwFirewallCfgApp, CWinApp)
	//{{AFX_MSG_MAP(CVwFirewallCfgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgApp construction

CVwFirewallCfgApp::CVwFirewallCfgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVwFirewallCfgApp object

CVwFirewallCfgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgApp initialization

BOOL CVwFirewallCfgApp::InitInstance()
{
	//
	//	执行特殊命令
	//
	TCHAR szCmdLine[ MAX_PATH ]	= {0};
	BOOL  bBgRun;
	BOOL  bOpIIS;
	CModuleInfo modinfo;
	TCHAR * pLocDrvFile;

	_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), _T("%s"), GetCommandLine() );
	bBgRun	= ( _tcsstr( szCmdLine, "-bgrun" ) ? TRUE : FALSE );
	bOpIIS	= ( _tcsstr( szCmdLine, "-opiis" ) ? TRUE : FALSE );

	if ( _tcsstr( szCmdLine, "-install" ) )
	{
		//
		//	将程序安装目录保存到配置文件
		//
		CVwFirewallConfigFile::SaveConfig
			(
				CVWFIREWALLCONFIGFILE_INI_DOMAIN_APPINFO,
				CVWFIREWALLCONFIGFILE_INI_KEY_APP_INSDIR,
				CVwFirewallConfigFile::mb_szModPath
			);
		CVwFirewallConfigFile::SaveConfig
			(
				CVWFIREWALLCONFIGFILE_INI_DOMAIN_APPINFO,
				CVWFIREWALLCONFIGFILE_INI_KEY_APP_LOGDIR,
				CVwFirewallConfigFile::m_szLogsDir
			);

		//	停止执行，返回 FALSE
		return FALSE;
	}
	else if ( _tcsstr( szCmdLine, "-uninstall" ) )
	{
		//	关闭对话框和系统托盘图标
		SendWinodwCloseMessage();

		//	停止执行，返回 FALSE
		return FALSE;
	}
	else if ( _tcsstr( szCmdLine, "-drv_install" ) )
	{
		//
		//	安装 驱动
		//
		if ( delib_is_wow64() )
		{
			pLocDrvFile = CVwFirewallConfigFile::m_szLocDriverFile_wnet_amd64;
		}
		else
		{
			pLocDrvFile = CVwFirewallConfigFile::m_szLocDriverFile_wnet_x86;
		}
	
		delib_drv_install
		(
			DEVIE_NAMEA,
			pLocDrvFile,
			DRIVER_DESCA,
			SERVICE_SYSTEM_START
		);

		//
		//	发送安装请求
		//
		SendInstallRequest( TRUE );

		return FALSE;
	}
	else if ( _tcsstr( szCmdLine, "-drv_uninstall" ) )
	{
		//
		//	卸载 驱动
		//
		delib_drv_uninstall( DEVIE_NAMEA );

		//
		//	发送卸载请求
		//
		SendInstallRequest( FALSE );

		return FALSE;
	}


	//
	//	互斥量控制一个进程运行
	//
	if ( IsCfgRunning() )
	{
		SendWinodwShowMessage();
		return FALSE;

		//	已经有一个应用程序在运行了		
		TCHAR szMsgCaption[ MAX_PATH ]	= {0};
		TCHAR szString[ MAX_PATH ]	= {0};

		//	Warning
		LoadString( AfxGetInstanceHandle(), IDS_MSG_CAPTION_WARNING, szMsgCaption, sizeof(szMsgCaption) );
		//	Configuration tool alread running.
		LoadString( AfxGetInstanceHandle(), IDS_ALREADYRUN, szString, sizeof(szString) );
		//	
		MessageBox( NULL, szString, szMsgCaption, MB_ICONWARNING );

		return FALSE;
	}

	//
	//	如果配置目录不存在，则创建之
	//
	if ( ! PathIsDirectory( CVwFirewallConfigFile::m_szConfDir ) )
	{
		CreateDirectory( CVwFirewallConfigFile::m_szConfDir, NULL );
	}

	// Init RichEdit
	AfxInitRichEdit();


	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CDlgSplash dlgSplash;
	dlgSplash.DoModal();


	m_pMainDlg = new CVwFirewallCfgDlg();
	m_pMainWnd   =   m_pMainDlg;
	if ( ! m_pMainDlg->Create( IDD_VWFIREWALLCFG_DIALOG ) )
		return FALSE;
	m_pMainDlg->ShowWindow( SW_MINIMIZE );
	m_pMainDlg->ShowWindow( SW_HIDE );
	m_pMainDlg->UpdateWindow();
	return TRUE;


/*
	CVwFirewallCfgDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CVwFirewallCfgApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if ( m_pMainDlg )
	{
		delete m_pMainDlg;
	}

	return CWinApp::ExitInstance();
}




/**
 *	强制让正在运行的 VwCfgNew 配置管理中心退出
 */
BOOL CVwFirewallCfgApp::SendWinodwCloseMessage()
{
	BOOL  bRet			= FALSE;
	CString strCaption;
	HWND  hWnd			= NULL;
	
	strCaption.Format( IDS_APP_CAPTION );
	hWnd = ::FindWindow( NULL, strCaption );	
	if ( hWnd )
	{
		::SendMessage( hWnd, WM_CLOSE, 200500, 200500 );
		
		Sleep( 1000 );
		if ( NULL == ::FindWindow( NULL, strCaption ) )
		{
			bRet = TRUE;
		}
	}
	
	//	..
	return ( IsCfgRunning() ? FALSE : TRUE );
}

/**
 *	强制让正在运行的 VwCfgNew 配置管理中心退出
 */
VOID CVwFirewallCfgApp::SendWinodwShowMessage()
{
	HWND  hWnd;
	CString strCaption;

	strCaption.Format( IDS_APP_CAPTION );
	hWnd = ::FindWindow( NULL, strCaption );
	if ( hWnd )
	{
		::SendMessage( hWnd, WM_COMMAND, WM_RBUTTONUP, 0 );
		::SendMessage( hWnd, WM_COMMAND, IDM_MENU_SYSTRAY_OPEN, 0 );

		//	Dirty hack to bring the window to the foreground
		::SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
		::SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
		::SetFocus( hWnd );
	}
}

/**
 *	判断 VwCfgNew 是否正在运行
 */
BOOL CVwFirewallCfgApp::IsCfgRunning()
{
	//	..
	BOOL   bExist	= FALSE;
	HANDLE hMutex = CreateMutex( NULL, TRUE, MUTEX_VWFIREWALLCFG_NAME );
	if ( hMutex )
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			//	已经有一个应用程序在运行了
			bExist = TRUE;
		}
		//CloseHandle( hMutex );
	}
	
	return bExist;
}

/**
 *	向服务器端发送安装请求
 */
BOOL CVwFirewallCfgApp::SendInstallRequest( BOOL bInstall )
{
	BOOL bRet			= FALSE;
	CDeHttp cDeHttp;
	TCHAR szUrl[ MAX_PATH ]		= {0};
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	TCHAR szProjectName[ 64 ];

	__try
	{
		_sntprintf( szProjectName, sizeof(szProjectName)/sizeof(TCHAR)-1, _T("%s"), PROJECT_NAMET );
		_tcslwr( szProjectName );
		_sntprintf
		(
			szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
			"http://install.xingworld.net/%s/%s/?cver=%s",
			szProjectName,					//	产品名
			( bInstall ? "install" : "uninstall" ),
			CVwFirewallConfigFile::m_szSysDriverVersion
		);

		bRet = cDeHttp.GetResponse( szUrl, szResponse, sizeof(szResponse), szError, 3 * 1000 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}


