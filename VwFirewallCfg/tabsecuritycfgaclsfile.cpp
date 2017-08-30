// TabSecurityCfgACLS.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabSecurityCfgACLSFile.h"

#include "Lm.h"
#include "Lmaccess.h"
#pragma comment( lib, "Netapi32.lib" )

#include "DlgDetail.h"
#include "DlgRecoverSecurity.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSFile dialog


CTabSecurityCfgACLSFile::CTabSecurityCfgACLSFile(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSecurityCfgACLSFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSecurityCfgACLSFile)
	//}}AFX_DATA_INIT

	//	...
	m_bDoingFix		= FALSE;
	m_bFixInProgress	= FALSE;

	memset( m_szInfo, 0, sizeof(m_szInfo) );
	m_dwInfoType = 0;
}
CTabSecurityCfgACLSFile::~CTabSecurityCfgACLSFile()
{
	m_cThSleepFixAclsProblems.EndSleep();
	m_cThSleepFixAclsProblems.EndThread();
}

void CTabSecurityCfgACLSFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSecurityCfgACLSFile)
	DDX_Control(pDX, IDC_BUTTON_RECOVER, m_btnRecover);
	DDX_Control(pDX, IDC_BUTTON_DETECT, m_btnDetect);
	DDX_Control(pDX, IDC_BUTTON_FIX, m_btnFix);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_proBar);
	DDX_Control(pDX, IDC_LIST_GROUP, m_listGroup);
	//}}AFX_DATA_MAP
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSecurityCfgACLSFile, CDialog)
	//{{AFX_MSG_MAP(CTabSecurityCfgACLSFile)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, OnButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON_FIX, OnButtonFix)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUP, OnDblclkListGroup)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP, OnRclickListGroup)
	ON_COMMAND(IDM_MENU_LIST_DETAIL, OnMenuListDetail)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER, OnButtonRecover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfgACLSFile message handlers

BOOL CTabSecurityCfgACLSFile::OnInitDialog() 
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
	m_listGroup.InsertColumn( 0, "名称", LVCFMT_LEFT, 130 );
	m_listGroup.InsertColumn( 1, "状态", LVCFMT_LEFT, 70 );
	m_listGroup.InsertColumn( 2, "文件", LVCFMT_LEFT, 200 );
	m_listGroup.InsertColumn( 3, "安全建议", LVCFMT_LEFT, 100 );
	m_listGroup.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );


	//
	//	...
	//
	LoadDataFromCfgFile();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CTabSecurityCfgACLSFile::PreTranslateMessage(MSG* pMsg) 
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

void CTabSecurityCfgACLSFile::OnButtonDetect() 
{
	DetectAclsProblems();
	m_btnFix.EnableWindow( TRUE );
	m_btnRecover.EnableWindow( TRUE );
}

void CTabSecurityCfgACLSFile::OnButtonFix() 
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

void CTabSecurityCfgACLSFile::OnButtonRecover() 
{
	CDlgRecoverSecurity dlg;
	
	if ( IDYES == MessageBox( _T("确认还原C盘NTFS权限为默认吗？"), _T("确认"), MB_ICONQUESTION|MB_YESNO ) )
	{
		dlg.DoModal();
		OnButtonDetect();
	}
}



void CTabSecurityCfgACLSFile::OnRclickListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CTabSecurityCfgACLSFile::OnDblclkListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnMenuListDetail();
	*pResult = 0;
}

