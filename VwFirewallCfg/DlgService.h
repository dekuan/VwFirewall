#if !defined(AFX_DLGSERVICE_H__4DEFABE7_CF81_4C5A_993D_56AD3CE96087__INCLUDED_)
#define AFX_DLGSERVICE_H__4DEFABE7_CF81_4C5A_993D_56AD3CE96087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgService.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgService dialog

class CDlgService : public CDialog
{
// Construction
public:
	CDlgService(CWnd* pParent = NULL);   // standard constructor
	~CDlgService();

	BOOL MakeAlphaDlg( HWND hWnd, INT nAlphaPercent );


	LPCTSTR m_lpcszSvcName;
	BOOL    m_bSvcStart;

private:
	static unsigned __stdcall _threadOpService( PVOID arglist );
	VOID OpServiceProc();
	CDeThreadSleep m_cThSleepOpService;

public:
// Dialog Data
	//{{AFX_DATA(CDlgService)
	enum { IDD = IDD_DLG_SERVICE };
	CStatic	m_stcHint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgService)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgService)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSERVICE_H__4DEFABE7_CF81_4C5A_993D_56AD3CE96087__INCLUDED_)
