// TabSecurityCfgObject.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfgObject.h"

#include "DlgDetail.h"
#include "DlgRecoverReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgObject dialog


CTabSecurityCfgObject::CTabSecurityCfgObject(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSecurityCfgObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSecurityCfgObject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDoingFix		= FALSE;
	m_bFixInProgress	= FALSE;
	
	m_bDoingRecover		= FALSE;
	m_bRecoverInProgress	= FALSE;

	memset( m_szRecoverRegCmdLine, 0, sizeof(m_szRecoverRegCmdLine) );

	memset( m_szInfo, 0, sizeof(m_szInfo) );
	m_dwInfoType = 0;
}
CTabSecurityCfgObject::~CTabSecurityCfgObject()
{
	m_cThSleepFixAclsProblems.EndSleep();
	m_cThSleepFixAclsProblems.EndThread();
	
	m_cThSleepRecoverFixing.EndSleep();
	m_cThSleepRecoverFixing.EndThread();
}


void CTabSecurityCfgObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSecurityCfgObject)
	DDX_Control(pDX, IDC_LIST_GROUP, m_listGroup);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_proBar);
	DDX_Control(pDX, IDC_BUTTON_DETECT, m_btnDetect);
	DDX_Control(pDX, IDC_BUTTON_FIX, m_btnFix);
	DDX_Control(pDX, IDC_BUTTON_RECOVER, m_btnRecover);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSecurityCfgObject, CDialog)
	//{{AFX_MSG_MAP(CTabSecurityCfgObject)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, OnButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON_FIX, OnButtonFix)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER, OnButtonRecover)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUP, OnDblclkListGroup)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP, OnRclickListGroup)
	ON_COMMAND(IDM_MENU_LIST_DETAIL, OnMenuListDetail)
	ON_MESSAGE(UM_DO_NEXT, OnDoNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgObject message handlers

BOOL CTabSecurityCfgObject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//
	//	...
	//
	m_btnDetect.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_DETECT) ), RGB(0xFF,0x00,0xFF) );
	m_btnDetect.SetFlat( FALSE );
	
	m_btnFix.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_FIX) ), RGB(0xFF,0x00,0xFF) );
	m_btnFix.SetFlat( FALSE );
	
	m_btnRecover.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_RESTORE) ), RGB(0xFF,0x00,0xFF) );
	m_btnRecover.SetFlat( FALSE );


	//
	//	...
	//
	ListView_SetImageList( m_listGroup.m_hWnd, m_cTabSecurityCfgBase.m_hImageListGroup, LVSIL_SMALL );
	m_listGroup.InsertColumn( 0, "名称", LVCFMT_LEFT, 330 );
	m_listGroup.InsertColumn( 1, "状态", LVCFMT_LEFT, 70 );
	m_listGroup.InsertColumn( 2, "安全建议", LVCFMT_LEFT, 100 );
	m_listGroup.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );	
	
	//
	//	...
	//
	LoadDataFromCfgFile();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTabSecurityCfgObject::PreTranslateMessage(MSG* pMsg) 
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


void CTabSecurityCfgObject::OnButtonDetect() 
{
	DetectAclsProblems();
	m_btnFix.EnableWindow( TRUE );
	m_btnRecover.EnableWindow( TRUE );
}

void CTabSecurityCfgObject::OnButtonFix() 
{
	if ( ! m_bDoingFix )
	{
		if ( ! m_bFixInProgress )
		{
			//
			//	开启修复线程
			//
			m_cThSleepFixAclsProblems.m_hThread = (HANDLE)_beginthreadex
				(
					NULL,
					0,
					&_threadFixAclsProblems,
					(void*)this,
					0,
					&m_cThSleepFixAclsProblems.m_uThreadId
				);
		}
	}
	else
	{
		m_btnFix.EnableWindow( FALSE );
		m_btnFix.SetWindowText( "停止..." );
		m_bDoingFix = FALSE;
		m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( FALSE );
	}	
}

void CTabSecurityCfgObject::OnButtonRecover() 
{
	if ( ! m_bDoingRecover )
	{
		if ( ! m_bRecoverInProgress )
		{
			CDlgRecoverReg * pcDlgRecoverReg;
			
			memset( m_szRecoverRegCmdLine, 0, sizeof(m_szRecoverRegCmdLine) );
			pcDlgRecoverReg = new CDlgRecoverReg();
			pcDlgRecoverReg->m_dwType = CDLGRECOVERREG_TYPE_OBJECT;
			pcDlgRecoverReg->m_hParentWnd = m_hWnd;
			pcDlgRecoverReg->m_pszRecoverRegCmdLine = m_szRecoverRegCmdLine;
			pcDlgRecoverReg->m_dwRecoverRegCmdLineSize = sizeof(m_szRecoverRegCmdLine);
			pcDlgRecoverReg->Create( IDD_DLG_RECOVER_REG );
			pcDlgRecoverReg->ShowWindow( SW_SHOW );
			pcDlgRecoverReg->UpdateWindow();
		}
	}
	else
	{
		m_btnRecover.EnableWindow( FALSE );
		m_btnRecover.SetWindowText( "停止..." );
		m_bDoingRecover = FALSE;
		m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( FALSE );
	}
}

