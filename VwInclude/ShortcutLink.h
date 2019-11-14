// ShortcutLink.h: interface for the CShortcutLink class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SHORTCUTLINK_HEADER__
#define __SHORTCUTLINK_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 *	class of CShortcutLink
 */
class CShortcutLink
{
public:
	CShortcutLink();
	virtual ~CShortcutLink();

	BOOL Create( LPCSTR pszCmdLine, LPCSTR pszWorkingDirectory, LPCSTR pszFilePath, LPCSTR pszDesc, LPTSTR pszError );


};

#endif // !defined(AFX_SHORTCUTLINK_H__4D42D448_4DD5_410A_B16D_B935BA429A43__INCLUDED_)
