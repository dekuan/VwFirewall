// TabDomain.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabDomain.h"
#include "commctrl.h"
#include "atlbase.h"
#include "DeAdsiOpIIS.h"
#include "ProcSecurityDesc.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 *	加密字符串
 */
//	"警告"
static TCHAR g_szCTabDomain_Limit_Alert[] = { 65, 80, 71, 25, 0 };

//	"“普通个人版”最多能设置 50 个域名。需要解析更多泛域名请升级到更高版本。"
static TCHAR g_szCTabDomain_Limit_Psl[] =
{
	94, 79, 57, 42, 50, 87, 71, 9, 55, 52, 79, 25, 94, 78, 40, 17, 73, 31, 59, 35, 54,
	23, 41, 60, -33, -54, -49, -33, 71, 9, 44, 13, 60, 4, 94, 92, 47, 23, 45, 85, 66,
	29, 49, 9, 71, 3, 73, 31, 72, 69, 44, 13, 60, 4, 56, 20, 54, 2, 67, 73, 74,
	66, 71, 3, 71, 32, 79, 25, 78, 65, 94, 92, 0
};

//	"“高级个人版”和“终身个人版”最多能设置 100 个域名。需要解析更多泛域名请升级到更高版本。"
static TCHAR g_szCTabDomain_Limit_PslAdv[] =
{
	94, 79, 71, 32, 67, 73, 71, 9, 55, 52, 79, 25, 94, 78, 69, 50, 94, 79, 41, 42, 54,
	18, 71, 9, 55, 52, 79, 25, 94, 78, 40, 17, 73, 31, 59, 35, 54, 23, 41, 60, -33,
	-50, -49, -49, -33, 71, 9, 44, 13, 60, 4, 94, 92, 47, 23, 45, 85, 66, 29, 49, 9,
	71, 3, 73, 31, 72, 69, 44, 13, 60, 4, 56, 20, 54, 2, 67, 73, 74, 66, 71, 3,
	71, 32, 79, 25, 78, 65, 94, 92, 0
};

//	"“普通公司版”最多能设置 500 个域名。需要解析更多泛域名请升级到更高版本。"
static TCHAR g_szCTabDomain_Limit_ComNormal[] =
{
	94, 79, 57, 42, 50, 87, 70, 84, 52, 65, 79, 25, 94, 78, 40, 17, 73, 31, 59, 35, 54,
	23, 41, 60, -33, -54, -49, -49, -33, 71, 9, 44, 13, 60, 4, 94, 92, 47, 23, 45, 85,
	66, 29, 49, 9, 71, 3, 73, 31, 72, 69, 44, 13, 60, 4, 56, 20, 54, 2, 67, 73,
	74, 66, 71, 3, 71, 32, 79, 25, 78, 65, 94, 92, 0
};

//	"免费评估版本只能设置 2 个域名。需要解析更多泛域名请购买正式版本。"
static TCHAR g_szCTabDomain_Limit_evaluate[] =
{
	60, 29, 72, 46, 57, 63, 70, 63, 79, 25, 78, 65, 41, 68, 59, 35, 54, 23, 41, 60, -33,
	-51, -33, 71, 9, 44, 13, 60, 4, 94, 92, 47, 23, 45, 85, 66, 29, 49, 9, 71, 3,
	73, 31, 72, 69, 44, 13, 60, 4, 56, 20, 70, 69, 61, 13, 42, 2, 53, 66, 79, 25,
	78, 65, 94, 92, 0
};






/////////////////////////////////////////////////////////////////////////////
// CTabDomain dialog


CTabDomain::CTabDomain(CWnd* pParent /*=NULL*/)
	: CDialog(CTabDomain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabDomain)
	//}}AFX_DATA_INIT

	//
	//	是否正式版本
	//
	m_bRegedTemp = CTabBaseInfo::m_bReged;
}


