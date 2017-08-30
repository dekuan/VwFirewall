// TabRdp.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "TabRdp.h"
#include "DlgSetRDPPort.h"

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabRdp dialog


CTabRdp::CTabRdp(CWnd* pParent /*=NULL*/)
	: CDialog(CTabRdp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabRdp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTabRdp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabRdp)
	DDX_Control(pDX, IDC_LIST_RDP, m_listClientName);
	DDX_Control(pDX, IDC_BUTTON_MOD, m_btnMod);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestore);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabRdp, CDialog)
	//{{AFX_MSG_MAP(CTabRdp)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_MOD, OnButtonMod)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RDP, OnItemchangedListDomain)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RDP, OnDblclkListDomain)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_MODRDPPORT, OnButtonModRdpPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabRdp message handlers

BOOL CTabRdp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	m_btnSave.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_APPLY) ), RGB(0xFF,0x00,0xFF) );
	m_btnSave.SetFlat(FALSE);
	
	m_btnRestore.SetBitmaps( ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BUTTON_RESTORE) ), RGB(0xFF,0x00,0xFF) );
	m_btnRestore.SetFlat(FALSE);
	
	//
	//	计算机名
	m_strString.Format( IDS_LISTN_COMPUTERNAME );
	m_listClientName.InsertColumn( 0, m_strString, LVCFMT_LEFT, 400 );
	//	状态
	m_strString.Format( IDS_LISTN_STATUS );
	m_listClientName.InsertColumn( 1, m_strString, LVCFMT_LEFT, 150 );
	m_listClientName.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );


	//
	//	装载配置
	//
	LoadConfig();


	return TRUE;
}

BOOL CTabRdp::PreTranslateMessage(MSG* pMsg) 
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

void CTabRdp::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CTabRdp::OnButtonAdd() 
{
	CDlgClientAdd dlg;
	UINT uItemNew;

	dlg.m_bUse	= TRUE;
	if ( IDOK == dlg.DoModal() )
	{
		if ( -1 == GetListIndexByString( &m_listClientName, 0, dlg.m_szClientName ) )
		{
			uItemNew = m_listClientName.InsertItem( 0, dlg.m_szClientName );
			m_listClientName.SetCheck( uItemNew, dlg.m_bUse );
			if ( dlg.m_bUse )
			{
				m_strString.Format( IDS_LISTS_ALLOW );
				m_listClientName.SetItemText( uItemNew, 1, m_strString );
			}
			else
			{
				m_strString.Format( IDS_LISTS_DENY );
				m_listClientName.SetItemText( uItemNew, 1, m_strString );
			}
			EnableButtons( TRUE );
		}
		else
		{
			//	您输入的新计算机名已经存在。
			m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
			m_strString.Format( IDS_TABRDP_ERR_PCNEXIST1 );
			MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		}
	}	
}

void CTabRdp::OnButtonDel() 
{
	POSITION p_ListPos	= m_listClientName.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;
	DWORD ibe_id		= 0;

	if ( p_ListPos )
	{
		uItemSel = m_listClientName.GetNextSelectedItem( p_ListPos );
		m_listClientName.DeleteItem( uItemSel );

		EnableButtons( TRUE );
	}
	else
	{
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABRDP_ERR_OP1 );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
	}
}

