// TabSecurityCfgACLSAntiVirus.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfgACLSAntiVirus.h"

#include "DlgDetail.h"
#include "DlgRecoverSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSAntiVirus dialog


CTabSecurityCfgACLSAntiVirus::CTabSecurityCfgACLSAntiVirus(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSecurityCfgACLSAntiVirus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSecurityCfgACLSAntiVirus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDoingFix		= FALSE;
	m_bFixInProgress	= FALSE;

	memset( m_szInfo, 0, sizeof(m_szInfo) );
	m_dwInfoType = 0;
}
CTabSecurityCfgACLSAntiVirus::~CTabSecurityCfgACLSAntiVirus()
{
	m_cThSleepFixAclsProblems.EndSleep();
	m_cThSleepFixAclsProblems.EndThread();
}


void CTabSecurityCfgACLSAntiVirus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSecurityCfgACLSAntiVirus)
	DDX_Control(pDX, IDC_BUTTON_RECOVER, m_btnRecover);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_proBar);
	DDX_Control(pDX, IDC_BUTTON_FIX, m_btnFix);
	DDX_Control(pDX, IDC_BUTTON_DETECT, m_btnDetect);
	DDX_Control(pDX, IDC_LIST_GROUP, m_listGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSecurityCfgACLSAntiVirus, CDialog)
	//{{AFX_MSG_MAP(CTabSecurityCfgACLSAntiVirus)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUP, OnDblclkListGroup)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP, OnRclickListGroup)
	ON_COMMAND(IDM_MENU_LIST_DETAIL, OnMenuListDetail)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, OnButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON_FIX, OnButtonFix)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER, OnButtonRecover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSAntiVirus message handlers

BOOL CTabSecurityCfgACLSAntiVirus::OnInitDialog() 
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
BOOL CTabSecurityCfgACLSAntiVirus::PreTranslateMessage(MSG* pMsg) 
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


void CTabSecurityCfgACLSAntiVirus::OnButtonDetect() 
{
	DetectAclsProblems();
	m_btnFix.EnableWindow( TRUE );
	m_btnRecover.EnableWindow( TRUE );
}

void CTabSecurityCfgACLSAntiVirus::OnButtonFix() 
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

void CTabSecurityCfgACLSAntiVirus::OnButtonRecover() 
{
	CDlgRecoverSecurity dlg;

	if ( IDYES == MessageBox( _T("确认还原C盘NTFS权限为默认吗？"), _T("确认"), MB_ICONQUESTION|MB_YESNO ) )
	{
		dlg.DoModal();
		OnButtonDetect();
	}
}





void CTabSecurityCfgACLSAntiVirus::OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnMenuListDetail();
	*pResult = 0;
}

