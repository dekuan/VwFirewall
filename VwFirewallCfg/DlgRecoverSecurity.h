#if !defined(AFX_DLGRECOVERSECURITY_H__BBC13831_4A14_41D4_A9B4_0CCB0215F9C7__INCLUDED_)
#define AFX_DLGRECOVERSECURITY_H__BBC13831_4A14_41D4_A9B4_0CCB0215F9C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRecoverSecurity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRecoverSecurity dialog

class CDlgRecoverSecurity : public CDialog
{
// Construction
public:
	CDlgRecoverSecurity(CWnd* pParent = NULL);   // standard constructor
	~CDlgRecoverSecurity();
	
	BOOL MakeAlphaDlg( HWND hWnd, INT nAlphaPercent );
	
private:
	static unsigned __stdcall _threadRecoverSecurity( PVOID arglist );
	VOID RecoverSecurityProc();
	CDeThreadSleep m_cThSleepRecoverSecurity;

	CVwFirewallConfigFile m_cVwFirewallConfigFile;

public:
// Dialog Data
	//{{AFX_DATA(CDlgRecoverSecurity)
	enum { IDD = IDD_DLG_RECOVER_SECURITY };
	CStatic	m_stcHint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRecoverSecurity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRecoverSecurity)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRECOVERSECURITY_H__BBC13831_4A14_41D4_A9B4_0CCB0215F9C7__INCLUDED_)
