// TabSecurityCfgService.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfgService.h"
#include "DlgService.h"
#include "DlgRecoverReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgService dialog


CTabSecurityCfgService::CTabSecurityCfgService(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSecurityCfgService::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSecurityCfgService)
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
CTabSecurityCfgService::~CTabSecurityCfgService()
{
	m_cThSleepFixAclsProblems.EndSleep();
	m_cThSleepFixAclsProblems.EndThread();

	m_cThSleepRecoverFixing.EndSleep();
	m_cThSleepRecoverFixing.EndThread();
}

void CTabSecurityCfgService::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSecurityCfgService)
	DDX_Control(pDX, IDC_BUTTON_RECOVER, m_btnRecover);
	DDX_Control(pDX, IDC_BUTTON_FIX, m_btnFix);
	DDX_Control(pDX, IDC_BUTTON_DETECT, m_btnDetect);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_proBar);
	DDX_Control(pDX, IDC_LIST_GROUP, m_listGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSecurityCfgService, CDialog)
	//{{AFX_MSG_MAP(CTabSecurityCfgService)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, OnButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON_FIX, OnButtonFix)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUP, OnDblclkListGroup)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP, OnRclickListGroup)
	ON_COMMAND(IDM_MENU_LIST_SVC_START, OnMenuListSvcStart)
	ON_COMMAND(IDM_MENU_LIST_SVC_STOP, OnMenuListSvcStop)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER, OnButtonRecover)
	ON_MESSAGE(UM_DO_NEXT, OnDoNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgService message handlers

BOOL CTabSecurityCfgService::OnInitDialog() 
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
	m_listGroup.InsertColumn( 0, "服务名", LVCFMT_LEFT, 120 );
	m_listGroup.InsertColumn( 1, "服务状态", LVCFMT_LEFT, 60 );
	m_listGroup.InsertColumn( 2, "状态", LVCFMT_LEFT, 70 );
	m_listGroup.InsertColumn( 3, "安全建议", LVCFMT_LEFT, 70 );
	m_listGroup.InsertColumn( 4, "服务描述", LVCFMT_LEFT, 180 );
	m_listGroup.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );
	
	
	//
	//	...
	//
	LoadDataFromCfgFile();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTabSecurityCfgService::PreTranslateMessage(MSG* pMsg) 
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



void CTabSecurityCfgService::OnButtonDetect() 
{
	DetectAclsProblems();
	m_btnFix.EnableWindow( TRUE );
	m_btnRecover.EnableWindow( TRUE );
}

void CTabSecurityCfgService::OnButtonFix() 
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

void CTabSecurityCfgService::OnButtonRecover() 
{
	if ( ! m_bDoingRecover )
	{
		if ( ! m_bRecoverInProgress )
		{
			CDlgRecoverReg * pcDlgRecoverReg;

			memset( m_szRecoverRegCmdLine, 0, sizeof(m_szRecoverRegCmdLine) );
			pcDlgRecoverReg = new CDlgRecoverReg();
			pcDlgRecoverReg->m_dwType = CDLGRECOVERREG_TYPE_SERVICE;
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





void CTabSecurityCfgService::OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTabSecurityCfgService::OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL bHasSel = FALSE;
	POSITION p_ListPos = m_listGroup.GetFirstSelectedItemPosition();
	INT nItemSel;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
	BOOL bSvcRunning;
	INT nIndex;

	//	...
	bSvcRunning	= FALSE;

	if ( p_ListPos )
	{
		nItemSel = m_listGroup.GetNextSelectedItem(p_ListPos);
		if ( nItemSel >= 0 && nItemSel < m_listGroup.GetItemCount() )
		{
			bHasSel = TRUE;

			nIndex = m_listGroup.GetItemData( nItemSel );
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				if ( pstAclsDataGroup )
				{
					pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
					if ( pstAclsDataSet )
					{
						if ( SERVICE_RUNNING == delib_drv_get_status( pstAclsDataSet->szPath ) )
						{
							bSvcRunning = TRUE;
						}
					}
				}
			}
		}
	}
	
	NMITEMACTIVATE *pNotifier=(NMITEMACTIVATE*)pNMHDR;
	CPoint pt( pNotifier->ptAction.x,pNotifier->ptAction.y );
	ClientToScreen( &pt );
	CMenu menu;
	menu.LoadMenu( IDR_MENU_SVC_LIST );
	if ( FALSE == bHasSel )
	{
		menu.GetSubMenu(0)->EnableMenuItem( IDM_MENU_LIST_DETAIL, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED );
	}
	if ( bSvcRunning )
	{
		menu.GetSubMenu(0)->EnableMenuItem( IDM_MENU_LIST_SVC_START, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED );
	}
	else
	{
		menu.GetSubMenu(0)->EnableMenuItem( IDM_MENU_LIST_SVC_STOP, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED );
	}
	menu.GetSubMenu(0)->TrackPopupMenu( TPM_RIGHTBUTTON, pt.x+2, pt.y+2, this );
	menu.Detach();
	
	*pResult = 0;
}


void CTabSecurityCfgService::OnMenuListSvcStart() 
{
	CDlgService dlgService;
	POSITION p_ListPos = m_listGroup.GetFirstSelectedItemPosition();
	INT nItemSel;
	INT nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
	TCHAR szSvcStatus [ 64 ];

	if ( p_ListPos )
	{
		nItemSel = m_listGroup.GetNextSelectedItem( p_ListPos );
		if ( nItemSel >= 0 && nItemSel < m_listGroup.GetItemCount() )
		{
			nIndex = m_listGroup.GetItemData( nItemSel );
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				if ( pstAclsDataGroup )
				{
					pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
					if ( pstAclsDataSet )
					{
						dlgService.m_lpcszSvcName	= pstAclsDataSet->szPath;
						dlgService.m_bSvcStart		= TRUE;		//	启动服务
						dlgService.DoModal();

						//	...
						memset( szSvcStatus, 0, sizeof(szSvcStatus) );
						m_cTabSecurityCfgBase.GetServiceStatusText( pstAclsDataSet->szPath, szSvcStatus, sizeof(szSvcStatus) );
						m_listGroup.SetItemText( nItemSel, 1, szSvcStatus );
					}
				}
			}
		}
	}
}

