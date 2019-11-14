/*
 ************************************************************
 *
 *	CDeAdsiOpIIS.h
 *
 *	CLASS NAME:
 *		Desafe Adsi operation for IIS
 *		提供通过 ADSI 来操作 IIS Metabase 数据库的类
 * 
 *	AUTHOR:
 *		刘其星 liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-09-21 刘其星 liuqixing@gmail.com 增加
 *		Public:
 *			InstallExtension
 *			UninstallExtension
 *			IsExtensionInstalled
 *
 *		Private:
 *			ParseScriptMaps
 *			BuildScriptMaps
 *			IsFilenameExistInScriptMaps
 *			DeleteFilenameFromScriptMaps
 *
 *			ParseWebSvcExtRestrictionList
 *			BuildWebSvcExtRestrictionList
 *			IsFilenameExistInWebSvcExtRestrictionList
 *			DeleteFilenameFromWebSvcExtRestrictionList
 *
 *			MetaGetData
 *			MetaSetData
 *
 *		2009-04-08 刘其星 增加
 *		Public:
 *			ApplyExtensionScriptMapsToChildrenSite
 *		Private:
 *			AddOrUpdateScriptMaps
 *			DeleteScriptMaps
 *
 ************************************************************
 */


// AdsiOpIIS.h: interface for the CDeAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEADSIOPIIS_HEADER__
#define __DEADSIOPIIS_HEADER__


#include <vector>
using namespace std;

#include <atlbase.h>
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
#define CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST	L"IIS://localhost"
#define CDEADSIOPIIS_WCHR_METAPATH_W3SVC	L"IIS://localhost/w3svc"



/**
 *	结构体定义
 */
typedef struct tagHostInfo
{
	tagHostInfo()
	{
		memset( this, 0, sizeof(tagHostInfo) );
	}

	LONG  lnUse;
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
	WCHAR wszMetaPath[ MAX_PATH ];
	TCHAR szServerComment[ MAX_PATH ];
	TCHAR szRootPath[ 512 ];
	TCHAR szAppPoolId[ 128 ];

}STSITEINFO, *PSTSITEINFO;

//	".asp,C:\WINDOWS\system32\inetsrv\asp.dll,5,GET,HEAD,POST,TRACE"
typedef struct tagMetaScriptMap
{
	tagMetaScriptMap()
	{
		memset( this, 0, sizeof(tagMetaScriptMap) );
	}
	TCHAR szLine[ 1024 ];
	TCHAR szExtension[ 32 ];
	TCHAR szFilename[ MAX_PATH ];
	DWORD dwFlag;
	TCHAR szVerbs[ MAX_PATH ];

}STMETASCRIPTMAP, *LPSTMETASCRIPTMAP;

typedef struct tagMetaWebSvcExtRestrictionList
{
	tagMetaWebSvcExtRestrictionList()
	{
		memset( this, 0, sizeof(tagMetaWebSvcExtRestrictionList) );
	}
	TCHAR szLine[ 1024 ];
	DWORD dwAllowDenyFlag;
	TCHAR szExtensionPath[ MAX_PATH ];
	DWORD dwUIDeletableFlag;		//	是否用户手工在 IIS 的管理器中删除
	TCHAR szGroupID[ 128 ];
	TCHAR szApplicationName[ 128 ];

}STMETAWEBSVCEXTRESTRICTIONLIST, *LPSTMETAWEBSVCEXTRESTRICTIONLIST;



/**
 *	class of CDeAdsiOpIIS
 */
class CDeAdsiOpIIS
{
public:
	CDeAdsiOpIIS();
	virtual ~CDeAdsiOpIIS();

	//
	//	for site and host
	//
	DWORD GetSiteCount();
	//BOOL  GetAllSiteInfo( PVOID pDataInfoData, DWORD dwSiteCount );
	BOOL  GetAllSiteInfo( vector<STSITEINFO> & vcSiteInfoAll );
	BOOL  GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll, LPCTSTR lpcszSpecSites = NULL, BOOL bMustServerAutoStart = TRUE );

	//
	//	for filter
	//
	BOOL  InstallFilter( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterDesc, LPCTSTR lpcszFilterName, LPCTSTR lpcszFilterPath, BOOL bAutoRemoveAllFilter = TRUE );
	BOOL  OrderFilterInFirst( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterName );
	BOOL  UnInstallFilter( LPCTSTR lpcszFilterDescIn, LPCTSTR lpcszFilterNameIn, LPCTSTR lpcszFilterPathIn );
	BOOL  UnInstallSpecFilter( LPCTSTR lpcszFilterName, LPCTSTR lpcszMetaPath );
	DWORD GetFilterState( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );
	DWORD GetFilterEnabled( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );
	BOOL  IsFilterLoaded( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );

	//
	//	for extension
	//
	BOOL  InstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile );
	BOOL  ApplyExtensionScriptMapsToChildrenSite( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile );
	BOOL  UninstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile );
	BOOL  IsExtensionInstalled( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile );

	LONG  MetaGetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey );
	BOOL  MetaSetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey, LONG lnValue );

private:

	BOOL  FindFilterMetaInfo( LPCTSTR lpcszFilterPathIn, LPTSTR lpszFilterName, DWORD dwFnSize, LPTSTR lpszFilterParentPath, DWORD dwFpSize, LPTSTR lpszFilterFilePath, DWORD dwFfpSize );	
	BOOL  ParseBindingsAndPuts( LPCTSTR lpcszSiteId, VARIANT & vtVarSub, vector<STHOSTINFO> & vcHostInfoAll, BOOL bSpecSite = FALSE );

	BOOL  AddOrUpdateScriptMaps( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, DWORD dwFlag, LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile, LPCTSTR lpcszVerbs );
	BOOL  DeleteScriptMaps( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszExtensionFile );
	BOOL  ParseScriptMaps( VARIANT & vtVarData, vector<STMETASCRIPTMAP> & vcScriptMap );
	BOOL  BuildScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, VARIANT & vtVarData );
	BOOL  IsFilenameExistInScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile );
	BOOL  DeleteFilenameFromScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile );

	BOOL  ParseWebSvcExtRestrictionList( VARIANT & vtVarData, vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt );
	BOOL  BuildWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, VARIANT & vtVarData );
	BOOL  IsFilenameExistInWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile );
	BOOL  DeleteFilenameFromWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile );

	BOOL  DeleteSpecStrFromList( LPCTSTR lpcszStringInput, LPCTSTR lpcszSpecStr, LPCTSTR lpcszSplitStr, LPTSTR lpszStringOut, DWORD dwSize );

	//	从指定 metabase 路径中获取数据
	BOOL  MetaGetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub );
	BOOL  MetaSetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub );

	//
	//	others
	//
	BOOL  Enable32BitAppOnWin64();

};



#endif // __DEADSIOPIIS_HEADER__