void CTabSecurityCfgObject::OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnMenuListDetail();
	*pResult = 0;
}

void CTabSecurityCfgObject::OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL bHasSel = FALSE;
	POSITION p_ListPos = m_listGroup.GetFirstSelectedItemPosition();
	if ( p_ListPos )
	{
		INT nItemSel = m_listGroup.GetNextSelectedItem(p_ListPos);
		CString strName = m_listGroup.GetItemText( nItemSel, 0 );
		if ( strName.GetLength() > 0 )
		{
			bHasSel = TRUE;
		}
	}

	NMITEMACTIVATE *pNotifier=(NMITEMACTIVATE*)pNMHDR;
	CPoint pt( pNotifier->ptAction.x,pNotifier->ptAction.y );
	ClientToScreen( &pt );
	CMenu menu;
	menu.LoadMenu( IDR_MENU_ACLS_LIST );
	if ( FALSE == bHasSel )
	{
		menu.GetSubMenu(0)->EnableMenuItem( IDM_MENU_LIST_DETAIL, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED );
	}
	menu.GetSubMenu(0)->TrackPopupMenu( TPM_RIGHTBUTTON, pt.x+2, pt.y+2, this );
	menu.Detach();

	*pResult = 0;
}

void CTabSecurityCfgObject::OnMenuListDetail() 
{
	INT nItemSel		= 0;
	INT nIndex;
	CString strFilePath	= _T("");
	CString strCmdLine	= _T("");
	POSITION p_ListPos	= m_listGroup.GetFirstSelectedItemPosition();
	CDlgDetail * pcDlgDetail;
	STPARSECFGDATAGROUP* pstAclsDataGroup;
	
	if ( p_ListPos )
	{
		nItemSel = m_listGroup.GetNextSelectedItem( p_ListPos );
		if ( nItemSel >=0 && nItemSel < m_listGroup.GetItemCount() )
		{
			nIndex = m_listGroup.GetItemData( nItemSel );
			if ( nIndex >= 0 && nIndex < m_CfgObjectData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgObjectData.m_vcAclsGroupList.at( nIndex );
				if ( pstAclsDataGroup )
				{
					//dlgDetail.m_pvcAclsSetList = & pstAclsDataGroup->vcAclsSetList;
					//dlgDetail.DoModal();
					pcDlgDetail = new CDlgDetail();
					pcDlgDetail->m_pvcAclsSetList = & pstAclsDataGroup->vcAclsSetList;
					pcDlgDetail->Create( IDD_DLG_DETAIL );
					pcDlgDetail->ShowWindow( SW_SHOW );
					pcDlgDetail->UpdateWindow();
				}
			}
		}
	}
}

