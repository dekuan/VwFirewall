#if !defined(AFX_TABSECURITYCFG_H__76924E3A_11E6_4991_A10A_F8933B84D8C6__INCLUDED_)
#define AFX_TABSECURITYCFG_H__76924E3A_11E6_4991_A10A_F8933B84D8C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSecurityCfg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSecurityCfg dialog

#include "TabBaseInfo.h"

#include "TabSecurityCfgBase.h"

#include "TabSecurityCfgACLSFile.h"
#include "TabSecurityCfgACLSFolder.h"
#include "TabSecurityCfgACLSAntiVirus.h"
#include "TabSecurityCfgService.h"
#include "TabSecurityCfgObject.h"




/////////////////////////////////////////////////////////////////////////////
// CTabUpdate dialog


class CTabSecurityCfg :
	public CDialog,
	public CTabBaseInfo
{
// Construction
public:
	CTabSecurityCfg(CWnd* pParent = NULL);   // standard constructor


	VOID ShowTabWindow( UINT uIndex );


	//	...
	HICON m_hIconInfo_16x16;
	HICON m_hIconAlert_16x16;
	HICON m_hIconOk_16x16;
	HICON m_hIconX_16x16;

	CXingBalloon * m_pxballoon;

	UINT m_uShowTabIndex;
	CTabSecurityCfgACLSFile m_cTabSecurityCfgACLSFile;
	CTabSecurityCfgACLSFolder m_cTabSecurityCfgACLSFolder;
	CTabSecurityCfgACLSAntiVirus m_cTabSecurityCfgACLSAntiVirus;
	CTabSecurityCfgService m_cTabSecurityCfgService;
	CTabSecurityCfgObject m_cTabSecurityCfgObject;


// Dialog Data
	//{{AFX_DATA(CTabSecurityCfg)
	enum { IDD = IDD_TAB_SECURITYCFG };
	CStatic	m_stcInfo;
	CStatic	m_stcInfoIcon;
	CButton	m_roAclsFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSecurityCfg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSecurityCfg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioAclsFile();
	afx_msg void OnRadioAclsFolder();
	afx_msg void OnRadioAclsAntivirus();
	afx_msg void OnRadioService();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRadioObject();
	virtual LRESULT OnDataChange( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSECURITYCFG_H__76924E3A_11E6_4991_A10A_F8933B84D8C6__INCLUDED_)
