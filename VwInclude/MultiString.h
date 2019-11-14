// MultiString.h: interface for the CMultiString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISTRING_H__EA089E82_D102_11D2_8F26_0000B48A73B5__INCLUDED_)
#define AFX_MULTISTRING_H__EA089E82_D102_11D2_8F26_0000B48A73B5__INCLUDED_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

class CMultiString  
{
public:
	static char* StringListToMultiString(char** ppStrList, int nStrNum, /*OUT*/ int* pnLen);
	static char** MultiStringToStringList(char* pBuf, int nBufLen, /*OUT*/ int* pnNum);
	static WCHAR ** MultiStringToStringListW( WCHAR * pBuf, INT nBufLen, /*OUT*/ INT * pnNum);
	static BOOL MultiStringToStringList(char* pBuf, int nBufLen, char** ppStrList, /*IN OUT*/ int* pnNum);
	CMultiString();
	virtual ~CMultiString();

private:
	int m_nBufLen;
	char* m_pBuf;
};

#endif // !defined(AFX_MULTISTRING_H__EA089E82_D102_11D2_8F26_0000B48A73B5__INCLUDED_)