LRESULT CTabSecurityCfgObject::OnDoNext( WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case WPARAM_HEMLHELP_VWFIREWALL_DONEXT_RECOVER_REG:
		{
			if ( ! m_bDoingRecover )
			{
				if ( ! m_bRecoverInProgress )
				{	
					//
					//	开启恢复线程
					//
					m_cThSleepRecoverFixing.m_hThread = (HANDLE)_beginthreadex
						(
							NULL,
							0,
							&_threadRecoverFixing,
							(void*)this,
							0,
							&m_cThSleepRecoverFixing.m_uThreadId
						);
				}
			}
			else
			{
				m_btnRecover.EnableWindow( FALSE );
				m_btnRecover.SetWindowText( "停止..." );
				m_bDoingRecover = FALSE;
				m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( FALSE );
			}	
		}
		break;
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//	Private
//

VOID CTabSecurityCfgObject::LoadDataFromCfgFile()
{
	//	...
	m_CfgObjectData.LoadData( m_cVwFirewallConfigFile.m_szCfgObjectData );

	m_proBar.SetRange( 0, m_CfgObjectData.m_vcAclsGroupList.size() );
	m_proBar.SetPos( 0 );
}

VOID CTabSecurityCfgObject::InitList()
{
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
//	TCHAR szPermissionsDesc[ MAX_PATH ];
//	TCHAR szTemp[ 1024 ];
//	TCHAR szSvcStatus[ MAX_PATH ];
//	TCHAR szSvcDesc[ MAX_PATH ];
	UINT uItemNew;
	UINT uCount;
//	LONG lnSvcStatus;

	if ( m_listGroup.GetItemCount() > 0 )
	{
		return;
	}

	m_proBar.ShowWindow( SW_SHOW );

	for ( m_CfgObjectData.m_it = m_CfgObjectData.m_vcAclsGroupList.begin();
		m_CfgObjectData.m_vcAclsGroupList.end() != m_CfgObjectData.m_it;
		m_CfgObjectData.m_it ++ )
	{
		pstAclsDataGroup = m_CfgObjectData.m_it;
		if ( pstAclsDataGroup )
		{
			if ( pstAclsDataGroup->vcAclsSetList.size() )
			{
				pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
				if ( pstAclsDataSet )
				{
					uCount	 = m_listGroup.GetItemCount();
					uItemNew = m_listGroup.InsertItem( uCount, pstAclsDataGroup->szGroupName );
					m_listGroup.SetCheck( uItemNew, FALSE );
					m_listGroup.SetItemData( uItemNew, pstAclsDataGroup->nIndex );
					m_listGroup.SetItemText( uItemNew, 1, _T("待诊断") );
					m_listGroup.SetItemText( uItemNew, 2, _T("双击查看..") );
				}
			}
		}

		m_proBar.SetPos( uCount );
	}

	m_proBar.SetPos( m_CfgObjectData.m_vcAclsGroupList.size() );
	//	...
	m_proBar.ShowWindow( SW_HIDE );
}

VOID CTabSecurityCfgObject::DetectAclsProblems()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
//	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
	INT  nCount;
	INT  nCountChecked;
	BOOL bChecked;
//	TCHAR szSvcStatus[ 64 ];

	//	...
	m_proBar.ShowWindow( SW_SHOW );
	nCount		= m_listGroup.GetItemCount();
	nCountChecked	= 0;

	for ( i = 0; i < nCount; i ++ )
	{
		bChecked	= FALSE;
		nIndex		= m_listGroup.GetItemData( i );
		if ( nIndex >= 0 && nIndex < m_CfgObjectData.m_vcAclsGroupList.size() )
		{
			m_CfgObjectData.m_it = & m_CfgObjectData.m_vcAclsGroupList.at( nIndex );
			pstAclsDataGroup = m_CfgObjectData.m_it;
			if ( pstAclsDataGroup )
			{
				if ( m_cTabSecurityCfgBase.IsNeedToFix( pstAclsDataGroup ) )
				{
					bChecked = TRUE;
				}
			}
		}

		if ( bChecked )
		{
			nCountChecked ++;
			m_listGroup.SetCheck( i, TRUE );
			m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 2, 0, 0, 0 );
			m_listGroup.SetItemText( i, 1, "待修复" );
		}
		else
		{
			m_listGroup.SetCheck( i, FALSE );
			m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
			m_listGroup.SetItemText( i, 1, "已修复" );
		}


		//m_listGroup.EnsureVisible( i, FALSE );
		m_proBar.SetPos( i );
	}

	m_proBar.SetPos( m_CfgObjectData.m_vcAclsGroupList.size() );

	if ( nCountChecked > 0 )
	{
		m_dwInfoType = (LPARAM)CTABSECURITYCFG_ICONTYPE_ALERT;
		_sntprintf( m_szInfo, sizeof(m_szInfo)/sizeof(TCHAR)-1, _T("诊断%d项，发现%d项问题待修复。"), nCount, nCountChecked );
	}
	else
	{
		m_dwInfoType = (LPARAM)CTABSECURITYCFG_ICONTYPE_OK;
		_sntprintf( m_szInfo, sizeof(m_szInfo)/sizeof(TCHAR)-1, _T("诊断%d项，未发现问题。"), nCount );
	}
	::SendMessage( ::GetParent( m_hWnd ), UM_DATACHANGE, (WPARAM)WPARAM_HEMLHELP_VWFIREWALL_DATA_DATACHANGE, 0 );
}


