// AdsiOpIIS.h: interface for the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEADSIOPIIS_HEADER__
#define __DEADSIOPIIS_HEADER__


#include <vector>
using namespace std;

#include "atlbase.h"
#include <comutil.h>
#include <Iads.h>
#include <comdef.h>
#include <Adshlp.h>
#include <iiscnfg.h>	// MD_ & IIS_MD_ #defines header file.


#pragma comment( lib, "ActiveDS.lib" )
#pragma comment( lib, "adsiid.lib" )


/**
 *	常理定义
 */
#define CADSIOPIIS_WCHR_METAPATH_W3SVC	L"IIS://localhost/w3svc"


/**
 *	结构体定义
 */
typedef struct tagHostInfo
{
	tagHostInfo()
	{
		memset( this, 0, sizeof(tagHostInfo) );
	}
	
	LONG  lnCfgIndex;
	TCHAR szSiteId[ 64 ];
	TCHAR szIpAddr[ 32 ];
	DWORD dwIpAddrValue;
	ULONG uPort;
	TCHAR szHostName[ MAX_PATH ];

}STHOSTINFO, *PSTHOSTINFO;

typedef struct tagSiteInfo
{
	tagSiteInfo()
	{
		memset( this, 0, sizeof(tagSiteInfo) );
	}
	
	ULONG uIndex;		// index number
	TCHAR szSiteId[ 64 ];
	TCHAR szMetaPath[ MAX_PATH ];
	TCHAR szServerComment[ MAX_PATH ];

}STSITEINFO, *PSTSITEINFO;



/**
 *	class of CAdsiOpIIS
 */
class CAdsiOpIIS
{
public:
	CAdsiOpIIS();
	virtual ~CAdsiOpIIS();

	DWORD GetSiteCount();
	BOOL  InstallFilter( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterDesc, LPCTSTR lpcszFilterName, LPCTSTR lpcszFilterPath );
	BOOL  UnInstallFilter( LPCTSTR lpcszFilterDesc );
	BOOL  UnInstallSpecFilter( LPCTSTR lpcszFilterName, LPCTSTR lpcszMetaPath );
	DWORD GetFilterState( LPCTSTR lpcszFilterDesc, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );
	BOOL  IsFilterLoaded( LPCTSTR lpcszFilterDesc, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );


	BOOL  FindFilterMetaInfo( LPCTSTR lpcszFilterDesc, LPTSTR lpszFilterName, DWORD dwFnSize, LPTSTR lpszFilterParentPath, DWORD dwFpSize, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );
	
	//	BOOL  GetAllSiteInfo( PVOID pDataInfoData, DWORD dwSiteCount );
	BOOL  GetAllSiteInfo( vector<STSITEINFO> & vcSiteInfoAll );
	BOOL  GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll, LPCTSTR lpcszSpecSites = NULL );
	BOOL  ParseBindingsAndPuts( LPCTSTR lpcszSiteId, VARIANT & vtVarSub, vector<STHOSTINFO> & vcHostInfoAll );
	
	BOOL  DeleteSpecStrFromList( LPCTSTR lpcszStringInput, LPCTSTR lpcszSpecStr, LPCTSTR lpcszSplitStr, LPTSTR lpszStringOut, DWORD dwSize );
	
	LONG  MetaGetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey );
	BOOL  MetaSetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey, LONG lnValue );
};



#endif // __DEADSIOPIIS_HEADER__
