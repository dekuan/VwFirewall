#if !defined(AFX_DLGEXPIRE_H__B50440FE_FCF1_405E_9B3C_734E4C946421__INCLUDED_)
#define AFX_DLGEXPIRE_H__B50440FE_FCF1_405E_9B3C_734E4C946421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExpire.h : header file
//


#include "hyperlink.h"




/////////////////////////////////////////////////////////////////////////////
// CDlgExpire dialog

class CDlgExpire : public CDialog
{
// Construction
public:
	CDlgExpire(CWnd* pParent = NULL);   // standard constructor

	BOOL SetTopWindow();
	
	TCHAR m_szRegExpireDate[ MAX_PATH ];
	CHyperLink	m_LinkHowToRenew;



// Dialog Data
	//{{AFX_DATA(CDlgExpire)
	enum { IDD = IDD_DIALOG_EXPIRE };
	CStatic	m_stcInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExpire)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExpire)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPIRE_H__B50440FE_FCF1_405E_9B3C_734E4C946421__INCLUDED_)
