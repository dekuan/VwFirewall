// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5E8EC0D0_1F4C_4AF8_83F3_0408BED88DFD__INCLUDED_)
#define AFX_STDAFX_H__5E8EC0D0_1F4C_4AF8_83F3_0408BED88DFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//	for windows firewall com
#define _WIN32_WINNT	0x0500
#define _WIN32_DCOM




#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "Windows.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


//////////////////////////////////////////////////////////////////////////
//
enum
{
	UM_SETBUTTONFACE = ( WM_USER + 0x1001 ),
	UM_SETINALERT,
	UM_VIEWLOGS,
	UM_SELECT_SETTING_TREEITEM
};

#define MUTEX_VWFIREWALLCFG_NAME		_T( "VwFirewallCfg.exe" )

//	Timer
enum
{
	VWFIREWALLCFG_TIMER_AUTOHIDDEN	= 1000,
	VWFIREWALLCFG_TIMER_KILLSEL,
	VWFIREWALLCFG_TIMER_ONLINECHECK,
	VWFIREWALLCFG_TIMER_CLOSE_DLG_BUILDDICT,
	VWFIREWALLCFG_TIMER_UPDATE_LINKTEXT
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5E8EC0D0_1F4C_4AF8_83F3_0408BED88DFD__INCLUDED_)
