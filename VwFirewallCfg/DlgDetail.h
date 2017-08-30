#if !defined(AFX_DLGDETAIL_H__322188FE_039E_4358_B83F_0A751E2DD251__INCLUDED_)
#define AFX_DLGDETAIL_H__322188FE_039E_4358_B83F_0A751E2DD251__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDetail.h : header file
//

#include "ParseCfgData.h"
#include "VwFirewallConfigFile.h"




/////////////////////////////////////////////////////////////////////////////
// CDlgDetail dialog

class CDlgDetail : public CDialog
{
// Construction
public:
	CDlgDetail(CWnd* pParent = NULL);   // standard constructor
	~CDlgDetail();

	VOID InitList();
	BOOL GetRemarkInfo( STPARSECFGDATASET * pstAclsDataSet, LPTSTR lpszInfo, DWORD dwSize );

	vector<STPARSECFGDATASET> * m_pvcAclsSetList;
	vector<STPARSECFGDATASET>::iterator m_it;

	HICON m_hIconAlert;
	CVwFirewallConfigFile m_cVwFirewallConfigFile;

// Dialog Data
	//{{AFX_DATA(CDlgDetail)
	enum { IDD = IDD_DLG_DETAIL };
	CListCtrl	m_listDetail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDetail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDetail)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDETAIL_H__322188FE_039E_4358_B83F_0A751E2DD251__INCLUDED_)
