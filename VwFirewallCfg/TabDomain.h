#if !defined(AFX_TABDOMAIN_H__2C742892_1ECE_421E_8221_10E1B4207BC6__INCLUDED_)
#define AFX_TABDOMAIN_H__2C742892_1ECE_421E_8221_10E1B4207BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabDomain.h : header file
//

#include "TabBaseInfo.h"
#include "DlgDmAdd.h"
#include "DlgImport.h"
#include "ProcFunc.h"


/////////////////////////////////////////////////////////////////////////////
// CTabDomain dialog

class CTabDomain :
	public CDialog,
	public CTabBaseInfo,
	public CProcFunc
{
// Construction
public:
	CTabDomain(CWnd* pParent = NULL);   // standard constructor

	BOOL LoadConfig();
	BOOL SaveConfig();

	VOID EnableButtons( BOOL bEnabled, BOOL bAllButtons = FALSE );
	BOOL SaveAllIISHostToConfig();


	CString m_strCaption;
	CString m_strString;

	BOOL m_bRegedTemp;


// Dialog Data
	//{{AFX_DATA(CTabDomain)
	enum { IDD = IDD_TAB_DOMAIN };
	CButton	m_btnMod;
	CListCtrl	m_listDomain;
	CButton	m_btnDel;
	CButton	m_btnAdd;
	CButtonST	m_btnRestore;
	CButtonST	m_btnSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabDomain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabDomain)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonMod();
	afx_msg void OnDblclkListDomain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	afx_msg void OnButtonRestore();
	afx_msg void OnItemchangedListDomain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonImp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABDOMAIN_H__2C742892_1ECE_421E_8221_10E1B4207BC6__INCLUDED_)
