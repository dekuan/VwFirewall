// DlgDetail.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDetail dialog


CDlgDetail::CDlgDetail(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDetail::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDetail)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pvcAclsSetList = NULL;

	//	...
	m_hIconAlert = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_ALERT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED );
}
CDlgDetail::~CDlgDetail()
{
	if ( m_hIconAlert )
	{
		DestroyIcon( m_hIconAlert );
	}
}


void CDlgDetail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDetail)
	DDX_Control(pDX, IDC_LIST_DETAIL, m_listDetail);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDetail, CDialog)
	//{{AFX_MSG_MAP(CDlgDetail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDetail message handlers

BOOL CDlgDetail::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SHFILEINFO stSfi;
	ListView_SetImageList( m_listDetail.m_hWnd, SHGetFileInfo("",0,&stSfi,sizeof(stSfi),SHGFI_SYSICONINDEX|SHGFI_SMALLICON), LVSIL_SMALL );
	m_listDetail.InsertColumn( 0, "文件/文件夹/注册表键", LVCFMT_LEFT, 280 );
	m_listDetail.InsertColumn( 1, "安全建议", LVCFMT_LEFT, 180 );
	m_listDetail.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP );


	//	...
	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID CDlgDetail::InitList()
{
	SHFILEINFO stSfi;
	UINT uItemNew;
	STPARSECFGDATASET * pstAclsDataSet;
	TCHAR szTemp[ 1024 ];

	if ( NULL == m_pvcAclsSetList )
	{
		return;
	}

	for ( m_it = m_pvcAclsSetList->begin(); m_it != m_pvcAclsSetList->end(); m_it ++ )
	{
		pstAclsDataSet = m_it;
		if ( pstAclsDataSet )
		{
			memset( &stSfi,0, sizeof(stSfi) );

			if ( _tcslen( pstAclsDataSet->szPath ) > 0 )
			{
				if ( PathFileExists( pstAclsDataSet->szPath ) || PathIsDirectory( pstAclsDataSet->szPath ) )
				{
					SHGetFileInfo( pstAclsDataSet->szPath, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
				}
				else
				{
					stSfi.iIcon = -1;
				}
				uItemNew = m_listDetail.InsertItem( m_listDetail.GetItemCount(), pstAclsDataSet->szPath );
				m_listDetail.SetItem( uItemNew, 0, LVIF_IMAGE, NULL, stSfi.iIcon, 0, 0, 0 );
				
				memset( szTemp, 0, sizeof(szTemp) );
				GetRemarkInfo( pstAclsDataSet, szTemp, sizeof(szTemp) );
				m_listDetail.SetItemText( uItemNew, 1, szTemp );
			}
			else if ( _tcslen( pstAclsDataSet->szCmd ) > 0 )
			{
				SHGetFileInfo( m_cVwFirewallConfigFile.mb_szSysCmdFile, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
				uItemNew = m_listDetail.InsertItem( m_listDetail.GetItemCount(), pstAclsDataSet->szCmd );
				m_listDetail.SetItem( uItemNew, 0, LVIF_IMAGE, NULL, stSfi.iIcon, 0, 0, 0 );
				
				memset( szTemp, 0, sizeof(szTemp) );
				GetRemarkInfo( pstAclsDataSet, szTemp, sizeof(szTemp) );
				m_listDetail.SetItemText( uItemNew, 1, szTemp );
			}
			else if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
			{
				SHGetFileInfo( m_cVwFirewallConfigFile.mb_szSysRegeditFile, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );

				if ( _tcslen( pstAclsDataSet->szRegVName ) > 0 )
				{
					_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%s\\%s|%s"), pstAclsDataSet->szRegRoot, pstAclsDataSet->szRegPath, pstAclsDataSet->szRegVName );
				}
				else
				{
					_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%s\\%s"), pstAclsDataSet->szRegRoot, pstAclsDataSet->szRegPath );
				}
				uItemNew = m_listDetail.InsertItem( m_listDetail.GetItemCount(), szTemp );
				m_listDetail.SetItem( uItemNew, 0, LVIF_IMAGE, NULL, stSfi.iIcon, 0, 0, 0 );

				memset( szTemp, 0, sizeof(szTemp) );
				GetRemarkInfo( pstAclsDataSet, szTemp, sizeof(szTemp) );
				m_listDetail.SetItemText( uItemNew, 1, szTemp );
			}
		}
	}
}

BOOL CDlgDetail::GetRemarkInfo( STPARSECFGDATASET * pstAclsDataSet, LPTSTR lpszInfo, DWORD dwSize )
{
	BOOL bRet;

	if ( NULL == pstAclsDataSet )
	{
		return FALSE;
	}
	if ( NULL == lpszInfo || 0 == dwSize )
	{
		return FALSE;
	}
	if ( NULL == m_pvcAclsSetList )
	{
		return FALSE;
	}

	bRet = FALSE;

	if ( _tcslen( pstAclsDataSet->szRemark ) > 0 )
	{
		bRet = TRUE;
		_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("%s"), pstAclsDataSet->szRemark );
	}
	else if ( 0 == _tcsicmp( "acls", pstAclsDataSet->szType ) )
	{
		if ( 0 == _tcsnicmp( "/R", pstAclsDataSet->szParam, 2 ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除%s用户权限"), pstAclsDataSet->szUser );
		}
		else if ( 0 == _tcsnicmp( "/D", pstAclsDataSet->szParam, 2 ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("拒绝%s用户权限"), pstAclsDataSet->szUser );
		}
		else if ( 0 == _tcsnicmp( "/G", pstAclsDataSet->szParam, 2 ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("添加%s用户权限"), pstAclsDataSet->szUser );
		}
	}
	else if ( 0 == _tcsicmp( "exec", pstAclsDataSet->szType ) )
	{
		if ( 0 == _tcsicmp( "PEBAKF", pstAclsDataSet->szChk ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("备份原始PE文件") );
		}
		else if ( 0 == _tcsicmp( "EDIR", pstAclsDataSet->szChk ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除目录") );
		}
		else if ( 0 == _tcsicmp( "EFILE", pstAclsDataSet->szChk ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除文件") );
		}
		else if ( _tcsstr( pstAclsDataSet->szCmd, "attrib.exe" ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("设置文件属性%s"), pstAclsDataSet->szParam );
		}
		else if ( 0 == _tcsnicmp( pstAclsDataSet->szCmd, "del", 3 ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除文件") );
		}
		else if ( 0 == _tcsnicmp( pstAclsDataSet->szCmd, "rd ", 3 ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除目录") );
		}
		else if ( _tcsstr( pstAclsDataSet->szCmd, "echo CREATE_NEW_TEMP_TEXTFILE" ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("创建临时文本文件") );
		}
	}
	else if ( 0 == _tcsicmp( "reg", pstAclsDataSet->szType ) )
	{
		if ( 0 == _tcsicmp( "DELETE", pstAclsDataSet->szChk ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("删除注册表项") );
		}
		else if ( 0 == _tcsicmp( "ADD", pstAclsDataSet->szChk ) )
		{
			bRet = TRUE;
			_sntprintf( lpszInfo, dwSize/sizeof(TCHAR)-1, _T("添加注册表项") );
		}
	}

	return bRet;
}