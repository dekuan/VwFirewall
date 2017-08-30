#if !defined(AFX_DLGDMADD_H__582DABC0_AF36_4709_8D7B_87DC94D3CA94__INCLUDED_)
#define AFX_DLGDMADD_H__582DABC0_AF36_4709_8D7B_87DC94D3CA94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDmAdd.h : header file
//

#include "ProcFunc.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgDmAdd dialog

class CDlgDmAdd :
	public CDialog,
	public CProcFunc
{
// Construction
public:
	CDlgDmAdd(CWnd* pParent = NULL);   // standard constructor


	BOOL  m_bUse;
	TCHAR m_szPanDomain[ MAX_PATH ];
	TCHAR m_szToDomain[ MAX_PATH ];

	CHyperLink m_LinkHowToGetSubDomain;

// Dialog Data
	//{{AFX_DATA(CDlgDmAdd)
	enum { IDD = IDD_DLG_DMADD };
	CButton	m_chkUse;
	CEdit	m_editPanDomain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDmAdd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDmAdd)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual LRESULT OnOpenHtmlHelp( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDMADD_H__582DABC0_AF36_4709_8D7B_87DC94D3CA94__INCLUDED_)
