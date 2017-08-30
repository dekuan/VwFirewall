#if !defined(AFX_TABLICENSE_H__50455981_3814_47FA_94F2_1A10671EAFC7__INCLUDED_)
#define AFX_TABLICENSE_H__50455981_3814_47FA_94F2_1A10671EAFC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabLicense.h : header file
//

#include "TabBaseInfo.h"
#include "VwConstCommon.h"
#include "VwEnCodeString.h"
#include "DeHttp.h"
#include "VwOnlineLicenseCheck.h"

#include "DlgEnterKey.h"


/////////////////////////////////////////////////////////////////////////////
// CTabLicense dialog

class CTabLicense :
	public CDialog,
	public CTabBaseInfo,
	public CDeHttp,
	public CVwOnlineLicenseCheck
{
// Construction
public:
	CTabLicense(CWnd* pParent = NULL);   // standard constructor

	BOOL LoadConfig();
	VOID SetLicenseContent( BOOL bInit, BOOL bEnterpriseVersion, LPCTSTR lpcszPrType, LPCTSTR lpcszCDate, LONG lnExpireType = -1, TCHAR * lpszExpireDate = NULL );
	VOID SetRichEditSpecBold( CRichEditCtrl * pRichedit, int nStart, int nEnd );
	VOID SetRichEditSpecColor( CRichEditCtrl * pRichedit, int nStart, int nEnd, DWORD dwFontStyle = 0, COLORREF crTextColor = RGB(0xFF,0x00,0x00) );
	void OnlineCheckNow();
	VOID RenewalNotice();

	STVWFIREWALLCONFIG m_stConfig;

	//TCHAR m_szRegIp[ 64 ];
	//TCHAR m_szRegHost[ 128 ];
	//TCHAR m_szRegKey[ 64 ];
	//TCHAR m_szRegPrType[ 32 ];
	//DWORD m_dwRegExpireType;
	//TCHAR m_szRegCreateDate[ 32 ];
	//TCHAR m_szRegExpireDate[ 32 ];

	TCHAR m_szMsgCaption[ MAX_PATH ];
	TCHAR m_szString[ MAX_PATH ];


	CXingBalloon * m_pxballoon;
	CHyperLink m_LinkBuy;
	


// Dialog Data
	//{{AFX_DATA(CTabLicense)
	enum { IDD = IDD_TAB_LICENSE };
	CButtonST	m_btnOnlineCheck;
	CButtonST	m_btnEnterKey;
	CRichEditCtrl	m_richeditInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabLicense)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabLicense)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonEnterKey();
	afx_msg void OnButtonOnlineCheck();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLICENSE_H__50455981_3814_47FA_94F2_1A10671EAFC7__INCLUDED_)