unsigned __stdcall CTabSecurityCfgObject::_threadFixAclsProblems( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgObject * pThis = (CTabSecurityCfgObject*)arglist;
		if ( pThis )
		{
			pThis->FixAclsProblemsProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CTabSecurityCfgObject::FixAclsProblemsProc()
{
	//	...
	m_btnFix.SetWindowText( "停止(&S)" );
	m_bFixInProgress	= TRUE;
	m_bDoingFix		= TRUE;
	m_btnDetect.EnableWindow( FALSE );
	m_btnRecover.EnableWindow( FALSE );
	m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( TRUE );
	m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );

	//
	m_cTabSecurityCfgBase.Regsvr32_Wshom_ocx( TRUE );
	BackupRegister();
	FixAclsProblems();
	m_cTabSecurityCfgBase.Regsvr32_Wshom_ocx( FALSE );

	//	...
	OnButtonDetect();

	//
	m_btnFix.SetWindowText( "修复(&F)" );
	m_btnFix.EnableWindow( TRUE );
	m_bFixInProgress	= FALSE;
	m_bDoingFix		= FALSE;
	m_btnDetect.EnableWindow( TRUE );
	m_btnRecover.EnableWindow( TRUE );
	m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( FALSE );
	m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
}
VOID CTabSecurityCfgObject::BackupRegister()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
	INT  nCount;
	INT  nTempFileNo;
	TCHAR szCurrentBakFile[ MAX_PATH ];
	SYSTEMTIME st;
	TCHAR szTempFile[ MAX_PATH ];
	TCHAR szCmdLine[ 1024 ];

	//
	//	检查备份目录是否存在，不存在创建之
	//
	if ( ! PathIsDirectory( m_cVwFirewallConfigFile.m_szBackupDir ) )
	{
		CreateDirectory( m_cVwFirewallConfigFile.m_szBackupDir, NULL );
	}
	if ( ! PathIsDirectory( m_cVwFirewallConfigFile.m_szBackupObjectDir ) )
	{
		CreateDirectory( m_cVwFirewallConfigFile.m_szBackupObjectDir, NULL );
	}

	nTempFileNo	= 0;
	nCount		= m_listGroup.GetItemCount();

	GetLocalTime( &st );
	_sntprintf( szCurrentBakFile, sizeof(szCurrentBakFile)/sizeof(TCHAR)-1, 
		_T("%sbackup_[%d-%02d-%02d_%02d.%02d.%02d].reg"), 
		m_cVwFirewallConfigFile.m_szBackupObjectDir,
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	for ( i = 0; i < nCount; i ++ )
	{
		if ( ! m_bDoingFix )
		{
			break;
		}

		m_listGroup.EnsureVisible( i, FALSE );
		m_proBar.SetPos( i );

		if ( m_listGroup.GetCheck( i ) )
		{
			nIndex = m_listGroup.GetItemData( i );
			if ( nIndex >= 0 && nIndex < m_CfgObjectData.m_vcAclsGroupList.size() )
			{
				m_CfgObjectData.m_it = & m_CfgObjectData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgObjectData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 1, "正在备份" );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, i, 0 );
					
					for ( pstAclsDataGroup->it = pstAclsDataGroup->vcAclsSetList.begin();
						pstAclsDataGroup->vcAclsSetList.end() != pstAclsDataGroup->it;
						pstAclsDataGroup->it ++
					)
					{
						pstAclsDataSet = pstAclsDataGroup->it;
						if ( pstAclsDataSet )
						{
							if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
							{
								_sntprintf( szTempFile, sizeof(szTempFile)/sizeof(TCHAR)-1, _T("%s__regbak%d.tmp"), m_cVwFirewallConfigFile.m_szBackupObjectDir, nTempFileNo );
								nTempFileNo ++;

								//	c:\windows\regedit.exe /e "E:\code\VC\VW\VwFirewall\VwFirewallCfg\Debug\backup\object\111.reg.tmp" "HKEY_CLASSES_ROOT\WScript.Shell"
								_sntprintf( szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s /e \"%s\" \"%s\\%s\"",
									m_cVwFirewallConfigFile.mb_szSysRegeditFile, szTempFile, 
									pstAclsDataSet->szRegRoot, pstAclsDataSet->szRegPath );
								delib_run_block_process_ex( szCmdLine, FALSE );

								//	...
								m_cTabSecurityCfgBase.SpellRegisterFile( szCurrentBakFile, szTempFile );
								DeleteFile( szTempFile );
							}
						}
					}

					Sleep( 1000 );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
					Sleep( 200 );

					m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 3, 0, 0, 0 );
					m_listGroup.SetItemText( i, 1, "已备份" );
				}
			}
		}
	}

	m_proBar.SetPos( m_CfgObjectData.m_vcAclsGroupList.size() );
}
VOID CTabSecurityCfgObject::FixAclsProblems()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
//	STPARSEACLSDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
	INT  nCount;
	//TCHAR szPermissionsDesc[ MAX_PATH ];
	//TCHAR szCmdLine[ 1024 ];


	//	...
	m_proBar.ShowWindow( SW_SHOW );
	nCount = m_listGroup.GetItemCount();
	

	for ( i = 0; i < nCount; i ++ )
	{
		if ( ! m_bDoingFix )
		{
			break;
		}

		m_listGroup.EnsureVisible( i, FALSE );
		m_proBar.SetPos( i );

		if ( m_listGroup.GetCheck( i ) )
		{
			nIndex = m_listGroup.GetItemData( i );
			if ( nIndex >= 0 && nIndex < m_CfgObjectData.m_vcAclsGroupList.size() )
			{
				m_CfgObjectData.m_it = & m_CfgObjectData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgObjectData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 1, "正在修复" );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, i, 0 );

					//
					//	Fix all sets in this group
					//
					m_cTabSecurityCfgBase.FixAclsProblemsInGroup( pstAclsDataGroup );
					
					//	...
					Sleep( 1000 );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
					Sleep( 200 );

					//
					//	Display the fixing result
					//
					if ( ! m_cTabSecurityCfgBase.IsNeedToFix( pstAclsDataGroup ) )
					{
						m_listGroup.SetCheck( i, FALSE );
						m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
						m_listGroup.SetItemText( i, 1, "已修复" );
					}
					else
					{
						m_listGroup.SetCheck( i, TRUE );
						m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 2, 0, 0, 0 );
						m_listGroup.SetItemText( i, 1, "修复失败" );
						//break;
					}
				}
			}
		}
	}

	m_proBar.SetPos( m_CfgObjectData.m_vcAclsGroupList.size() );
}






