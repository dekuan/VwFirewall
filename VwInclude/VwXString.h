// XString.h  Version 1.1 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XSTRING_H
#define XSTRING_H

// find
TCHAR * _tcsistr(const TCHAR * str, const TCHAR * substr);
int _tcsccnt(const TCHAR *str, TCHAR ch);

// removal
TCHAR * _tcscrem(TCHAR *str, TCHAR ch);
TCHAR * _tcsicrem(TCHAR *str, TCHAR ch);
TCHAR * _tcsstrrem(TCHAR *str, const TCHAR *substr);
TCHAR * _tcsistrrem(TCHAR *str, const TCHAR *substr);

// replace
TCHAR * _tcscrep(TCHAR *str, TCHAR chOld, TCHAR chNew);
TCHAR * _tcsicrep(TCHAR *str, TCHAR chOld, TCHAR chNew);
int     _tcsistrrep(const TCHAR * lpszStr, 
					const TCHAR * lpszOld, 
					const TCHAR * lpszNew, 
					TCHAR * lpszResult);

// trim
TCHAR *_tcsltrim(TCHAR *str, const TCHAR *targets);
TCHAR *_tcsrtrim(TCHAR *str, const TCHAR *targets);
TCHAR *_tcstrim(TCHAR *str, const TCHAR *targets);

// copy
TCHAR *_tcsnzdup(const TCHAR *str, size_t count);
TCHAR *_tcszdup(const TCHAR * str);

#endif //XSTRING_H