void CTabSecurityCfgACLSAntiVirus::OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CTabSecurityCfgACLSAntiVirus::OnMenuListDetail() 
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
			if ( nIndex >= 0 && nIndex < m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgAclsAntiVirusData.m_vcAclsGroupList.at( nIndex );
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




//////////////////////////////////////////////////////////////////////////
//	Private
//



VOID CTabSecurityCfgACLSAntiVirus::LoadDataFromCfgFile()
{
	//	...
	m_CfgAclsAntiVirusData.LoadData( m_cVwFirewallConfigFile.m_szCfgAclsAntiVirusData );

	m_proBar.SetRange( 0, m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() );
	m_proBar.SetPos( 0 );
}

VOID CTabSecurityCfgACLSAntiVirus::InitList()
{
	STPARSECFGDATAGROUP * pstAclsDataGroup;
	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
//	TCHAR szPermissionsDesc[ MAX_PATH ];
//	TCHAR szTemp[ 1024 ];
	UINT uItemNew;
	UINT uCount;

	if ( m_listGroup.GetItemCount() > 0 )
	{
		return;
	}

	m_proBar.ShowWindow( SW_SHOW );

	for ( m_CfgAclsAntiVirusData.m_it = m_CfgAclsAntiVirusData.m_vcAclsGroupList.begin();
		m_CfgAclsAntiVirusData.m_vcAclsGroupList.end() != m_CfgAclsAntiVirusData.m_it;
		m_CfgAclsAntiVirusData.m_it ++ )
	{
		pstAclsDataGroup = m_CfgAclsAntiVirusData.m_it;
		if ( pstAclsDataGroup )
		{
			if ( pstAclsDataGroup->vcAclsSetList.size() )
			{
				pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
				if ( pstAclsDataSet )
				{
					//if ( PathIsDirectory( pstAclsDataSet->szPath ) )
					{
						uCount	 = m_listGroup.GetItemCount();
						uItemNew = m_listGroup.InsertItem( uCount, pstAclsDataGroup->szGroupName );
						m_listGroup.SetCheck( uItemNew, FALSE );
						m_listGroup.SetItemData( uItemNew, pstAclsDataGroup->nIndex );

						m_listGroup.SetItemText( uItemNew, 1, "待诊断" );
						m_listGroup.SetItemText( uItemNew, 2, "双击查看.." );
						//m_listGroup.SetItemText( uItemNew, 2, pstAclsDataSet->szPath );

						/*
						if ( 0 == _tcsicmp( "D", pstAclsDataSet->szChk ) )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("拒绝%s访问"), pstAclsDataSet->szUser );
						}
						else if ( 0 == _tcsicmp( "E", pstAclsDataSet->szChk ) )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("删除%s访问"), pstAclsDataSet->szUser );
						}
						else if ( 0 == _tcsicmp( "N", pstAclsDataSet->szChk ) )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("添加%s访问"), pstAclsDataSet->szUser );
						}
						else if ( 0 == _tcsicmp( "EDIR", pstAclsDataSet->szChk ) )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("删除危险目录及文件") );
						}
						m_listGroup.SetItemText( uItemNew, 2, szTemp );
						*/
					}
				}
			}
		}

		m_proBar.SetPos( uCount );
	}

	m_proBar.SetPos( m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() );
	//	...
	m_proBar.ShowWindow( SW_HIDE );
}

VOID CTabSecurityCfgACLSAntiVirus::DetectAclsProblems()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
//	STPARSECFGDATASET * pstAclsDataSet;
//	ACCESS_MASK Mask;
	INT  nCount;
	INT  nCountChecked;
	BOOL bChecked;
	//TCHAR szPermissionsDesc[ MAX_PATH ];

	//	...
	m_proBar.ShowWindow( SW_SHOW );
	nCount		= m_listGroup.GetItemCount();
	nCountChecked	= 0;

	for ( i = 0; i < nCount; i ++ )
	{
		bChecked	= FALSE;
		nIndex		= m_listGroup.GetItemData( i );
		if ( nIndex >= 0 && nIndex < m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() )
		{
			m_CfgAclsAntiVirusData.m_it = & m_CfgAclsAntiVirusData.m_vcAclsGroupList.at( nIndex );
			pstAclsDataGroup = m_CfgAclsAntiVirusData.m_it;
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

	m_proBar.SetPos( m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() );

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


unsigned __stdcall CTabSecurityCfgACLSAntiVirus::_threadFixAclsProblems( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgACLSAntiVirus * pThis = (CTabSecurityCfgACLSAntiVirus*)arglist;
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
VOID CTabSecurityCfgACLSAntiVirus::FixAclsProblemsProc()
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
VOID CTabSecurityCfgACLSAntiVirus::FixAclsProblems()
{
	INT  i;
	INT  nIndex;
	STPARSECFGDATAGROUP * pstAclsDataGroup;
//	STPARSECFGDATASET * pstAclsDataSet;
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
			if ( nIndex >= 0 && nIndex < m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() )
			{
				m_CfgAclsAntiVirusData.m_it = & m_CfgAclsAntiVirusData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgAclsAntiVirusData.m_it;
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

	m_proBar.SetPos( m_CfgAclsAntiVirusData.m_vcAclsGroupList.size() );
}

