// VwFirewallCfg.h : main header file for the VwFirewallCfg application
//

#if !defined(AFX_VwFirewallCfg_H__26539E20_87F2_4CB1_9D8B_80DC031550CC__INCLUDED_)
#define AFX_VwFirewallCfg_H__26539E20_87F2_4CB1_9D8B_80DC031550CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "VwFirewallConfigFile.h"
#include "VwFirewallCfgDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVwFirewallCfgApp:
// See VwFirewallCfg.cpp for the implementation of this class
//

class CVwFirewallCfgApp :
	public CWinApp,
	public CVwFirewallConfigFile
{
public:
	CVwFirewallCfgApp();

	BOOL SendWinodwCloseMessage();
	VOID SendWinodwShowMessage();
	BOOL IsCfgRunning();
	BOOL SendInstallRequest( BOOL bInstall );

	CVwFirewallCfgDlg * m_pMainDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVwFirewallCfgApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVwFirewallCfgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VwFirewallCfg_H__26539E20_87F2_4CB1_9D8B_80DC031550CC__INCLUDED_)
