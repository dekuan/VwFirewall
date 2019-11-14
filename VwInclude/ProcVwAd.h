// ProcVwAd.h: interface for the CProcVwAd class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCVWAD_HEADER__
#define __PROCVWAD_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
using namespace std;


/**
 *	config struct
 */
typedef struct tagProcVwAdItem
{
	TCHAR szTxt[ MAX_PATH ];
	TCHAR szUrl[ MAX_PATH ];

}STPROCVWADITEM, *LPSTPROCVWADITEM;

typedef struct tagProcVwAdConfig
{
	DWORD dwDownloadTimeout;

}STPROCVWADCONFIG, *LPSTPROCVWADCONFIG;

typedef struct tagProcVwAdInfo
{
	DWORD dwSingleSpan;
	DWORD dwLoopSpan;

}STPROCVWADINFO, *LPSTPROCVWADINFO;

/**
 *	class of CProcVwAd
 */
class CProcVwAd
{
public:
	CProcVwAd();
	virtual ~CProcVwAd();

public:
	VOID SetDownloadTimeout( DWORD dwTimeout );
	BOOL QueryAdInfo( LPCTSTR lpcszAdName, LPCTSTR lpcszLanguage, UINT uRedirectUrlId, vector<STPROCVWADITEM> * pvcAdList, STPROCVWADINFO * pstAdInfo = NULL );
	BOOL QueryAdInfo( LPCTSTR lpcszAdName, LPCTSTR lpcszLanguage, LPCTSTR lpcszUrl, vector<STPROCVWADITEM> * pvcAdList, STPROCVWADINFO * pstAdInfo = NULL );

private:
	VOID InitData();


private:
	STPROCVWADCONFIG m_stCfg;
};

#endif // __PROCVWAD_HEADER__