void CTabSecurityCfgService::OnMenuListSvcStop() 
{
	CDlgService dlgService;
	POSITION p_ListPos = m_listGroup.GetFirstSelectedItemPosition();
	INT nItemSel;
	INT nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
	TCHAR szSvcStatus [ 64 ];
	
	if ( p_ListPos )
	{
		nItemSel = m_listGroup.GetNextSelectedItem( p_ListPos );
		if ( nItemSel >= 0 && nItemSel < m_listGroup.GetItemCount() )
		{
			nIndex = m_listGroup.GetItemData( nItemSel );
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				if ( pstAclsDataGroup )
				{
					pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
					if ( pstAclsDataSet )
					{
						dlgService.m_lpcszSvcName	= pstAclsDataSet->szPath;
						dlgService.m_bSvcStart		= FALSE;	//	启动服务
						dlgService.DoModal();

						//	...
						memset( szSvcStatus, 0, sizeof(szSvcStatus) );
						m_cTabSecurityCfgBase.GetServiceStatusText( pstAclsDataSet->szPath, szSvcStatus, sizeof(szSvcStatus) );
						m_listGroup.SetItemText( nItemSel, 1, szSvcStatus );
					}
				}
			}
		}
	}
}

LRESULT CTabSecurityCfgService::OnDoNext( WPARAM wParam, LPARAM lParam )
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
					//	开启修复线程
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

VOID CTabSecurityCfgService::LoadDataFromCfgFile()
{
	//	...
	m_CfgServiceData.LoadData( m_cVwFirewallConfigFile.m_szCfgServiceData );

	m_proBar.SetRange( 0, m_CfgServiceData.m_vcAclsGroupList.size() );
	m_proBar.SetPos( 0 );
}

