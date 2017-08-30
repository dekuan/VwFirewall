// DlgImport.cpp : implementation file
//

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "DlgImport.h"

#include "DeAdsiOpIIS.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImport dialog


CDlgImport::CDlgImport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pListParent	= NULL;
}


void CDlgImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImport)
	DDX_Control(pDX, IDC_BUTTON_IMPORT, m_btnImport);
	DDX_Control(pDX, IDC_LIST_DOMAIN, m_listDomain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImport, CDialog)
	//{{AFX_MSG_MAP(CDlgImport)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnButtonImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImport message handlers

BOOL CDlgImport::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	m_strString.Format( IDS_LISTN_DOMAIN );
	m_listDomain.InsertColumn( 0, m_strString, LVCFMT_LEFT, 280 );
	m_strString.Format( IDS_LISTN_IMPORTRESULT );
	m_listDomain.InsertColumn( 1, m_strString, LVCFMT_LEFT, 90 );
	m_listDomain.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );

	//	从 IIS 读取所有 HOST 信息到 LISTVIEW
	ReadAllIISHostToList();


	return TRUE;
}

/**
 *	从 IIS 读取所有 HOST 信息到 LISTVIEW
 */
BOOL CDlgImport::ReadAllIISHostToList()
{
	BOOL bRet	= FALSE;
	CDeAdsiOpIIS aOpiis;
	vector<STHOSTINFO> vcAllHost;
	vector<STHOSTINFO>::iterator it;
	STHOSTINFO * pstHostTemp;	
	UINT uItemNew;

	m_listDomain.DeleteAllItems();

	if ( aOpiis.GetAllHostInfo( vcAllHost ) && vcAllHost.size() )
	{
		for ( it = vcAllHost.begin(); it != vcAllHost.end(); it ++ )
		{
			pstHostTemp = it;
			
			if ( _tcslen(pstHostTemp->szHostName) &&
				0 != _tcsicmp( pstHostTemp->szHostName, _T("localhost") ) )
			{
				uItemNew = m_listDomain.InsertItem( 0, pstHostTemp->szHostName );
				m_listDomain.SetItemText( uItemNew, 1, "" );
				m_listDomain.SetCheck( uItemNew, TRUE );
			}
		}
	}

	return bRet;
}

void CDlgImport::OnButtonImport() 
{
	UINT  uItemCount;
	UINT  i;
	TCHAR szDomain[ MAX_PATH ];
	UINT  uItemNew;

	if ( NULL == m_pListParent )
	{
		ASSERT( "NULL == m_pListParent" );
		return;
	}

	uItemCount = m_listDomain.GetItemCount();

	//	..
	for ( i = 0; i < uItemCount; i ++ )
	{
		m_listDomain.GetItemText( i, 0, szDomain, sizeof(szDomain) );

		if ( -1 == GetListIndexByPanDomain( m_pListParent, szDomain ) )
		{
			uItemNew = m_pListParent->InsertItem( 0, szDomain );
			m_pListParent->SetCheck( uItemNew, TRUE );
			m_strString.Format( IDS_LISTS_ALLOW );
			m_pListParent->SetItemText( uItemNew, 1, m_strString );

			m_listDomain.SetCheck( i, TRUE );
			m_strString.Format( IDS_LISTS_IMPORTSUCC );
			m_listDomain.SetItemText( i, 1, m_strString );
		}
		else
		{
			m_listDomain.SetCheck( i, FALSE );
			m_strString.Format( IDS_LISTS_DOMAINEXIST );
			m_listDomain.SetItemText( i, 1, m_strString );
		}
	}

	//	...
	m_btnImport.EnableWindow( FALSE );
}
