// VwIISAppPoolChecker.h: interface for the CVwIISAppPoolChecker class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWIISAPPPOOLCHECKER_HEADER__
#define __VWIISAPPPOOLCHECKER_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;



typedef enum tagVwIISAppPoolCheckerWindowsSysType
{
	VWIISAPPPOOLCHECKER_WINDOWS_32S,
	VWIISAPPPOOLCHECKER_WINDOWS_NT3,
	VWIISAPPPOOLCHECKER_WINDOWS_95,
	VWIISAPPPOOLCHECKER_WINDOWS_98,
	VWIISAPPPOOLCHECKER_WINDOWS_ME,
	VWIISAPPPOOLCHECKER_WINDOWS_NT4,
	VWIISAPPPOOLCHECKER_WINDOWS_2000,
	VWIISAPPPOOLCHECKER_WINDOWS_XP,
	VWIISAPPPOOLCHECKER_WINDOWS_2003,
	VWIISAPPPOOLCHECKER_WINDOWS_2008,	//	Windows Server 2008
	VWIISAPPPOOLCHECKER_WINDOWS_VISTA,	//	Windows Vista
	VWIISAPPPOOLCHECKER_WINDOWS_2008_R2,	//	Windows Server 2008 R2
	VWIISAPPPOOLCHECKER_WINDOWS_7,		//	Windows Server 7

} VWIISAPPPOOLCHECKERWINDOWSSYSTYPE;

typedef struct tagVwIISAppPoolCheckerProcessInfo
{
	tagVwIISAppPoolCheckerProcessInfo()
	{
		memset( this, 0, sizeof(tagVwIISAppPoolCheckerProcessInfo) );
	}
	DWORD dwProcID;
	TCHAR szProcName[ MAX_PATH ];
	TCHAR szProcPath[ MAX_PATH ];
	DWORD dwMemoryUsed;

} STVWIISAPPPOOLCHECKERPROCESSINFO;


/**
 *	class of CVwIISAppPoolChecker
 */
class CVwIISAppPoolChecker
{
public:
	CVwIISAppPoolChecker();
	virtual ~CVwIISAppPoolChecker();

	BOOL IsIISAppPoolRunning();
	BOOL EnumProcess( vector<STVWIISAPPPOOLCHECKERPROCESSINFO> & vcProcessList, TCHAR * pszError );

private:
	BOOL UpPrivilege( TCHAR * pszPrivilege, BOOL bEnable );
	VWIISAPPPOOLCHECKERWINDOWSSYSTYPE GetShellSystemType();

};

#endif // __VWIISAPPPOOLCHECKER_HEADER__


