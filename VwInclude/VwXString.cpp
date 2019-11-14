// XString.cpp  Version 1.1 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.1 - 2007 August 7
//     - Added _tcszdup()
//     - Added _tcszndup()
//     - Added _tcsccnt()
//
//     Version 1.0 - 2007 June 25
//     - Initial public release
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
// Public APIs:
//        NAME                            DESCRIPTION
//   ----------------    ------------------------------------------------------
//   find
//      _tcsistr()       Find a substring in a string (case insensitive)
//      _tcsccnt()       Count number of occurrences of a character in a string
//   removal
//      _tcscrem()       Remove character in a string (case sensitive)
//      _tcsicrem()      Remove character in a string (case insensitive)
//      _tcsstrrem()     Remove substring in a string (case sensitive)
//      _tcsistrrem()    Remove substring in a string (case insensitive)
//   replace
//      _tcscrep()       Replace character in a string (case sensitive)
//      _tcsicrep()      Replace character in a string (case insensitive)
//      _tcsistrrep()    Replace one substring in a string with another 
//                       substring (case insensitive)
//   trim
//      _tcsltrim()      Removes (trims) leading whitespace characters from a string
//      _tcsrtrim()      Removes (trims) trailing whitespace characters from a string
//      _tcstrim()       Removes (trims) leading and trailing whitespace characters 
//                       from a string
//    copy
//      _tcszdup()       Allocates buffer with new, fills it with zeros, copies
//                       string
//      _tcsnzdup()      Allocates buffer with new, fills it with zeros, copies
//                       count characters from string to buffer
//
///////////////////////////////////////////////////////////////////////////////


// NOTE ABOUT PRECOMPILED HEADERS:
// This file does not need to be compiled with precompiled headers (.pch).
// To disable this, go to Project | Settings | C/C++ | Precompiled Headers
// and select "Not using precompiled headers".  Be sure to do this for all
// build configurations.
//#include "stdafx.h"

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "VwXString.h"
//#include "XTrace.h"

#if 0  // -----------------------------------------------------------
#ifdef _DEBUG
void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif // -----------------------------------------------------------

#pragma warning(disable : 4127)	// conditional expression is constant (_ASSERTE)
#pragma warning(disable : 4996)	// disable bogus deprecation warning


