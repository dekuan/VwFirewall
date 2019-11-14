// GetWindowTitle.h: interface for the CGetWindowTitle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GETWINDOWTITLE_HEADER__
#define __GETWINDOWTITLE_HEADER__



//////////////////////////////////////////////////////////////////////////
typedef struct tagSTPARAMS {
	HANDLE hEvent;
	DWORD dwPID;
	TCHAR * pszWinTitle;
	DWORD dwSize;
	DWORD dwTimeout;
}STPARAMS;



//////////////////////////////////////////////////////////////////////////
BOOL WINAPI GetWindowTitle( DWORD dwPID, TCHAR * pszWinTitle, DWORD dwSize, DWORD dwTimeout );
BOOL CALLBACK CallBackGetWindowTitle( HWND hwnd, LPARAM lParam );







#endif // __GETWINDOWTITLE_HEADER__
