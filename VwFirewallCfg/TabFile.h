#if !defined(AFX_TABFILE_H__3AB8DA39_0957_48BA_BA49_BCD10F08B039__INCLUDED_)
#define AFX_TABFILE_H__3AB8DA39_0957_48BA_BA49_BCD10F08B039__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabFile dialog
#include "TabBaseInfo.h"
#include "ProcFunc.h"

//	menu id ( 10000 ~ 10300 )
#define CTABFILE_MENU_ADDEP_STARTID	10000
#define CTABFILE_MENU_ADDEP_EXPLORER	10001
#define CTABFILE_MENU_ADDEP_SERV_U	10002
#define CTABFILE_MENU_ADDEP_OTHERS	10003


class CTabFile :
	public CDialog,
	public CTabBaseInfo,
	public CProcFunc
{
// Construction
public:
	CTabFile(CWnd* pParent = NULL);   // standard constructor

	VOID AddExceptedProcess( DWORD dwMenuID );
	VOID InsertNewExceptedProcess( LPCTSTR lpcszPath, BOOL bChecked );

	VOID EnableButtonsDir( BOOL bEnabled, BOOL bAllButtons = FALSE );
	VOID EnableButtonsEp( BOOL bEnabled, BOOL bAllButtons = FALSE );

	BOOL LoadConfig();
	BOOL SaveConfig();

private:
	CString m_strCaption;
	CString m_strString;

public:
// Dialog Data
	//{{AFX_DATA(CTabFile)
	enum { IDD = IDD_TAB_FILE };
	CListCtrl	m_listProtectedDir;
	CListCtrl	m_listExceptedProcess;
	CButton	m_btnMod_Dir;
	CButton	m_btnDel_Dir;
	CButton	m_btnAdd_Dir;
	CButton	m_btnDel_Ep;
	CButton	m_btnAdd_Ep;
	CButtonST	m_btnRestore;
	CButtonST	m_btnSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFile)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonAddDir();
	afx_msg void OnButtonDelDir();
	afx_msg void OnButtonModDir();
	afx_msg void OnItemchangedListProtecteddir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListExceptedprocess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListProtecteddir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	afx_msg void OnButtonRestore();
	afx_msg void OnButtonAddEp();
	afx_msg void OnButtonDelEp();
	afx_msg void OnButtonModEp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFILE_H__3AB8DA39_0957_48BA_BA49_BCD10F08B039__INCLUDED_)