void CTabDomain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabDomain)
	DDX_Control(pDX, IDC_BUTTON_MOD, m_btnMod);
	DDX_Control(pDX, IDC_LIST_DOMAIN, m_listDomain);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestore);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabDomain, CDialog)
	//{{AFX_MSG_MAP(CTabDomain)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_MOD, OnButtonMod)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DOMAIN, OnDblclkListDomain)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DOMAIN, OnItemchangedListDomain)
	ON_BN_CLICKED(IDC_BUTTON_IMP, OnButtonImp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabDomain message handlers

BOOL CTabDomain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//
	m_btnSave.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_APPLY) ), RGB(0xFF,0x00,0xFF) );
	m_btnSave.SetFlat(FALSE);
	
	m_btnRestore.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_RESTORE) ), RGB(0xFF,0x00,0xFF) );
	m_btnRestore.SetFlat(FALSE);
	
	//
	m_strString.Format( IDS_LISTN_DOMAIN );
	m_listDomain.InsertColumn( 0, m_strString, LVCFMT_LEFT, 400 );
	m_strString.Format( IDS_LISTN_STATUS );
	m_listDomain.InsertColumn( 1, m_strString, LVCFMT_LEFT, 150 );
	m_listDomain.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );

	//
	//	装载配置
	//
	LoadConfig();

	
	return TRUE;
}

BOOL CTabDomain::PreTranslateMessage(MSG* pMsg) 
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

void CTabDomain::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CTabDomain::OnButtonSave() 
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
		EnableButtons( FALSE );
	}
}

void CTabDomain::OnButtonRestore() 
{
	OnCancel();

	//LoadConfig();
}




/**
 *	装载配置信息
 */
BOOL CTabDomain::LoadConfig()
{
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;
	UINT uItemNew;

	TCHAR szTemp[ MAX_PATH ];
	STVWFIREWALLDOMAIN stDomain;

	//
	//	[Domain]
	//	domain="use:1;dm:.chinapig.cn;ecpext:abc,www;"
	//	domain="use:1;dm:.chinameate.cn;ecpext:abc,www;"
	//

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	nSecsCount = delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_DOMAIN, NULL, FALSE );
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			if ( nSecsCount ==
				delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_DOMAIN, pstIniSecs, FALSE ) )
			{
				m_listDomain.DeleteAllItems();
				for ( i = 0; i < nSecsCount; i ++ )
				{
					memset( & stDomain, 0, sizeof(stDomain) );
	
					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_WCD, szTemp, sizeof(szTemp) );
					if ( 0 == strcmp( szTemp, "1" ) )
					{
						stDomain.bWildcard = TRUE;

						stDomain.szDomain[0] = '*';
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_DM, stDomain.szDomain+1, sizeof(stDomain.szDomain)-1 );
					}
					else
					{
						stDomain.bWildcard = FALSE;
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_DM, stDomain.szDomain, sizeof(stDomain.szDomain) );
					}

					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_USE, szTemp, sizeof(szTemp) );
					if ( 0 == strcmp( szTemp, "1" ) )
					{
						stDomain.bUse = TRUE;
					}
					else
					{
						stDomain.bUse = FALSE;
					}

					//
					//	插入到 ListView
					//
					if ( CProcFunc::IsValidPanDomain(stDomain.szDomain) || CProcFunc::IsValidDomain(stDomain.szDomain) )
					{
						uItemNew = m_listDomain.InsertItem( m_listDomain.GetItemCount(), stDomain.szDomain );
						m_listDomain.SetCheck( uItemNew, stDomain.bUse );
						if ( stDomain.bUse )
						{
							m_strString.Format( IDS_LISTS_ALLOW );
							m_listDomain.SetItemText( uItemNew, 1, m_strString );
						}
						else
						{
							m_strString.Format( IDS_LISTS_DENY );
							m_listDomain.SetItemText( uItemNew, 1, m_strString );
						}
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
BOOL CTabDomain::SaveConfig()
{
	BOOL bRet	= FALSE;
	UINT uItemCount;
	UINT i;
	CString strLine;
	TCHAR szBuffer[ 65535 ];
	CSimpleArray<STVWFIREWALLDOMAIN> aDomainList;
	STVWFIREWALLDOMAIN stDmItem;



	//	清理列表先
	aDomainList.RemoveAll();
	memset( szBuffer, 0, sizeof(szBuffer) );

	uItemCount = m_listDomain.GetItemCount();
	if ( uItemCount > 100 )
	{
		//	请不要添加多于 100 个域名。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABDOMAIN_ERR_TOOMUCH_DOMAINS );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		return FALSE;
	}


	if ( uItemCount > 0 )
	{
		//
		//	循环获取所有域名
		//
		for ( i = 0; i < uItemCount; i ++ )
		{
			memset( & stDmItem, 0, sizeof(stDmItem) );

			stDmItem.bUse	= m_listDomain.GetCheck( i );
			m_listDomain.GetItemText( i, 0, stDmItem.szDomain, sizeof(stDmItem.szDomain) );
			if ( 0 == _tcsnicmp( stDmItem.szDomain, _T("*"), 1 ) )
			{
				stDmItem.bWildcard = TRUE;
			}
			else
			{
				stDmItem.bWildcard = FALSE;
			}
			StrTrim( stDmItem.szDomain, "* " );

			aDomainList.Add( stDmItem );
		}

		//
		//	排序，以便驱动部分更快速查找
		//
		qsort
		(
			aDomainList.GetData(), 
			aDomainList.GetSize(), 
			sizeof(STVWFIREWALLDOMAIN),
			_qsortCmpFunc_tagVwDmFirewallDomain
		);
	}

	for ( i = 0; i < aDomainList.GetSize(); i ++ )
	{
		//
		//	[Domain]
		//	domain="use:1;dm:.chinapig.cn;ecpext:abc,www;"
		//	domain="use:1;dm:.chinameate.cn;ecpext:abc,www;"
		//

		strLine	= "";
		strLine += CVWFIREWALLCONFIGFILE_INI_KEY_DOMAIN;
		strLine += "=\"";
		strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_USE;
		strLine += ( aDomainList[i].bUse ? "1" : "0" );
		strLine += ";";

		strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_DM;
		strLine += aDomainList[i].szDomain;
		strLine += ";";

		strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_WCD;
		strLine += ( aDomainList[i].bWildcard ? "1" : "0" );
		strLine += ";";

		strLine += "\"\r\n";

		if ( _tcslen(szBuffer) + strLine.GetLength() + 2 < sizeof(szBuffer) )
		{
			_tcscat( szBuffer, strLine.GetBuffer(0) );
			strLine.ReleaseBuffer();
			//szBuffer[ _tcslen(szBuffer) + 1 ] = NULL;
		}
		else
		{
			break;
		}
	}

	if ( _tcslen(szBuffer) + 2 < sizeof(szBuffer) )
	{
		//	AAA\r\nN
		//	0123 4 5 
		//	BBB\r\nN
		//	CCC\r\nNN
		if ( _tcslen(szBuffer) > 2 )
		{
			//	去掉最后的 \r\n
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
		}

		delib_drv_wow64_enablewow64_fs_redirection( FALSE );
		bRet = WritePrivateProfileSection( CVWFIREWALLCONFIGFILE_INI_DOMAIN_DOMAIN, szBuffer, CVwFirewallConfigFile::m_szCfgFile );
		delib_drv_wow64_enablewow64_fs_redirection( TRUE );

		//	恭喜，配置信息保存成功！
		m_strCaption.Format( IDS_MSG_CAPTION_CONGLT );
		m_strString.Format( IDS_TABDOMAIN_ERR_SAVESUCC );
		MessageBox( m_strString, m_strCaption, MB_ICONINFORMATION );
	}
	else
	{
		//	内部错误，数据过于庞大，无法保存！
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABDOMAIN_ERR_BIGDATA );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}

	return bRet;
}