void CTabSecurityCfgACLSFile::OnMenuListDetail() 
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
			if ( nIndex >= 0 && nIndex < m_CfgAclsFileData.m_vcAclsGroupList.size() )
			{
				pstAclsDataGroup = & m_CfgAclsFileData.m_vcAclsGroupList.at( nIndex );
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


VOID CTabSecurityCfgACLSFile::LoadDataFromCfgFile()
{
	//	...
	m_CfgAclsFileData.LoadData( m_cVwFirewallConfigFile.m_szCfgAclsFileData );

	m_proBar.SetRange( 0, m_CfgAclsFileData.m_vcAclsGroupList.size() );
	m_proBar.SetPos( 0 );
}

VOID CTabSecurityCfgACLSFile::InitList()
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

	for ( m_CfgAclsFileData.m_it = m_CfgAclsFileData.m_vcAclsGroupList.begin();
		m_CfgAclsFileData.m_vcAclsGroupList.end() != m_CfgAclsFileData.m_it;
		m_CfgAclsFileData.m_it ++ )
	{
		pstAclsDataGroup = m_CfgAclsFileData.m_it;
		if ( pstAclsDataGroup )
		{
			if ( pstAclsDataGroup->vcAclsSetList.size() )
			{
				pstAclsDataSet = pstAclsDataGroup->vcAclsSetList.begin();
				if ( pstAclsDataSet )
				{
					if ( PathFileExists( pstAclsDataSet->szPath ) )
					{
						uCount	 = m_listGroup.GetItemCount();
						uItemNew = m_listGroup.InsertItem( uCount, pstAclsDataGroup->szGroupName );
						m_listGroup.SetCheck( uItemNew, FALSE );
						m_listGroup.SetItemData( uItemNew, pstAclsDataGroup->nIndex );

						m_listGroup.SetItemText( uItemNew, 1, "待诊断" );
						m_listGroup.SetItemText( uItemNew, 2, pstAclsDataSet->szPath );
						m_listGroup.SetItemText( uItemNew, 3, "双击查看.." );

						/*
						if ( 'D' == pstAclsDataSet->szChk[ 0 ] )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("拒绝%s访问"), pstAclsDataSet->szUser );
						}
						else if ( 'E' == pstAclsDataSet->szChk[ 0 ] )
						{
							_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("删除%s访问"), pstAclsDataSet->szUser );
						}
						m_listGroup.SetItemText( uItemNew, 3, szTemp );
						*/
					}
				}
			}
		}

		m_proBar.SetPos( uCount );
	}

	m_proBar.SetPos( m_CfgAclsFileData.m_vcAclsGroupList.size() );
	//	...
	m_proBar.ShowWindow( SW_HIDE );
}