void CTabRdp::OnButtonMod() 
{
	CDlgClientAdd dlg;
	POSITION p_ListPos	= m_listClientName.GetFirstSelectedItemPosition();
	UINT uItemSel		= 0;

	if ( p_ListPos )
	{
		uItemSel	= m_listClientName.GetNextSelectedItem( p_ListPos );
		dlg.m_bUse	= m_listClientName.GetCheck( uItemSel );
		m_listClientName.GetItemText( uItemSel, 0, dlg.m_szClientName, sizeof(dlg.m_szClientName) );

		if ( IDOK == dlg.DoModal() )
		{
			if ( -1 == GetListIndexByString( &m_listClientName, 0, dlg.m_szClientName, uItemSel ) )
			{
				m_listClientName.SetCheck( uItemSel, dlg.m_bUse );
				m_listClientName.SetItemText( uItemSel, 0, dlg.m_szClientName );
				if ( dlg.m_bUse )
				{
					m_strString.Format( IDS_LISTS_ALLOW );
					m_listClientName.SetItemText( uItemSel, 1, m_strString );
				}
				else
				{
					m_strString.Format( IDS_LISTS_DENY );
					m_listClientName.SetItemText( uItemSel, 1, m_strString );
				}	
				
				
				EnableButtons( TRUE );
			}
			else
			{
				//	您编辑的计算机名与现有其他计算机名重复。
				m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
				m_strString.Format( IDS_TABRDP_ERR_PCNEXIST2 );
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

void CTabRdp::OnItemchangedListDomain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if ( CTabBaseInfo::IsCheckboxChanged( pNMListView ) )
	{
		EnableButtons( TRUE );
	}
	
	*pResult = 0;
}

void CTabRdp::OnDblclkListDomain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonMod();
	
	*pResult = 0;
}

void CTabRdp::OnButtonSave() 
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

void CTabRdp::OnButtonRestore() 
{
	OnCancel();

	//LoadConfig();
}





VOID CTabRdp::EnableButtons( BOOL bEnabled, BOOL bAllButtons /* = FALSE */ )
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
 *	装载配置信息
 */
BOOL CTabRdp::LoadConfig()
{
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;
	UINT uItemNew;

	TCHAR szTemp[ MAX_PATH ];
	STRDPCLIENTNAME stClientName;

	//
	//	[Domain]
	//	domain="use:1;dm:.chinapig.cn;ecpext:abc,www;"
	//	domain="use:1;dm:.chinameate.cn;ecpext:abc,www;"
	//

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	nSecsCount = delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_RDPCLIENTNAME, NULL, FALSE );
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			if ( nSecsCount ==
				delib_ini_parse_section_lineex( CVwFirewallConfigFile::m_szCfgFile, CVWFIREWALLCONFIGFILE_INI_DOMAIN_RDPCLIENTNAME, pstIniSecs, FALSE ) )
			{
				m_listClientName.DeleteAllItems();
				for ( i = 0; i < nSecsCount; i ++ )
				{
					memset( & stClientName, 0, sizeof(stClientName) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM, stClientName.szClientName, sizeof(stClientName.szClientName) );

					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWFIREWALLCONFIGFILE_CFGVALNAME_USE, szTemp, sizeof(szTemp) );
					if ( 0 == strcmp( szTemp, "1" ) )
					{
						stClientName.bUse = TRUE;
					}
					else
					{
						stClientName.bUse = FALSE;
					}

					//
					//	插入到 ListView
					//
					if ( CProcFunc::IsValidComputerName(stClientName.szClientName) )
					{
						uItemNew = m_listClientName.InsertItem( m_listClientName.GetItemCount(), stClientName.szClientName );
						m_listClientName.SetCheck( uItemNew, stClientName.bUse );
						if ( stClientName.bUse )
						{
							m_strString.Format( IDS_LISTS_ALLOW );
							m_listClientName.SetItemText( uItemNew, 1, m_strString );
						}
						else
						{
							m_strString.Format( IDS_LISTS_DENY );
							m_listClientName.SetItemText( uItemNew, 1, m_strString );
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
BOOL CTabRdp::SaveConfig()
{
	BOOL bRet	= FALSE;
	UINT uItemCount;
	UINT uUsedItemCount;
	UINT i;
	CString strLine;
	TCHAR szBuffer[ 65535 ];
	STRDPCLIENTNAME stDmItem;


	//	清理列表先
	memset( szBuffer, 0, sizeof(szBuffer) );

	uItemCount = m_listClientName.GetItemCount();
	if ( uItemCount > 100 )
	{
		//	请不要添加多于 100 个计算机名。
		m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
		m_strString.Format( IDS_TABRDP_ERR_TOOMUCH_DOMAINS );
		MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
		return FALSE;
	}

	if ( uItemCount > 0 )
	{
		uUsedItemCount	= 0;
	
		//
		//	循环获取所有域名
		//
		for ( i = 0; i < uItemCount; i ++ )
		{
			memset( & stDmItem, 0, sizeof(stDmItem) );

			stDmItem.bUse	= m_listClientName.GetCheck( i );
			m_listClientName.GetItemText( i, 0, stDmItem.szClientName, sizeof(stDmItem.szClientName) );
			StrTrim( stDmItem.szClientName, "\r\n\t " );
			if ( _tcslen( stDmItem.szClientName ) )
			{
				if ( stDmItem.bUse )
				{
					uUsedItemCount ++;
				}
	
				//
				//	[RdpClientName]
				//	rdpclientname="use:1;item:mxing;"
				//
				strLine	= "";
				strLine += CVWFIREWALLCONFIGFILE_INI_KEY_RDP_CLIENTNAME;
				strLine += "=\"";
				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_USE;
				strLine += ( stDmItem.bUse ? "1" : "0" );
				strLine += ";";

				strLine += CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM;
				strLine += stDmItem.szClientName;
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
		}

		if ( 0 == uUsedItemCount )
		{
			//	目前列表内没有任何允许连接的计算机名，为了安全，微盾防火墙核心将自动暂停“远程桌面安全”功能。
			m_strCaption.Format( IDS_MSG_CAPTION_WARNING );
			m_strString.Format( IDS_TABRDP_ERR_EMPTYHINT );
			MessageBox( m_strString, m_strCaption, MB_ICONWARNING );
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
		bRet = WritePrivateProfileSection( CVWFIREWALLCONFIGFILE_INI_DOMAIN_RDPCLIENTNAME, szBuffer, CVwFirewallConfigFile::m_szCfgFile );
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


void CTabRdp::OnButtonModRdpPort() 
{
	CDlgSetRDPPort dlg;

	if ( IDOK == dlg.DoModal() )
	{
		EnableButtons( TRUE );
	}	
}
