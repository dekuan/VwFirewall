// TabFile.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabFile.h"
#include "DlgProtectedDirAdd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFile dialog


CTabFile::CTabFile(CWnd* pParent /*=NULL*/)
	: CDialog(CTabFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabFile)
	//}}AFX_DATA_INIT
}


void CTabFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFile)
	DDX_Control(pDX, IDC_LIST_PROTECTEDDIR, m_listProtectedDir);
	DDX_Control(pDX, IDC_LIST_EXCEPTEDPROCESS, m_listExceptedProcess);
	DDX_Control(pDX, IDC_BUTTON_MOD_DIR, m_btnMod_Dir);
	DDX_Control(pDX, IDC_BUTTON_DEL_DIR, m_btnDel_Dir);
	DDX_Control(pDX, IDC_BUTTON_ADD_DIR, m_btnAdd_Dir);
	DDX_Control(pDX, IDC_BUTTON_DEL_EP, m_btnDel_Ep);
	DDX_Control(pDX, IDC_BUTTON_ADD_EP, m_btnAdd_Ep);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestore);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFile, CDialog)
	//{{AFX_MSG_MAP(CTabFile)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ADD_DIR, OnButtonAddDir)
	ON_BN_CLICKED(IDC_BUTTON_DEL_DIR, OnButtonDelDir)
	ON_BN_CLICKED(IDC_BUTTON_MOD_DIR, OnButtonModDir)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROTECTEDDIR, OnItemchangedListProtecteddir)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EXCEPTEDPROCESS, OnItemchangedListExceptedprocess)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROTECTEDDIR, OnDblclkListProtecteddir)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EP, OnButtonAddEp)
	ON_BN_CLICKED(IDC_BUTTON_DEL_EP, OnButtonDelEp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFile message handlers

BOOL CTabFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//
	m_btnSave.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_APPLY) ), RGB(0xFF,0x00,0xFF) );
	m_btnSave.SetFlat(FALSE);
	
	m_btnRestore.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_RESTORE) ), RGB(0xFF,0x00,0xFF) );
	m_btnRestore.SetFlat(FALSE);


	//	状态
	//m_strString.Format( IDS_LISTN_STATUS );
	m_listProtectedDir.InsertColumn( 0, "被保护目录", LVCFMT_LEFT, 310 );
	m_listProtectedDir.InsertColumn( 1, "子目录保护", LVCFMT_LEFT, 80 );
	m_listProtectedDir.InsertColumn( 2, "例外的扩展名", LVCFMT_LEFT, 150 );
	m_listProtectedDir.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );

	SHFILEINFO stSfi;
	ListView_SetImageList( m_listExceptedProcess.m_hWnd, SHGetFileInfo("",0,&stSfi,sizeof(stSfi),SHGFI_SYSICONINDEX|SHGFI_SMALLICON), LVSIL_SMALL );
	m_listExceptedProcess.InsertColumn( 0, "进程完整文件名", LVCFMT_LEFT, 500 );
	m_listExceptedProcess.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );


	//
	//	...
	//
	LoadConfig();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTabFile::PreTranslateMessage(MSG* pMsg) 
{
	//	拦截用户按下的 ESC 键，强迫用户点 X 退出
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			return FALSE;
		}
	}
	else if ( WM_COMMAND == pMsg->message )
	{
		//
		//	设置语言
		//
		if ( pMsg->wParam >= 10000 && pMsg->wParam <= 10300 )
		{
			AddExceptedProcess( pMsg->wParam );
			return FALSE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}



void CTabFile::OnLButtonDown(UINT nFlags, CPoint point) 
{
	/*
	//
	//	处理任何位置的拖拽效果
	//
	HWND hParent = ::GetParent( m_hWnd );
	if ( hParent )
	{
		::PostMessage( hParent, WM_NCLBUTTONDOWN, HTCAPTION,MAKELPARAM( point.x, point.y ) );
	}
	*/
	
	//	向系统发送 HTCAPTION 消息，让系统以为鼠标点在标题栏上
	CDialog::OnLButtonDown(nFlags, point);
}

void CTabFile::OnButtonSave() 
{
	if ( SaveConfig() )
	{
		//
		//	通知驱动程序: 装载新的配置信息
		//
		CTabBaseInfo::NotifyDriverToLoadNewConfig();
		
		//
		//	装载新配置
		//
		LoadConfig();
		
		//
		//	失效 保存、恢复 按钮
		//
		EnableButtonsDir( FALSE );
		EnableButtonsEp( FALSE );
	}
}

void CTabFile::OnButtonRestore() 
{
	OnCancel();

	//LoadConfig();
}

void CTabFile::OnItemchangedListProtecteddir(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if ( CTabBaseInfo::IsCheckboxChanged( pNMListView ) )
	{
		EnableButtonsDir( TRUE );
	}
	
	*pResult = 0;
}

void CTabFile::OnItemchangedListExceptedprocess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if ( CTabBaseInfo::IsCheckboxChanged( pNMListView ) )
	{
		EnableButtonsEp( TRUE );
	}
	
	*pResult = 0;
}