void CTabDomain::OnButtonAdd() 
{
	CDlgDmAdd dlg;
	UINT uItemNew;

	dlg.m_bUse	= TRUE;
	if ( IDOK == dlg.DoModal() )
	{
		if ( -1 == GetListIndexByPanDomain( &m_listDomain, dlg.m_szPanDomain ) )
		{
			uItemNew = m_listDomain.InsertItem( 0, dlg.m_szPanDomain );
			m_listDomain.SetCheck( uItemNew, dlg.m_bUse );

			if ( dlg.m_bUse )
			{
				m_strString.Format( IDS_LISTS_ALLOW );
				m_listDomain.SetItemText( uItemNew, 1, m_strString );
			}
			else
			{
				m_strString.Format( IDS_LISTS_DENY );
				m_listDomain.SetItemText( uItemNew, 1, m_strString );
			}	

			EnableButtons( TRUE );
		}
		else
		{
			//	您输入的新域名已经存在。
			m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
			m_strString.Format( IDS_TABDOMAIN_ERR_DMEXIST1 );
			MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		}
	}
}

void CTabDomain::OnButtonDel() 
{
	POSITION p_ListPos	= m_listDomain.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;
	DWORD ibe_id		= 0;

	if ( p_ListPos )
	{
		uItemSel = m_listDomain.GetNextSelectedItem( p_ListPos );
		m_listDomain.DeleteItem( uItemSel );

		EnableButtons( TRUE );
	}
	else
	{
		//	请先选择一条记录。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABDOMAIN_ERR_OP1 );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}
}

void CTabDomain::OnButtonMod() 
{
	CDlgDmAdd dlg;
	POSITION p_ListPos	= m_listDomain.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;

	if ( p_ListPos )
	{
		uItemSel = m_listDomain.GetNextSelectedItem( p_ListPos );
		dlg.m_bUse	= m_listDomain.GetCheck( uItemSel );
		m_listDomain.GetItemText( uItemSel, 0, dlg.m_szPanDomain, sizeof(dlg.m_szPanDomain) );

		if ( IDOK == dlg.DoModal() )
		{
			if ( -1 == GetListIndexByPanDomain( &m_listDomain, dlg.m_szPanDomain, uItemSel ) )
			{
				m_listDomain.SetCheck( uItemSel, dlg.m_bUse );
				m_listDomain.SetItemText( uItemSel, 0, dlg.m_szPanDomain );
				if ( dlg.m_bUse )
				{
					m_strString.Format( IDS_LISTS_ALLOW );
					m_listDomain.SetItemText( uItemSel, 1, m_strString );
				}
				else
				{
					m_strString.Format( IDS_LISTS_DENY );
					m_listDomain.SetItemText( uItemSel, 1, m_strString );
				}
				EnableButtons( TRUE );
			}
			else
			{
				//	您编辑的域名与现有其他域名重复。
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( IDS_TABDOMAIN_ERR_DMEXIST2 );
				MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
			}
		}
	}
	else
	{
		//	请先选择一条记录。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABDOMAIN_ERR_OP1 );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}
}

void CTabDomain::OnDblclkListDomain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonMod();

	*pResult = 0;
}
void CTabDomain::OnItemchangedListDomain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if ( CTabBaseInfo::IsCheckboxChanged( pNMListView ) )
	{
		EnableButtons( TRUE );
	}

	*pResult = 0;
}