VOID CTabSecurityCfgService::InitList()
{
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
//	TCHAR szPermissionsDesc[ MAX_PATH ];
//	TCHAR szTemp[ 1024 ];
	TCHAR szSvcStatus[ MAX_PATH ];
	TCHAR szSvcDesc[ MAX_PATH ];
	UINT uItemNew;
	UINT uCount;
//	LONG lnSvcStatus;

	if ( m_listGroup.GetItemCount() > 0 )
	{
		return;
	}

	m_proBar.ShowWindow( SW_SHOW );

	for ( m_CfgServiceData.m_it = m_CfgServiceData.m_vcAclsGroupList.begin();
		m_CfgServiceData.m_vcAclsGroupList.end() != m_CfgServiceData.m_it;
		m_CfgServiceData.m_it ++ )
	{
		pstAclsDataGroup = m_CfgServiceData.m_it;
		if ( pstAclsDataGroup )
		{
			if ( pstAclsDataGroup->vcAclsSetList.size() )
			{
				pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
				if ( pstAclsDataSet )
				{
					uCount	 = m_listGroup.GetItemCount();
					uItemNew = m_listGroup.InsertItem( uCount, pstAclsDataSet->szPath );
					m_listGroup.SetCheck( uItemNew, FALSE );
					m_listGroup.SetItemData( uItemNew, pstAclsDataGroup->nIndex );

					if ( 0 == _tcsicmp( pstAclsDataSet->szChk, "DISABLED" ) )
					{
						m_listGroup.SetItemText( uItemNew, 3, "建议禁用" );
					}
					else if ( 0 == _tcsicmp( pstAclsDataSet->szChk, "DEMAND" ) )
					{
						m_listGroup.SetItemText( uItemNew, 3, "建议手动启动" );
					}

					if ( delib_drv_is_exist( pstAclsDataSet->szPath ) )
					{
						memset( szSvcStatus, 0, sizeof(szSvcStatus) );
						memset( szSvcDesc, 0, sizeof(szSvcDesc) );
						m_cTabSecurityCfgBase.GetServiceStatusText( pstAclsDataSet->szPath, szSvcStatus, sizeof(szSvcStatus) );

						if ( DELIBDRV_ERROR_SUCC !=
							delib_drv_get_description( pstAclsDataSet->szPath, szSvcDesc, sizeof(szSvcDesc) ) )
						{
							_tcscpy( szSvcDesc, _T("") );
						}
					}
					else
					{
						_tcscpy( szSvcDesc, _T("服务未安装或被禁止") );
					}

					m_listGroup.SetItemText( uItemNew, 1, szSvcStatus );
					m_listGroup.SetItemText( uItemNew, 2, _T("待诊断") );
					m_listGroup.SetItemText( uItemNew, 4, szSvcDesc );
				}
			}
		}

		m_proBar.SetPos( uCount );
	}

	m_proBar.SetPos( m_CfgServiceData.m_vcAclsGroupList.size() );
	//	...
	m_proBar.ShowWindow( SW_HIDE );
}

VOID CTabSecurityCfgService::DetectAclsProblems()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
	INT  nCount;
	INT  nCountChecked;
	BOOL bChecked;
	TCHAR szSvcStatus[ 64 ];

	//	...
	m_proBar.ShowWindow( SW_SHOW );
	nCount		= m_listGroup.GetItemCount();
	nCountChecked	= 0;

	for ( i = 0; i < nCount; i ++ )
	{
		bChecked	= FALSE;
		nIndex		= m_listGroup.GetItemData( i );
		if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
		{
			m_CfgServiceData.m_it = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
			pstAclsDataGroup = m_CfgServiceData.m_it;
			if ( pstAclsDataGroup )
			{
				if ( pstAclsDataGroup->vcAclsSetList.size() )
				{
					pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
					if ( pstAclsDataSet )
					{
						memset( szSvcStatus, 0, sizeof(szSvcStatus) );
						m_cTabSecurityCfgBase.GetServiceStatusText( pstAclsDataSet->szPath, szSvcStatus, sizeof(szSvcStatus) );
						m_listGroup.SetItemText( i, 1, szSvcStatus );
					}
				}

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
			m_listGroup.SetItemText( i, 2, "待修复" );
		}
		else
		{
			m_listGroup.SetCheck( i, FALSE );
			m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
			m_listGroup.SetItemText( i, 2, "已修复" );
		}

		//m_listGroup.EnsureVisible( i, FALSE );
		m_proBar.SetPos( i );
	}

	m_proBar.SetPos( m_CfgServiceData.m_vcAclsGroupList.size() );

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


unsigned __stdcall CTabSecurityCfgService::_threadFixAclsProblems( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgService * pThis = (CTabSecurityCfgService*)arglist;
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


VOID CTabSecurityCfgService::FixAclsProblemsProc()
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
	BackupServicesInRegister();
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
VOID CTabSecurityCfgService::BackupServicesInRegister()
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
	if ( ! PathIsDirectory( m_cVwFirewallConfigFile.m_szBackupServiceDir ) )
	{
		CreateDirectory( m_cVwFirewallConfigFile.m_szBackupServiceDir, NULL );
	}

	nTempFileNo	= 0;
	nCount		= m_listGroup.GetItemCount();
	
	GetLocalTime( &st );
	_sntprintf( szCurrentBakFile, sizeof(szCurrentBakFile)/sizeof(TCHAR)-1, 
		_T("%sbackup_[%d-%02d-%02d_%02d.%02d.%02d].reg"), 
		m_cVwFirewallConfigFile.m_szBackupServiceDir,
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
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				m_CfgServiceData.m_it = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgServiceData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 2, "正在备份" );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, i, 0 );
					
					for ( pstAclsDataGroup->it = pstAclsDataGroup->vcAclsSetList.begin();
					pstAclsDataGroup->vcAclsSetList.end() != pstAclsDataGroup->it;
					pstAclsDataGroup->it ++
						)
					{
						pstAclsDataSet = pstAclsDataGroup->it;
						if ( pstAclsDataSet )
						{
							if ( 0 == _tcsicmp( "svc", pstAclsDataSet->szType ) )
							{
								_sntprintf( szTempFile, sizeof(szTempFile)/sizeof(TCHAR)-1, _T("%s__regbak%d.tmp"), m_cVwFirewallConfigFile.m_szBackupServiceDir, nTempFileNo );
								nTempFileNo ++;
								
								//	c:\windows\regedit.exe /e "E:\code\VC\VW\VwFirewall\VwFirewallCfg\Debug\backup\object\111.reg.tmp" "HKEY_CLASSES_ROOT\WScript.Shell"
								_sntprintf
									(
									szCmdLine, sizeof(szCmdLine)/sizeof(TCHAR)-1, "%s /e \"%s\" \"%s%s\"",
									m_cVwFirewallConfigFile.mb_szSysRegeditFile, szTempFile, 
									_T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\"),
									pstAclsDataSet->szPath
									);
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
					m_listGroup.SetItemText( i, 2, "已备份" );
				}
			}
		}
	}
	
	m_proBar.SetPos( m_CfgServiceData.m_vcAclsGroupList.size() );
}
VOID CTabSecurityCfgService::FixAclsProblems()
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
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				m_CfgServiceData.m_it = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgServiceData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 2, "正在修复" );
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
						m_listGroup.SetItemText( i, 2, "已修复" );
					}
					else
					{
						m_listGroup.SetCheck( i, TRUE );
						m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 2, 0, 0, 0 );
						m_listGroup.SetItemText( i, 2, "修复失败" );
						//break;
					}
				}
			}
		}
	}

	m_proBar.SetPos( m_CfgServiceData.m_vcAclsGroupList.size() );
}