void CTabFile::OnDblclkListProtecteddir(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonModDir();

	*pResult = 0;
}


void CTabFile::OnButtonAddDir() 
{
	CDlgProtectedDirAdd dlg;
	UINT uItemNew;

	dlg.m_bUse	= TRUE;
	dlg.m_bTree	= TRUE;
	_tcscpy( dlg.m_szEcpExt, _T(".jpg.jpeg.gif.png") );
	if ( IDOK == dlg.DoModal() )
	{
		if ( -1 == GetListIndexByString( &m_listProtectedDir, 0, dlg.m_szDir ) )
		{
			uItemNew = m_listProtectedDir.InsertItem( 0, dlg.m_szDir );
			m_listProtectedDir.SetCheck( uItemNew, dlg.m_bUse );
			m_listProtectedDir.SetItemText( uItemNew, 1, dlg.m_bTree ? "1" : "0" );
			m_listProtectedDir.SetItemText( uItemNew, 2, dlg.m_szEcpExt );

			EnableButtonsDir( TRUE );
		}
		else
		{
			//	您输入的新计算机名已经存在。
			m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
			m_strString.Format( "您输入的新目录已经存在。" );
			MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		}
	}
}

void CTabFile::OnButtonDelDir() 
{
	POSITION p_ListPos	= m_listProtectedDir.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;
	DWORD ibe_id		= 0;
	
	if ( p_ListPos )
	{
		uItemSel = m_listProtectedDir.GetNextSelectedItem( p_ListPos );
		m_listProtectedDir.DeleteItem( uItemSel );
		
		EnableButtonsDir( TRUE );
	}
	else
	{
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( "请先选择一条记录。" );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}	
}

