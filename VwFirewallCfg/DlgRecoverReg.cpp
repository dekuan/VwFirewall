// DlgRecoverReg.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgRecoverReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverReg dialog


CDlgRecoverReg::CDlgRecoverReg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRecoverReg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRecoverReg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	//	...
	m_dwType		= CDLGRECOVERREG_TYPE_OBJECT;
	m_hParentWnd		= NULL;
	m_pszRecoverRegCmdLine	= NULL;

	//	...
	m_pszBackupDir		= NULL;
}


void CDlgRecoverReg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRecoverReg)
	DDX_Control(pDX, IDC_STATIC_HINT, m_stcHint);
	DDX_Control(pDX, IDC_BUTTON_RECOVER, m_btnRecover);
	DDX_Control(pDX, IDC_LIST_FILE, m_listFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRecoverReg, CDialog)
	//{{AFX_MSG_MAP(CDlgRecoverReg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE, OnClickListFile)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER, OnButtonRecover)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverReg message handlers

BOOL CDlgRecoverReg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//
	//	...
	//
	if ( CDLGRECOVERREG_TYPE_SERVICE == m_dwType )
	{
		m_pszBackupDir = m_cVwFirewallConfigFile.m_szBackupServiceDir;
	}
	else
	{
		//	m_dwType = CDLGRECOVERREG_TYPE_OBJECT
		m_pszBackupDir = m_cVwFirewallConfigFile.m_szBackupObjectDir;
	}


	// TODO: Add extra initialization here
	SHFILEINFO stSfi;
	ListView_SetImageList( m_listFile.m_hWnd, SHGetFileInfo("",0,&stSfi,sizeof(stSfi),SHGFI_SYSICONINDEX|SHGFI_SMALLICON), LVSIL_SMALL );
	m_listFile.InsertColumn( 0, "自动备份文件", LVCFMT_LEFT, 280 );
	m_listFile.InsertColumn( 1, "文件大小", LVCFMT_RIGHT, 100 );
	m_listFile.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP );

	//	...
	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


VOID CDlgRecoverReg::InitList()
{
	SHFILEINFO stSfi;
	UINT uItemNew;
	
	HANDLE hFindFile;
	WIN32_FIND_DATA stFindFileData;
	TCHAR szToFind[ MAX_PATH ];
//	TCHAR szNewSource[MAX_PATH];
	TCHAR szFullFilepath[MAX_PATH];
	DWORD dwFileSize;
	TCHAR szTemp[ MAX_PATH ];

	
	memset( szToFind, 0, sizeof(szToFind) );
	_sntprintf( szToFind, sizeof(szToFind)/sizeof(TCHAR)-1, "%s%s", m_pszBackupDir, "*.*" );

	__try
	{
		hFindFile = FindFirstFile( szToFind, &stFindFileData );
		if ( INVALID_HANDLE_VALUE != hFindFile )
		{			
			do
			{
				if ( 0 == _tcsicmp( stFindFileData.cFileName, "." ) || 0 == _tcsicmp( stFindFileData.cFileName, ".." ) )
				{
					continue;
				}
				if ( FILE_ATTRIBUTE_DIRECTORY == ( FILE_ATTRIBUTE_DIRECTORY & stFindFileData.dwFileAttributes ) )
				{
				}
				else
				{
					memset( szFullFilepath, 0, sizeof(szFullFilepath) );
					_sntprintf( szFullFilepath, sizeof(szFullFilepath)/sizeof(TCHAR)-1, "%s%s", m_pszBackupDir, stFindFileData.cFileName );

					if ( CDLGRECOVERREG_TYPE_SERVICE == m_dwType )
					{
						SHGetFileInfo( m_cVwFirewallConfigFile.mb_szSysServicesMscFile, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
					}
					else
					{
						//	CDLGRECOVERREG_TYPE_OBJECT
						SHGetFileInfo( m_cVwFirewallConfigFile.mb_szSysRegeditFile, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
					}
					_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "%s", stFindFileData.cFileName );
					delib_replace( szTemp, _T(".reg"), _T("") );
					delib_replace( szTemp, _T("_"), _T(" ") );
					delib_replace( szTemp, _T("."), _T(":") );

					uItemNew = m_listFile.InsertItem( m_listFile.GetItemCount(), szTemp );
					m_listFile.SetItem( uItemNew, 0, LVIF_IMAGE, NULL, stSfi.iIcon, 0, 0, 0 );

					dwFileSize = delib_get_file_size( szFullFilepath );
					if ( dwFileSize > 1024 * 1024 )
					{
						_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "%d MB", dwFileSize / 1024 / 1024 );
						m_listFile.SetItemText( uItemNew, 1, szTemp );
					}
					else if ( dwFileSize > 1024 )
					{
						_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "%d KB", dwFileSize / 1024 );
						m_listFile.SetItemText( uItemNew, 1, szTemp );
					}
					else
					{
						_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "%d Bytes", dwFileSize );
						m_listFile.SetItemText( uItemNew, 1, szTemp );
					}
				}
			}while( FindNextFile( hFindFile, &stFindFileData ) );

			FindClose( hFindFile );
			hFindFile = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

void CDlgRecoverReg::OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL bHasSel;
	INT nItemSel;
	POSITION p_ListPos;
	TCHAR szFilename[MAX_PATH];

	bHasSel		= FALSE;
	p_ListPos	= m_listFile.GetFirstSelectedItemPosition();
	memset( szFilename, 0, sizeof(szFilename) );

	if ( p_ListPos )
	{
		nItemSel = m_listFile.GetNextSelectedItem(p_ListPos);
		m_listFile.GetItemText( nItemSel, 0, szFilename, sizeof(szFilename) );
		if ( _tcslen( szFilename ) > 0 )
		{
			bHasSel = TRUE;
		}
	}

	//
	//	...
	//
	m_btnRecover.EnableWindow( bHasSel );

	*pResult = 0;
}