unsigned __stdcall CTabSecurityCfgService::_threadRecoverFixing( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgService * pThis = (CTabSecurityCfgService*)arglist;
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
VOID CTabSecurityCfgService::RecoverFixingProc()
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
VOID CTabSecurityCfgService::RecoverFixing()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	INT  nCount;
	

	//
	//	将备份的注册表导入注册表
	//
	if ( _tcslen( m_szRecoverRegCmdLine ) )
	{
		delib_run_block_process_ex( m_szRecoverRegCmdLine, FALSE );
	}

	MessageBox( "恭喜，已经完成还原。必须在计算机重启后配置才生效。建议：立即重启计算机。", "还原完成", MB_ICONINFORMATION );
	return;







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
			if ( nIndex >= 0 && nIndex < m_CfgServiceData.m_vcAclsGroupList.size() )
			{
				m_CfgServiceData.m_it = & m_CfgServiceData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgServiceData.m_it;
				if ( pstAclsDataGroup )
				{
					m_listGroup.SetItemText( i, 2, "正在还原" );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, i, 0 );
					
					//
					//	Fix all sets in this group
					//
					m_cTabSecurityCfgBase.RecoverFixingInGroup( pstAclsDataGroup );

					Sleep( 1000 );
					m_cTabSecurityCfgBase.SetFlashFixingIcon( &m_listGroup, -1, -1 );
					Sleep( 200 );

					m_listGroup.SetCheck( i, FALSE );
					m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
					m_listGroup.SetItemText( i, 2, "已还原" );

					/*
					//
					//	Display the fixing result
					//
					if ( m_cTabSecurityCfgBase.IsNeedToFix( pstAclsDataGroup ) )
					{
						m_listGroup.SetCheck( i, FALSE );
						m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
						m_listGroup.SetItemText( i, 2, "还原成功" );
					}
					else
					{
						m_listGroup.SetCheck( i, TRUE );
						m_listGroup.SetItem( i, 0, LVIF_IMAGE, NULL, 2, 0, 0, 0 );
						m_listGroup.SetItemText( i, 2, "还原失败" );
						//break;
					}
					*/
				}
			}
		}
	}
	m_proBar.SetPos( m_CfgServiceData.m_vcAclsGroupList.size() );

	if ( i == nCount )
	{
		MessageBox( "恭喜，已经完成还原。某些特殊服务必须在计算机重启后才能正常工作。建议：立即重启计算机。", "还原完成", MB_ICONINFORMATION );
	}
}