void CTabFile::OnButtonModDir() 
{
	CDlgProtectedDirAdd dlg;
	POSITION p_ListPos	= m_listProtectedDir.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;
	TCHAR szTemp[ 32 ];

	if ( p_ListPos )
	{
		uItemSel	= m_listProtectedDir.GetNextSelectedItem( p_ListPos );
		dlg.m_bUse	= m_listProtectedDir.GetCheck( uItemSel );
		m_listProtectedDir.GetItemText( uItemSel, 0, dlg.m_szDir, sizeof(dlg.m_szDir) );
		m_listProtectedDir.GetItemText( uItemSel, 1, szTemp, sizeof(szTemp) );
		dlg.m_bTree	= ( 0 == _tcsicmp( "1", szTemp ) ? TRUE : FALSE );
		m_listProtectedDir.GetItemText( uItemSel, 2, dlg.m_szEcpExt, sizeof(dlg.m_szEcpExt) );

		if ( IDOK == dlg.DoModal() )
		{
			if ( -1 == GetListIndexByString( &m_listProtectedDir, 0, dlg.m_szDir, uItemSel ) )
			{
				m_listProtectedDir.SetCheck( uItemSel, dlg.m_bUse );
				m_listProtectedDir.SetItemText( uItemSel, 0, dlg.m_szDir );
				m_listProtectedDir.SetItemText( uItemSel, 1, dlg.m_bTree ? "1" : "0" );
				m_listProtectedDir.SetItemText( uItemSel, 2, dlg.m_szEcpExt );

				EnableButtonsDir( TRUE );
			}
			else
			{
				//	您编辑的计算机名与现有其他计算机名重复。
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( "您编辑的目录名与现有其他目录名重复。" );
				MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
			}
		}
	}
	else
	{
		//	请先选择一条记录。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABRDP_ERR_OP1 );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}
}


//////////////////////////////////////////////////////////////////////////

void CTabFile::OnButtonAddEp() 
{
	RECT   rect;
	CMenu  menu;
	CMenu * pPopup;
	
	//	..
	::GetWindowRect( m_btnAdd_Ep.m_hWnd, &rect );
	
	menu.LoadMenu( IDR_MENU_ADD_EP );
	pPopup = menu.GetSubMenu(0);

	if ( pPopup && pPopup->m_hMenu )
	{
		//pPopup->DeleteMenu( 0, 0 );
		pPopup->InsertMenu( 0, MF_ENABLED|MF_STRING, CTABFILE_MENU_ADDEP_EXPLORER, "添加“Windows资源管理器”为例外进程" );
		pPopup->InsertMenu( 0, MF_ENABLED|MF_STRING, CTABFILE_MENU_ADDEP_SERV_U, "添加“Serv-U Ftp Server”为例外进程" );
		pPopup->AppendMenu( MF_ENABLED|MF_STRING, CTABFILE_MENU_ADDEP_OTHERS, "添加其他应用程序.." );

		pPopup->TrackPopupMenu( TPM_RIGHTBUTTON, rect.left, rect.bottom, this );
	}

	menu.Detach();
}

VOID CTabFile::AddExceptedProcess( DWORD dwMenuID )
{
	BOOL bGetSucc;
	CFileDialog * pFileDlg;
	TCHAR szPathName[ MAX_PATH ];
	TCHAR szWindowsDir[ MAX_PATH ];
	TCHAR szServUDir[ MAX_PATH ];
	DWORD dwSize;
	DWORD dwType;


	//	...
	bGetSucc = FALSE;

	switch( dwMenuID )
	{
	case CTABFILE_MENU_ADDEP_EXPLORER:
		{
			//	"C:\\Windows\\Explorer.exe"
			if ( GetWindowsDirectory( szWindowsDir, sizeof(szWindowsDir) ) > 0 )
			{
				_sntprintf( szPathName, sizeof(szPathName)/sizeof(TCHAR)-1, _T("%s\\explorer.exe"), szWindowsDir );
				if ( PathFileExists( szPathName ) )
				{
					bGetSucc = TRUE;
				}
			}
		}
		break;
	case CTABFILE_MENU_ADDEP_SERV_U:
		{
			dwSize	= sizeof(szServUDir);
			dwType	= REG_SZ;

			if ( ERROR_SUCCESS == SHGetValue
				(
					HKEY_LOCAL_MACHINE,
					"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Serv-U",
					"Path",
					&dwType, szServUDir, &dwSize
				)
			)
			{
				_sntprintf( szPathName, sizeof(szPathName)/sizeof(TCHAR)-1, _T("%s\\ServUDaemon.exe"), szServUDir );
				if ( PathFileExists( szPathName ) )
				{
					bGetSucc = TRUE;
				}
				else
				{
					_sntprintf( szPathName, sizeof(szPathName)/sizeof(TCHAR)-1, _T("%s\\Serv-U.exe"), szServUDir );
					if ( PathFileExists( szPathName ) )
					{
						bGetSucc = TRUE;
					}
				}
			}

			if ( ! bGetSucc )
			{
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( "可能本机没有安装 Serv-U 服务，无法搜索到服务位置，您可以手动添加。" );
				MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
			}
		}
		break;
	case CTABFILE_MENU_ADDEP_OTHERS:
		{
			m_strCaption.Format( IDS_MSG_CAPTION_NOTICE );
			m_strString.Format( "注意：请不要添加 IIS 进程 w3wp.exe 或者其他应用服务器服务进程，否则文件保护将失去意义。\r\n\r\n点击“确定”开始选择程序路径。" );
			MessageBox( m_strString, m_strCaption, MB_ICONINFORMATION );

			pFileDlg = new CFileDialog( TRUE, ".*", NULL, OFN_FILEMUSTEXIST, _T("Exe files(*.exe)|*.exe||\0"), this );
			if ( pFileDlg )
			{
				if ( IDOK == pFileDlg->DoModal() )
				{
					_sntprintf( szPathName, sizeof(szPathName)/sizeof(TCHAR)-1, _T("%s"), pFileDlg->GetPathName() );
					if ( _tcslen( szPathName ) > 0 && PathFileExists( szPathName ) )
					{
						bGetSucc = TRUE;
					}
				}
				
				delete [] pFileDlg;
				pFileDlg = NULL;
			}
		}
		break;
	default:
		{
		}
		break;
	}

	//	...
	if ( bGetSucc )
	{
		if ( CProcFunc::IsValidExceptedProcessFile( szPathName ) )
		{
			if ( -1 == GetListIndexByString( &m_listExceptedProcess, 0, szPathName ) )
			{
				//	...	
				InsertNewExceptedProcess( szPathName, TRUE );
				EnableButtonsEp( TRUE );
			}
			else
			{
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( "您输入的可执行文件名已经存在。" );
				MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
			}
		}
		else
		{
			m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
			m_strString.Format( "请输入正确的例外进程路径。不要含有中文或者其他特殊字符。" );
			MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		}
	}
}
VOID CTabFile::InsertNewExceptedProcess( LPCTSTR lpcszPath, BOOL bChecked )
{
	SHFILEINFO stSfi;
	UINT uItemNew;

	if ( NULL == lpcszPath )
	{
		return;
	}
	if ( 0 == _tcslen(lpcszPath) )
	{
		return;
	}
	//if ( ! PathFileExists( lpcszPath ) )
	//{
	//	return;
	//}

	SHGetFileInfo( lpcszPath, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
	uItemNew = m_listExceptedProcess.InsertItem( m_listExceptedProcess.GetItemCount(), lpcszPath );
	m_listExceptedProcess.SetItem( uItemNew, 0, LVIF_IMAGE, NULL, stSfi.iIcon, 0, 0, 0 );
	m_listExceptedProcess.SetCheck( uItemNew, bChecked );
}

void CTabFile::OnButtonDelEp() 
{
	POSITION p_ListPos	= m_listExceptedProcess.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;
	DWORD ibe_id		= 0;
	
	if ( p_ListPos )
	{
		uItemSel = m_listExceptedProcess.GetNextSelectedItem( p_ListPos );
		m_listExceptedProcess.DeleteItem( uItemSel );
		
		EnableButtonsDir( TRUE );
	}
	else
	{
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( "请先选择一条记录。" );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}
}

void CTabFile::OnButtonModEp() 
{
	// TODO: Add your control notification handler code here
	
}







//////////////////////////////////////////////////////////////////////////
//	Private
//	





VOID CTabFile::EnableButtonsDir( BOOL bEnabled, BOOL bAllButtons /* = FALSE */ )
{
	m_btnSave.EnableWindow( bEnabled );
	//m_btnRestore.EnableWindow( bEnabled );
	
	if ( bAllButtons )
	{
		m_btnAdd_Dir.EnableWindow( bEnabled );
		m_btnDel_Dir.EnableWindow( bEnabled );
		m_btnMod_Dir.EnableWindow( bEnabled );
	}
}

VOID CTabFile::EnableButtonsEp( BOOL bEnabled, BOOL bAllButtons /* = FALSE */ )
{
	m_btnSave.EnableWindow( bEnabled );
	//m_btnRestore.EnableWindow( bEnabled );
	
	if ( bAllButtons )
	{
		m_btnAdd_Ep.EnableWindow( bEnabled );
		m_btnDel_Ep.EnableWindow( bEnabled );
	}
}






/**
 *	装载配置信息
 */
BOOL CTabFile::LoadConfig()
{
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;
	UINT uItemNew;

	TCHAR szTemp[ MAX_PATH ];
	STPROTECTEDDIR stProtectedDir;
	STEXCEPTEDPROCESS stExceptedProcess;

	//
	//	[Domain]
	//	domain="use:1;dm:.chinapig.cn;ecpext:abc,www;"
	//	domain="use:1;dm:.chinameate.cn;ecpext:abc,www;"
	//

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//
	//	Load Protected Dir
	//
	nSecsCount = delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_PROTECTEDDIR, NULL, FALSE );
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			if ( nSecsCount ==
				delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_PROTECTEDDIR, pstIniSecs, FALSE ) )
			{
				m_listProtectedDir.DeleteAllItems();
				for ( i = 0; i < nSecsCount; i ++ )
				{
					memset( & stProtectedDir, 0, sizeof(stProtectedDir) );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_DIR, ";", stProtectedDir.szDir, sizeof(stProtectedDir.szDir), FALSE );

					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_USE, szTemp, sizeof(szTemp) );
					stProtectedDir.bUse = ( 0 == strcmp( szTemp, "1" ) ? TRUE : FALSE );

					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_TREE, szTemp, sizeof(szTemp) );
					stProtectedDir.bProtSubDir = ( 0 == strcmp( szTemp, "1" ) ? TRUE : FALSE );

					delib_get_casecookie_value_ex( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_ECPEXT, ";", stProtectedDir.szExceptedExt, sizeof(stProtectedDir.szExceptedExt), FALSE );

					//
					//	插入到 ListView
					//
					if ( _tcslen( stProtectedDir.szDir ) )
					{
						uItemNew = m_listProtectedDir.InsertItem( m_listProtectedDir.GetItemCount(), stProtectedDir.szDir );
						m_listProtectedDir.SetCheck( uItemNew, stProtectedDir.bUse );
						m_listProtectedDir.SetItemText( uItemNew, 1, stProtectedDir.bProtSubDir ? "1" : "0" );
						m_listProtectedDir.SetItemText( uItemNew, 2, stProtectedDir.szExceptedExt );
					}
				}
			}

			delete [] pstIniSecs;
			pstIniSecs = NULL;
		}
	}

	
	//
	//	Load Excepted Processes
	//
	nSecsCount = delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_EXCEPTEDPROCESS, NULL, FALSE );
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			if ( nSecsCount ==
				delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_EXCEPTEDPROCESS, pstIniSecs, FALSE ) )
			{
				m_listExceptedProcess.DeleteAllItems();
				for ( i = 0; i < nSecsCount; i ++ )
				{
					memset( & stExceptedProcess, 0, sizeof(stExceptedProcess) );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM, ";", stExceptedProcess.szPath, sizeof(stExceptedProcess.szPath), FALSE );
	
					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value_ex( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_USE, ";", szTemp, sizeof(szTemp), FALSE );
					stExceptedProcess.bUse = ( 0 == strcmp( szTemp, "1" ) ? TRUE : FALSE );

					//
					//	插入到 ListView
					//
					if ( _tcslen( stExceptedProcess.szPath ) )
					{
						InsertNewExceptedProcess( stExceptedProcess.szPath, stExceptedProcess.bUse );
					}
				}
			}
			
			delete [] pstIniSecs;
			pstIniSecs = NULL;
		}
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );


	return TRUE;
}