VOID CTabSecurityCfgACLSFile::DetectAclsProblems()
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
		if ( nIndex >= 0 && nIndex < m_CfgAclsFileData.m_vcAclsGroupList.size() )
		{
			m_CfgAclsFileData.m_it = & m_CfgAclsFileData.m_vcAclsGroupList.at( nIndex );
			pstAclsDataGroup = m_CfgAclsFileData.m_it;
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

	m_proBar.SetPos( m_CfgAclsFileData.m_vcAclsGroupList.size() );

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



unsigned __stdcall CTabSecurityCfgACLSFile::_threadFixAclsProblems( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CTabSecurityCfgACLSFile * pThis = (CTabSecurityCfgACLSFile*)arglist;
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
VOID CTabSecurityCfgACLSFile::FixAclsProblemsProc()
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
VOID CTabSecurityCfgACLSFile::FixAclsProblems()
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
			if ( nIndex >= 0 && nIndex < m_CfgAclsFileData.m_vcAclsGroupList.size() )
			{
				m_CfgAclsFileData.m_it = & m_CfgAclsFileData.m_vcAclsGroupList.at( nIndex );
				pstAclsDataGroup = m_CfgAclsFileData.m_it;
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
	
	m_proBar.SetPos( m_CfgAclsFileData.m_vcAclsGroupList.size() );
}

BOOL CTabSecurityCfgACLSFile::GetPermissionsDesc( ACCESS_MASK amMask, LPTSTR lpszPermissionsDesc, DWORD dwSize )
{
	if ( NULL == lpszPermissionsDesc )
		return FALSE;
	
	BOOL bRet = FALSE;
	TCHAR szTemp[ MAX_PATH ] = {0};
	
	////////////////////////////////////////////////////////////
	// Generic
	if ( GENERIC_READ == (amMask & GENERIC_READ) )
	{
		_tcscat( szTemp, "G-READ|" );
	}
	if ( GENERIC_WRITE == (amMask & GENERIC_WRITE) )
	{
		_tcscat( szTemp, "G-WRITE|" );
	}
	if ( GENERIC_EXECUTE == (amMask & GENERIC_EXECUTE) )
	{
		_tcscat( szTemp, "G-EXECUTE|" );
	}
	if ( GENERIC_ALL == (amMask & GENERIC_ALL) )
	{
		_tcscat( szTemp, "G-ALL|" );
	}
	////////////////////////////////////////////////////////////
	// Standard
	if ( MAXIMUM_ALLOWED == (amMask & MAXIMUM_ALLOWED) )
	{
		_tcscat( szTemp, "S-MAX_ALLOWED|" );
	}
	if ( STANDARD_RIGHTS_ALL == (amMask & STANDARD_RIGHTS_ALL) )
	{
		_tcscat( szTemp, "S-ALL|" );
	}
	if ( STANDARD_RIGHTS_EXECUTE == (amMask & STANDARD_RIGHTS_EXECUTE) )
	{
		_tcscat( szTemp, "S-EXECUTE|" );
	}
	if ( STANDARD_RIGHTS_READ == (amMask & STANDARD_RIGHTS_READ) )
	{
		_tcscat( szTemp, "S-READ|" );
	}
	if ( STANDARD_RIGHTS_REQUIRED == (amMask & STANDARD_RIGHTS_REQUIRED) )
	{
		_tcscat( szTemp, "S-REQUIRED|" );
	}
	if ( STANDARD_RIGHTS_WRITE == (amMask & STANDARD_RIGHTS_WRITE) )
	{
		_tcscat( szTemp, "S-WRITE|" );
	}	
	if ( SPECIFIC_RIGHTS_ALL == (amMask & SPECIFIC_RIGHTS_ALL) )
	{
		_tcscat( szTemp, "S-WRITE|" );
	}

	////////////////////////////////////////////////////////////
	// Standard - 2
	if ( DELETE == (amMask & DELETE) )
	{
		_tcscat( szTemp, "DELETE|" );
	}
	if ( READ_CONTROL == (amMask & READ_CONTROL) )
	{
		_tcscat( szTemp, "READ|" );
	}
	if ( WRITE_DAC == (amMask & WRITE_DAC) )
	{
		_tcscat( szTemp, "WRITE_DAC|" );
	}
	if ( WRITE_OWNER == (amMask & WRITE_OWNER) )
	{
		_tcscat( szTemp, "WRITE_DAC|" );
	}
	if ( SYNCHRONIZE == (amMask & SYNCHRONIZE) )
	{
		_tcscat( szTemp, "SYNCHRONIZE|" );
	}
	
	_sntprintf( lpszPermissionsDesc, dwSize-sizeof(TCHAR), "%s", szTemp );
	
	return bRet;
}



BOOL CTabSecurityCfgACLSFile::TestGroup()
{
	NET_API_STATUS nStatus;
	DWORD dwLevel = 0;	
	LPBYTE Buffer;
	DWORD entriesread;
	DWORD totalentries;
	USER_MODALS_INFO_0 * pstUserModalsInfo;
	GROUP_INFO_0 * pstGropuInfo0;
//	GROUP_INFO_1 * pstGropuInfo1;
	GROUP_USERS_INFO_0 * pstGroupUserInfo0;
	UINT i;
	UINT x;


	Buffer	= NULL;
	//
	// Call the NetUserModalsGet function; specify level 0.
	//
	nStatus = NetUserModalsGet( NULL, dwLevel, (LPBYTE *)&Buffer );
	//
	// If the call succeeds, print the global information.
	//
	if (nStatus == NERR_Success)
	{
		if ( Buffer != NULL)
		{
			pstUserModalsInfo = (USER_MODALS_INFO_0*)Buffer;

			//printf("\tMinimum password length:  %d\n", Buffer->usrmod0_min_passwd_len );
			//printf("\tMaximum password age (d): %d\n", Buffer->usrmod0_max_passwd_age / 86400);
			//printf("\tMinimum password age (d): %d\n", Buffer->usrmod0_min_passwd_age / 86400);
			//printf("\tForced log off time (s):  %d\n", Buffer->usrmod0_force_logoff );
			//printf("\tPassword history length:  %d\n", Buffer->usrmod0_password_hist_len );
		}
	}
	if ( Buffer )
	{
		NetApiBufferFree( Buffer );
		Buffer = NULL;
	}





	nStatus	= NetGroupGetInfo( NULL, L"Guests", 0, &Buffer );
	if ( NERR_Success == nStatus )
	{
		//	
		pstGropuInfo0 = (GROUP_INFO_0*)Buffer;
	}
	if ( Buffer )
	{
		NetApiBufferFree( Buffer );
		Buffer = NULL;
	}

	nStatus = NetGroupGetUsers( NULL, L"Guests", 0, &Buffer, MAX_PREFERRED_LENGTH, &entriesread, &totalentries, 0);
	if ( NERR_Success == nStatus )
	{
		pstGroupUserInfo0 = (GROUP_USERS_INFO_0 *)Buffer;
		for ( i = 0; i < entriesread; ++pstGroupUserInfo0 )
		{
			x = 0;
			//v14 = sub_100FC80(25, *pstGroupUserInfo0, 0);
			//sub_100FC41((const char *)L"%Fws", v14);
			//++i;
			//if ( !(i % 3) )
			//	sub_100FFA6();
		}		
	}
	if ( Buffer )
	{
		NetApiBufferFree( Buffer );
		Buffer = NULL;
	}

	return TRUE;
}




