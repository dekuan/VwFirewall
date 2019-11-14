//
// sysdef.h
//
#ifndef __sysdef_h__
#define __sysdef_h__



#define PUBLIC
#define PRIVATE static

#ifndef _WIN32
typedef int INT;
typedef unsigned long ULONG;
#define IN
#define OUT
typedef char CHAR;
typedef WORD* LPWSTR;
typedef const WORD* LPCWSTR;
#endif //_WIN32

// undocumented message
#ifndef _WIN32
#define WM_ENTERMENULOOP 0x0211
#define WM_EXITMENULOOP 0212
#endif //_WIN32

typedef CHAR NORMAL_TEXT[256];

typedef enum _SYSTEMVER {
	svWin3x,
	svWin9x,
	svWinNT,
	svWin2000
} SYSTEM_VER;

typedef enum _CONVCODE_TYPE {
	cctAnsi,
	cctUnicode
} CONVCODE_TYPE;

// Debug Information Type
typedef enum _DBGINFO_TYPE {
	ditRunInfo, // normal run result
	ditErrInfo, // system occur error, report
	ditOther // other type
} DBGINFO_TYPE;




#endif //__sysdef_h__