/**
 *	保存配置信息
 */
BOOL CTabFile::SaveConfig()
{
	BOOL bRet	= FALSE;
	UINT i;

	UINT uItemCountPtDir;
	UINT uItemCountEp;
	CString strLine;
	TCHAR szBufferPtDir[ 65535 ];
	TCHAR szBufferEp[ 65535 ];

	STPROTECTEDDIR stProtectedDir;
	STEXCEPTEDPROCESS stExceptedProcess;
	TCHAR szTemp[ 32 ];


	uItemCountPtDir = m_listProtectedDir.GetItemCount();
	uItemCountEp	= m_listExceptedProcess.GetItemCount();
	if ( uItemCountPtDir > 100 )
	{
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( "请不要添加多于 100 个被保护目录。" );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		return FALSE;
	}
	if ( uItemCountEp > 100 )
	{
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( "请不要添加多于 100 个例外的访问进程。" );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		return FALSE;
	}

	//	清理列表先
	memset( szBufferPtDir, 0, sizeof(szBufferPtDir) );
	memset( szBufferEp, 0, sizeof(szBufferEp) );

	//
	//	Collectting Protected Directory
	//
	if ( uItemCountPtDir > 0 )
	{	
		for ( i = 0; i < uItemCountPtDir; i ++ )
		{
			memset( & stProtectedDir, 0, sizeof(stProtectedDir) );

			stProtectedDir.bUse	= m_listProtectedDir.GetCheck( i );
			m_listProtectedDir.GetItemText( i, 0, stProtectedDir.szDir, sizeof(stProtectedDir.szDir) );
			memset( szTemp, 0, sizeof(szTemp) );
			m_listProtectedDir.GetItemText( i, 1, szTemp, sizeof(szTemp) );
			stProtectedDir.bProtSubDir = ( 0 == _tcsicmp( "1", szTemp ) ? TRUE : FALSE );
			m_listProtectedDir.GetItemText( i, 2, stProtectedDir.szExceptedExt, sizeof(stProtectedDir.szExceptedExt) );
			
			StrTrim( stProtectedDir.szDir, "\r\n\t " );
			StrTrim( stProtectedDir.szExceptedExt, _T(" \r\n\t") );
			delib_replace( stProtectedDir.szExceptedExt, _T(" "), _T("") );

			if ( _tcslen( stProtectedDir.szDir ) )
			{
				//
				//	[ProtectedDir]
				//	protecteddir="use:1;dir:C:\Inetpub\wwwroot\;tree:1;allext:1;protext:asp.aspx;ecpext:.gif.;"
				//
				strLine	= "";
				strLine += CVWFIREWALLCONFIGFILE_INI_KEY_PROTECTEDDIR;
				strLine += "=\"";
				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_USE;
				strLine += ( stProtectedDir.bUse ? "1" : "0" );
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_DIR;
				strLine += stProtectedDir.szDir;
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_TREE;
				strLine += ( stProtectedDir.bProtSubDir ? "1" : "0" );
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_ALLEXT;
				strLine += "1";
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_PROTEXT;
				strLine += "";
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_ECPEXT;
				strLine += stProtectedDir.szExceptedExt;
				strLine += ";";

				strLine += "\"\r\n";

				if ( _tcslen(szBufferPtDir) + strLine.GetLength() + 2 < sizeof(szBufferPtDir) )
				{
					_tcscat( szBufferPtDir, strLine.GetBuffer(0) );
					strLine.ReleaseBuffer();
					//szBufferPtDir[ _tcslen(szBufferPtDir) + 1 ] = NULL;
				}
				else
				{
					break;
				}
			}
		}
	}

	//
	//	Collectting Excepted Process
	//
	if ( uItemCountEp > 0 )
	{	
		for ( i = 0; i < uItemCountEp; i ++ )
		{
			memset( & stExceptedProcess, 0, sizeof(stExceptedProcess) );
			
			stExceptedProcess.bUse	= m_listExceptedProcess.GetCheck( i );
			m_listExceptedProcess.GetItemText( i, 0, stExceptedProcess.szPath, sizeof(stExceptedProcess.szPath) );
			StrTrim( stExceptedProcess.szPath, "\r\n\t " );

			if ( _tcslen( stExceptedProcess.szPath ) )
			{
				//
				//	[ExceptedProcess]
				//	exceptedprocess="use:1;item:C:\WINDOWS\explorer.exe;"
				//
				strLine	= "";
				strLine += CVWFIREWALLCONFIGFILE_INI_KEY_EXCEPTEDPROCESS;
				strLine += "=\"";
				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_USE;
				strLine += ( stExceptedProcess.bUse ? "1" : "0" );
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM;
				strLine += stExceptedProcess.szPath;
				strLine += ";";

				strLine += "\"\r\n";

				if ( _tcslen(szBufferEp) + strLine.GetLength() + 2 < sizeof(szBufferEp) )
				{
					_tcscat( szBufferEp, strLine.GetBuffer(0) );
					strLine.ReleaseBuffer();
				}
				else
				{
					break;
				}
			}
		}
	}

	if ( _tcslen(szBufferPtDir) + 2 < sizeof(szBufferPtDir) &&
		_tcslen(szBufferEp) + 2 < sizeof(szBufferEp) )
	{
		//	AAA\r\nN
		//	0123 4 5 
		//	BBB\r\nN
		//	CCC\r\nNN
		if ( _tcslen(szBufferPtDir) > 2 )
		{
			//	去掉最后的 \r\n
			szBufferPtDir[ _tcslen(szBufferPtDir) - 1 ] = NULL;
			szBufferPtDir[ _tcslen(szBufferPtDir) - 1 ] = NULL;
		}
		if ( _tcslen(szBufferEp) > 2 )
		{
			//	去掉最后的 \r\n
			szBufferEp[ _tcslen(szBufferEp) - 1 ] = NULL;
			szBufferEp[ _tcslen(szBufferEp) - 1 ] = NULL;
		}

		delib_drv_wow64_enablewow64_fs_redirection( FALSE );
		
		bRet = TRUE;
		bRet &= WritePrivateProfileSection( CVWFIREWALLCONFIGFILE_INI_DOMAIN_PROTECTEDDIR, szBufferPtDir, CVwFirewallConfigFile::m_szCfgFile );
		bRet &= WritePrivateProfileSection( CVWFIREWALLCONFIGFILE_INI_DOMAIN_EXCEPTEDPROCESS, szBufferEp, CVwFirewallConfigFile::m_szCfgFile );
		
		delib_drv_wow64_enablewow64_fs_redirection( TRUE );

		//	恭喜，配置信息保存成功！
		m_strCaption.Format( IDS_MSG_CAPTION_CONGLT );
		m_strString.Format( IDS_TABRDP_ERR_SAVESUCC );
		MessageBox( m_strString, m_strCaption, MB_ICONINFORMATION );
	}
	else
	{
		//	内部错误，数据过于庞大，无法保存！
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABRDP_ERR_BIGDATA );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}







	return bRet;
}