///////////////////////////////////////////////////////////////////////////////
//
// _tcscrep()
//
// Purpose:     Replace character in a string (case sensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the character replacements
//              chOld   - character to look for
//              chNew   - character to replace it with
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcscrep(TCHAR *str, TCHAR chOld, TCHAR chNew)
{
	if (!str)
		return str;

	TCHAR *str1 = str;
	while (*str1)
	{
		if (*str1 == chOld)
			*str1 = chNew;
		str1++;
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsicrep()
//
// Purpose:     Replace character in a string (case insensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the character replacements
//              chOld   - character to look for
//              chNew   - character to replace it with
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcsicrep(TCHAR *str, TCHAR chOld, TCHAR chNew)
{
	if (!str)
		return str;

	size_t nLen = _tcslen(str);
	TCHAR *newstr = new TCHAR [nLen+1];
	ZeroMemory(newstr, nLen+1);
	_tcscpy(newstr, str);
	_tcslwr(newstr);

	TCHAR oldstr[2] = { chOld, _T('\0') };
	_tcslwr(oldstr);

	TCHAR *cp = newstr;
	while (*cp)
	{
		if (*cp == oldstr[0])
			str[cp-newstr] = chNew;
		cp++;
	}
	delete [] newstr;

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsicrem()
//
// Purpose:     Remove character in a string (case insensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the character removals
//              ch      - character to remove
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcsicrem(TCHAR * str, TCHAR ch)
{
	if (!str)
		return str;

	size_t nLen = _tcslen(str);
	TCHAR *newstr = new TCHAR [nLen+1];
	ZeroMemory(newstr, nLen+1);
	_tcscpy(newstr, str);
	_tcslwr(newstr);

	TCHAR chstr[2] = { ch, _T('\0') };
	_tcslwr(chstr);

	TCHAR *cp1 = str;
	TCHAR *cp2 = newstr;

	while (*cp2)
	{
		if (*cp2 != chstr[0])
			*cp1++ = str[cp2-newstr];
		cp2++;
	}
	*cp1 = _T('\0');

	delete [] newstr;

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcscrem()
//
// Purpose:     Remove character in a string (case sensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the character removals
//              ch      - character to remove
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcscrem(TCHAR * str, TCHAR ch)
{
	if (!str)
		return str;

	TCHAR *cp1 = str;
	TCHAR *cp2 = str;

	while (*cp2)
	{
		if (*cp2 != ch)
			*cp1++ = *cp2;
		cp2++;
	}
	*cp1 = _T('\0');

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsistrrem()
//
// Purpose:     Remove substring in a string (case insensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the substring removals
//              substr  - substring to remove
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcsistrrem(TCHAR * str, const TCHAR *substr)
{
	if (!str)
		return str;
	if (!substr)
		return str;

	TCHAR *target = NULL;
	size_t nSubstrLen = _tcslen(substr);
	TCHAR *cp = str;
	while ((target = _tcsistr(cp, substr)) != NULL)
	{
		_tcscpy(target, target + nSubstrLen);
		cp = target;
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsstrrem()
//
// Purpose:     Remove substring in a string (case sensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the substring removals
//              substr  - substring to remove
//
// Returns:     TCHAR * - Pointer to the updated string. Because the 
//                        modification is done in place, the pointer  
//                        returned is the same as the pointer passed 
//                        as the input argument. 
//
TCHAR * _tcsstrrem(TCHAR * str, const TCHAR *substr)
{
	if (!str)
		return str;
	if (!substr)
		return str;

	TCHAR *target = NULL;
	size_t nSubstrLen = _tcslen(substr);
	TCHAR *cp = str;
	while ((target = _tcsstr(cp, substr)) != NULL)
	{
		_tcscpy(target, target + nSubstrLen);
		cp = target;
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsistr()
//
// Purpose:     Find a substring in a string (case insensitive)
//
// Parameters:  str     - pointer to string; upon return, str will be updated 
//                        with the character removals
//              substr  - substring to find
//
// Returns:     TCHAR * - Pointer to the first occurrence of substr in str, 
//                        or NULL if substr does not appear in string.  If 
//                        substr points to a string of zero length, the 
//                        function returns str.
//
TCHAR * _tcsistr( const TCHAR * str, const TCHAR * substr )
{
	if ( ! str || ! substr || ( substr[0] == _T('\0') ) )
	{	
		return (TCHAR *) str;
	}	

	UINT nLenSb = _tcslen(substr);
	while ( *str )
	{
		if ( 0 == _tcsnicmp( str, substr, nLenSb ) )
		{	
			break;
		}	
		str++;
	}

	if ( *str == _T('\0') )
	{	
		str = NULL;
	}	

	return (TCHAR *) str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsistrrep()
//
// Purpose:     Replace one substring in a string with another 
//              substring (case insensitive)
//
// Parameters:  lpszStr    - Pointer to string; upon return, lpszStr will be 
//                           updated with the character removals
//              lpszOld    - Pointer to substring that is to be replaced
//              lpszNew    - Pointer to substring that will replace lpszOld
//              lpszResult - Pointer to buffer that receives result string.  
//                           This may be NULL, in which case the required size
//                           of the result buffer will be returned. (Call
//                           _tcsistrrep once to get size, then allocate 
//                           buffer, and call _tcsistrrep again.)
//
// Returns:     int        - Size of result string.  If lpszResult is NULL,
//                           the size of the buffer (in TCHARs) required to 
//                           hold the result string is returned.  Does not 
//                           include terminating nul character.  Returns 0
//                           if no replacements.
//
int _tcsistrrep(const TCHAR * lpszStr, 
				const TCHAR * lpszOld, 
				const TCHAR * lpszNew, 
				TCHAR * lpszResult)
{
	if (!lpszStr || !lpszOld || !lpszNew)
		return 0;

	size_t nStrLen = _tcslen(lpszStr);
	if (nStrLen == 0)
		return 0;

	size_t nOldLen = _tcslen(lpszOld);
	if (nOldLen == 0)
		return 0;

	size_t nNewLen = _tcslen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	TCHAR *pszStart = (TCHAR *) lpszStr;
	TCHAR *pszEnd = (TCHAR *) lpszStr + nStrLen;
	TCHAR *pszTarget = NULL;
	TCHAR * pszResultStr = NULL;

	while (pszStart < pszEnd)
	{
		while ((pszTarget = _tcsistr(pszStart, lpszOld)) != NULL)
		{
			nCount++;
			pszStart = pszTarget + nOldLen;
		}
		pszStart += _tcslen(pszStart);
	}

	// if any changes, make them now
	if (nCount > 0)
	{
		// allocate buffer for result string
		size_t nResultStrSize = nStrLen + (nNewLen - nOldLen) * nCount + 2;
		pszResultStr = new TCHAR [nResultStrSize];
		ZeroMemory(pszResultStr, nResultStrSize*sizeof(TCHAR));

		pszStart = (TCHAR *) lpszStr;
		pszEnd = (TCHAR *) lpszStr + nStrLen;
		TCHAR *cp = pszResultStr;

		// loop again to actually do the work
		while (pszStart < pszEnd)
		{
			while ((pszTarget = _tcsistr(pszStart, lpszOld)) != NULL)
			{
				int nCopyLen = (int)(pszTarget - pszStart);
				_tcsncpy(cp, &lpszStr[pszStart-lpszStr], nCopyLen);

				cp += nCopyLen;

				pszStart = pszTarget + nOldLen;

				_tcscpy(cp, lpszNew);

				cp += nNewLen;
			}
			_tcscpy(cp, pszStart);
			pszStart += _tcslen(pszStart);
		}

		//TRACE("pszResultStr=<%s>\n", pszResultStr);

		_ASSERTE(pszResultStr[nResultStrSize-2] == _T('\0'));

		if (lpszResult && pszResultStr)
			_tcscpy(lpszResult, pszResultStr);
	}

	int nSize = 0;
	if (pszResultStr)
	{
		nSize = (int)_tcslen(pszResultStr);
		delete [] pszResultStr;
	}

	//TRACE("_tcsistrrep returning %d\n", nSize);

	return nSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcstrim()
//
// Purpose:     Removes (trims) leading and trailing whitespace characters 
//              from a string
//
// Parameters:  str     - Pointer to the null-terminated string to be trimmed. 
//                        On return, str will hold the trimmed string.
//              targets - Pointer to string containing whitespace characters.
//                        If this is NULL, a default set of whitespace
//                        characters is used.
//
// Returns:     TCHAR * - Pointer to trimmed string
//
TCHAR *_tcstrim(TCHAR *str, const TCHAR *targets)
{
	return _tcsltrim(_tcsrtrim(str, targets), targets);
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsrtrim()
//
// Purpose:     Removes (trims) trailing whitespace characters from a string
//
// Parameters:  str     - Pointer to the null-terminated string to be trimmed. 
//                        On return, str will hold the trimmed string.
//              targets - Pointer to string containing whitespace characters.
//                        If this is NULL, a default set of whitespace
//                        characters is used.
//
// Returns:     TCHAR * - Pointer to trimmed string
//
TCHAR *_tcsrtrim(TCHAR *str, const TCHAR *targets)
{
	TCHAR *end;
	
	if (!str)
		return NULL;

	if (!targets)
		targets = _T(" \t\n\r");
		
	end = str + _tcslen(str);

	while (end-- > str)
	{
		if (!_tcschr(targets, *end))
			return str;
		*end = 0;
	}
	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsltrim()
//
// Purpose:     Removes (trims) leading whitespace characters from a string
//
// Parameters:  str     - Pointer to the null-terminated string to be trimmed. 
//                        On return, str will hold the trimmed string.
//              targets - Pointer to string containing whitespace characters.
//                        If this is NULL, a default set of whitespace
//                        characters is used.
//
// Returns:     TCHAR * - Pointer to trimmed string
//
TCHAR *_tcsltrim(TCHAR *str, const TCHAR *targets)
{
	if (!str)
		return NULL;
	
	if (!targets)
		targets = _T(" \t\r\n");
	
	while (*str)
	{
		if (!_tcschr(targets, *str))
			return str;
		++str;
	}
	return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcszdup()
//
// Purpose:     Allocates buffer with new, fills it with zeros, copies string
//              to buffer
//
// Parameters:  str     - Pointer to the null-terminated string to be copied. 
//
// Returns:     TCHAR * - Pointer to duplicated string (allocated with new)
//
TCHAR *_tcszdup(const TCHAR *str)
{
	if (!str)
		return NULL;

	size_t len = _tcslen(str);
	TCHAR *cp = new TCHAR [len + 16];
	memset(cp, 0, (len+16)*sizeof(TCHAR));
	_tcsncpy(cp, str, len);

	return cp;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsnzdup()
//
// Purpose:     Allocates buffer with new, fills it with zeros, copies count
//              characters from string to buffer
//
// Parameters:  str     - Pointer to the null-terminated string to be copied
//              count   - Number of characters to copy
//
// Returns:     TCHAR * - Pointer to duplicated string (allocated with new)
//
TCHAR *_tcsnzdup(const TCHAR *str, size_t count)
{
	if (!str)
		return NULL;

	TCHAR *cp = new TCHAR [count + 16];
	memset(cp, 0, (count+16)*sizeof(TCHAR));
	_tcsncpy(cp, str, count);

	return cp;
}

///////////////////////////////////////////////////////////////////////////////
//
// _tcsccnt()
//
// Purpose:     Count number of occurrences of a character in a string
//
// Parameters:  str - pointer to string
//              ch  - character to look for
//
// Returns:     int - Number of times ch is found in str
//
int _tcsccnt(const TCHAR *str, TCHAR ch)
{
	if (!str)
		return 0;

	int count = 0;

	while (*str)
	{
		if (*str++ == ch)
			count++;
	}

	return count;
}