void CDlgRecoverReg::OnButtonRecover() 
{
	BOOL bHasSel;
	INT nItemSel;
	POSITION p_ListPos;
	TCHAR szFilename[MAX_PATH];
	TCHAR szFullFilepath[MAX_PATH];
	TCHAR szCmdLine[ 1024 ];
	
	bHasSel		= FALSE;
	p_ListPos	= m_listFile.GetFirstSelectedItemPosition();
	memset( szFilename, 0, sizeof(szFilename) );
	memset( szFullFilepath, 0, sizeof(szFullFilepath) );
	memset( szCmdLine, 0, sizeof(szCmdLine) );

	//	...
	m_btnRecover.EnableWindow( FALSE );

	if ( p_ListPos &&
		m_pszRecoverRegCmdLine && m_dwRecoverRegCmdLineSize > 0 )
	{
		nItemSel = m_listFile.GetNextSelectedItem(p_ListPos);
		m_listFile.GetItemText( nItemSel, 0, szFilename, sizeof(szFilename) );
		if ( _tcslen( szFilename ) > 0 )
		{
			m_stcHint.SetWindowText( _T("正在初始化数据...") );
			m_listFile.EnableWindow( FALSE );

			delib_replace( szFilename, _T(" "), _T("_") );
			delib_replace( szFilename, _T(":"), _T(".") );
			_sntprintf( szFullFilepath, sizeof(szFullFilepath)/sizeof(TCHAR)-1, _T("%s%s.reg"), m_pszBackupDir, szFilename );
			_sntprintf
			(
				m_pszRecoverRegCmdLine,
				m_dwRecoverRegCmdLineSize/sizeof(TCHAR)-1,
				_T("%s /s \"%s\""),
				m_cVwFirewallConfigFile.mb_szSysRegeditFile,
				szFullFilepath
			);

			//	以下操作由父程序处理
			//delib_run_block_process_ex( szCmdLine, FALSE );


			//	...
			SetTimer( 1000, 100, NULL );
		}
	}

	//	...
	m_btnRecover.EnableWindow( TRUE );
}

void CDlgRecoverReg::OnTimer(UINT nIDEvent) 
{
	if ( 1000 == nIDEvent )
	{
		//MessageBox( "注册表还原成功，建议您立即重启计算机，以便所有配置生效。", "恭喜", MB_ICONINFORMATION );
		
		if ( m_hParentWnd )
		{
			::PostMessage( m_hParentWnd, UM_DO_NEXT, WPARAM_HEMLHELP_VWFIREWALL_DONEXT_RECOVER_REG, 0 );
		}

		//	...
		KillTimer( 1000 );
		OnOK();
	}	
	CDialog::OnTimer(nIDEvent);
}