unsigned __stdcall CTabSecurityCfgObject::_threadRecoverFixing( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgObject * pThis = (CTabSecurityCfgObject*)arglist;
		if ( pThis )
		{
			pThis->RecoverFixingProc();
		}

		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CTabSecurityCfgObject::RecoverFixingProc()
{
	//	...
	m_btnRecover.SetWindowText( "停止(&S)" );
	m_bRecoverInProgress	= TRUE;
	m_bDoingRecover		= TRUE;
	m_btnDetect.EnableWindow( FALSE );
	m_btnFix.EnableWindow( FALSE );
	m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( TRUE );
	m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
	
	//
	m_cTabSecurityCfgBase.Regsvr32_Wshom_ocx( TRUE );
	RecoverFixing();
	m_cTabSecurityCfgBase.Regsvr32_Wshom_ocx( FALSE );

	//	...
	DetectAclsProblems();

	//
	m_btnRecover.SetWindowText( "还原(&R)" );
	m_btnRecover.EnableWindow( TRUE );
	m_bRecoverInProgress	= FALSE;
	m_bDoingRecover		= FALSE;
	m_btnDetect.EnableWindow( TRUE );
	m_btnFix.EnableWindow( TRUE );
	m_cTabSecurityCfgBase.SetFixAclsProblemsStatus( FALSE );
	m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
}
VOID CTabSecurityCfgObject::RecoverFixing()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	INT  nCount;
	

	//	...
	m_proBar.ShowWindow( SW_SHOW );
	nCount = m_listGroup.GetItemCount();
	
	
	for ( i = 0; i < nCount; i ++ )
	{
		if ( ! m_bDoingRecover )
		{
			break;
		}
		
		m_listGroup.EnsureVisible( i, FALSE );
		m_proBar.SetPos( i );
		
		//if ( m_listGroup.GetCheck( i ) )
		{
			nIndex = m_listGroup.GetItemData( i );
			if ( nIndex >= 0 && nIndex < m_CfgObjectData.m_vcAclsGroupList.size() )
			{
				m_CfgObjectData.m_it = & m_CfgObjectData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgObjectData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 1, "正在还原" );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, i, 0 );

					//
					//	Fix all sets in this group
					//
					m_cTabSecurityCfgBase.RecoverFixingInGroup( pstAclsDataGroup );

					Sleep( 1000 );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
					Sleep( 200 );

					m_listGroup.SetCheck( i, FALSE );
					m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 3, 0, 0, 0 );
					m_listGroup.SetItemText( i, 1, "已还原" );
				}
			}
		}
	}
	m_proBar.SetPos( m_CfgObjectData.m_vcAclsGroupList.size() );

	//
	//	将备份的注册表导入注册表
	//
	if ( _tcslen( m_szRecoverRegCmdLine ) )
	{
		delib_run_block_process_ex( m_szRecoverRegCmdLine, FALSE );
	}


	if ( i == nCount )
	{
		MessageBox( "恭喜，已经完成还原。为了让所有配置生效，建议：立即重启计算机。", "还原完成", MB_ICONINFORMATION );
	}
}
