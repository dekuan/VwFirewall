#if !defined(AFX_TABRDP_H__61F874E6_1E9D_4284_92C9_BF47E00545C9__INCLUDED_)
#define AFX_TABRDP_H__61F874E6_1E9D_4284_92C9_BF47E00545C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabRdp.h : header file
//
#include "TabBaseInfo.h"
#include "ProcFunc.h"
#include "DlgClientAdd.h"


/////////////////////////////////////////////////////////////////////////////
// CTabRdp dialog

class CTabRdp :
	public CDialog,
	public CTabBaseInfo,
	public CProcFunc
{
// Construction
public:
	CTabRdp(CWnd* pParent = NULL);   // standard constructor

	BOOL LoadConfig();
	BOOL SaveConfig();

	VOID EnableButtons( BOOL bEnabled, BOOL bAllButtons = FALSE );

private:
	CString m_strCaption;
	CString m_strString;

public:
	
// Dialog Data
	//{{AFX_DATA(CTabRdp)
	enum { IDD = IDD_TAB_RDP };
	CListCtrl	m_listClientName;
	CButton	m_btnMod;
	CButton	m_btnDel;
	CButton	m_btnAdd;
	CButtonST	m_btnRestore;
	CButtonST	m_btnSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabRdp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabRdp)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonMod();
	afx_msg void OnItemchangedListDomain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListDomain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	afx_msg void OnButtonRestore();
	afx_msg void OnButtonModRdpPort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABRDP_H__61F874E6_1E9D_4284_92C9_BF47E00545C9__INCLUDED_)