void CTabDomain::OnButtonImp() 
{
	CDlgImport dlg;

	dlg.m_pListParent = & m_listDomain;
	if ( IDOK == dlg.DoModal() )
	{
	}
}



//////////////////////////////////////////////////////////////////////////
//	Private


VOID CTabDomain::EnableButtons( BOOL bEnabled, BOOL bAllButtons /* = FALSE */ )
{
	m_btnSave.EnableWindow( bEnabled );
	//m_btnRestore.EnableWindow( bEnabled );

	if ( bAllButtons )
	{
		m_btnAdd.EnableWindow( bEnabled );
		m_btnDel.EnableWindow( bEnabled );
		m_btnMod.EnableWindow( bEnabled );
	}
}

/**
 *	将所有 HOST 保存到配置文件
 */
BOOL CTabDomain::SaveAllIISHostToConfig()
{
	BOOL bRet	= FALSE;
	CDeAdsiOpIIS aOpiis;
	vector<STHOSTINFO> vcAllHost;
	vector<STHOSTINFO>::iterator it;
	STHOSTINFO * pstHostTemp;
	CString strLine;
	TCHAR szBuffer[ 65535 ];


	memset( szBuffer, 0, sizeof(szBuffer) );
	
	if ( aOpiis.GetAllHostInfo( vcAllHost ) && vcAllHost.size() )
	{
		for ( it = vcAllHost.begin(); it != vcAllHost.end(); it ++ )
		{
			pstHostTemp = it;
			
			if ( _tcslen(pstHostTemp->szHostName) )
			{
				strLine	= "";
				strLine += CVWFIREWALLCONFIGFILE_INI_KEY_SYSHOST;
				strLine += "=\"";
				strLine += pstHostTemp->szHostName;
				strLine += "\"\r\n";

				if ( _tcslen(szBuffer) + strLine.GetLength() + 2 < sizeof(szBuffer) )
				{
					_tcscat( szBuffer, strLine.GetBuffer(0) );
					strLine.ReleaseBuffer();
					//szBuffer[ _tcslen(szBuffer) + 1 ] = NULL;
				}
				else
				{
					break;
				}
			}
		}
	}

	if ( _tcslen(szBuffer) + 2 < sizeof(szBuffer) )
	{
		//	AAA\r\nN
		//	0123 4 5 
		//	BBB\r\nN
		//	CCC\r\nNN
		if ( _tcslen(szBuffer) > 2 )
		{
			//	去掉最后的 \r\n
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
		}
		bRet = WritePrivateProfileSection( CVWFIREWALLCONFIGFILE_INI_DOMAIN_SYSHOST, szBuffer, CVwFirewallConfigFile::m_szCfgFile );
	}
	
	return bRet;
}




