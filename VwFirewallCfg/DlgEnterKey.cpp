// DlgEnterKey.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgEnterKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEnterKey dialog


CDlgEnterKey::CDlgEnterKey(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEnterKey::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEnterKey)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgEnterKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEnterKey)
	DDX_Control(pDX, IDC_EDIT_REGKEY, m_editRegKey);
	DDX_Control(pDX, IDC_COMBO_REGIP, m_combRegIp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEnterKey, CDialog)
	//{{AFX_MSG_MAP(CDlgEnterKey)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEnterKey message handlers

BOOL CDlgEnterKey::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	STVWFIREWALLCONFIG stConfig;

	memset( &stConfig, 0, sizeof(stConfig) );
	if ( CVwFirewallConfigFile::LoadConfig( &stConfig ) )
	{
		InitIpAddrComb( stConfig.szRegIp );
		m_editRegKey.SetWindowText( stConfig.szRegKey );
	}


	return TRUE;
}

void CDlgEnterKey::OnButtonCopy() 
{
	TCHAR szTemp[ MAX_PATH ]	= {0};

	m_combRegIp.GetLBText( m_combRegIp.GetCurSel(), szTemp );


	//	拷贝到剪切板
	delib_copy_text_to_clipboard( szTemp, NULL );

	//	选中文字
	m_combRegIp.SetFocus();
//	::SendMessage( m_editRegKey.m_hWnd, EM_SETSEL, 0, -1 );
}

void CDlgEnterKey::OnButtonSave() 
{
	BOOL  bSaveOk			= FALSE;
	TCHAR szRegHost[ MAX_PATH ]	= {0};
	TCHAR szIpAddr[ MAX_PATH ]	= {0};
	TCHAR szKey [ 64 ]		= {0};
	CString strCap;
	CString strMessage;

	// ..
	m_combRegIp.GetLBText( m_combRegIp.GetCurSel(), szIpAddr );
	m_editRegKey.GetWindowText( szKey, sizeof(szKey) );

	StrTrim( szKey, "\t\r\n " );

	// ..
	if ( CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGIP, szIpAddr ) )
	{
		if ( CVwFirewallConfigFile::SaveConfig( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGKEY, szKey ) )
		{
			bSaveOk = TRUE;
		}
	}

	if ( bSaveOk )
	{
		OnOK();
	}
	else
	{
		//	Can not save data, please make sure Access database file writable!
		strCap.Format( IDS_MSG_CAPTION_WARNING );
		strMessage.Format( IDS_DLGENTERKEY_ERR_UPDATE );
		MessageBox( strMessage, strCap, MB_ICONWARNING );
	}
}




VOID CDlgEnterKey::InitIpAddrComb( LPCTSTR lpcszIpAddr )
{
	INT nCount;
	INT i;
	INT nSelIndex;
	STDEIPADDR * pstIpAddr;
	
	nCount = delib_get_all_locipaddr( NULL );
	if ( nCount > 0 )
	{
		pstIpAddr = new STDEIPADDR[ nCount ];
		if ( pstIpAddr )
		{
			memset( pstIpAddr, 0, sizeof(STDEIPADDR)*nCount );
			if ( delib_get_all_locipaddr( pstIpAddr ) )
			{
				nSelIndex = 0;
				for ( i = 0; i < nCount; i ++ )
				{
					m_combRegIp.AddString( pstIpAddr[ i ].szIpAddr );
					if ( lpcszIpAddr && _tcslen(lpcszIpAddr) > 0 && 
						0 == _tcsicmp( pstIpAddr[ i ].szIpAddr, lpcszIpAddr ) )
					{
						nSelIndex = i;
					}
				}
				m_combRegIp.SetCurSel( nSelIndex );
			}
		}
	}
}
