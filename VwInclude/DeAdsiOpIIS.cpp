// AdsiOpIIS.cpp: implementation of the CDeAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeAdsiOpIIS.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeAdsiOpIIS::CDeAdsiOpIIS()
{
	CoInitialize(NULL);
}

CDeAdsiOpIIS::~CDeAdsiOpIIS()
{
	CoUninitialize();
}

/**
 *	@ public
 *	获取站点的个数
 */
DWORD CDeAdsiOpIIS::GetSiteCount()
{
	DWORD dwCount = 0;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum = NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched	= 0L;
	CComBSTR bstrValue;
	
	
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		// Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
					{
						if ( 0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
							)
						{
							dwCount ++;
						}
					}
					pADs->Release();
				}
				
				// Release the VARIANT.
				VariantClear( &vtVar );
			}
			
			// Free Enumer
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}

		iContainer.Release();
	}
	
	return dwCount;
}

/*
BOOL CDeAdsiOpIIS::GetAllSiteInfo( PVOID pDataInfoData, DWORD dwSiteCount )
{
	if ( NULL == pDataInfoData )
		return FALSE;

	BOOL bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	DWORD dwIndex			= 0;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum		= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched			= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;


	PSTSITEINFO pstSiteInfo = (PSTSITEINFO)pDataInfoData;
	if ( pstSiteInfo )
	{
		// ..
		if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
						)
						{
							if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"), bstrValue, (IDispatch**)&iAdsSpec ) )
							{
								iAdsSpec->Get( _bstr_t("ServerComment"), &vtVarSub );

								// Get Site Id
								WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue), pstSiteInfo[dwIndex].szSiteId, sizeof(pstSiteInfo[dwIndex].szSiteId), NULL, NULL );
								// Get Metabase path
								_sntprintf( pstSiteInfo[dwIndex].szMetaPath, sizeof(pstSiteInfo[dwIndex].szMetaPath)/sizeof(TCHAR)-1, "IIS://localhost/w3svc/%s", pstSiteInfo[dwIndex].szSiteId );
								// Get ServerComment
								WideCharToMultiByte( CP_ACP, 0, vtVarSub.bstrVal, wcslen(vtVarSub.bstrVal), pstSiteInfo[dwIndex].szServerComment, sizeof(pstSiteInfo[dwIndex].szServerComment), NULL, NULL );

								bRet = TRUE;
								dwIndex ++;
								iAdsSpec.Release();
								VariantClear( &vtVarSub );
							}
						}

						pADs->Release();
					}

					VariantClear( &vtVar );
				}

				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}

			iContainer.Release();
		}
	}

	return bRet;
}
*/


/**
 *	@ Public
 *	获取哦所有站点信息
 */
BOOL CDeAdsiOpIIS::GetAllSiteInfo( vector<STSITEINFO> & vcSiteInfoAll )
{
	BOOL bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum		= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched			= 0L;
	CComBSTR bstrValue;
	VARIANT vtAppPoolId;
	VARIANT vtServerComment;
	VARIANT vtRootPath;
	STSITEINFO stSiteInfo;
	WCHAR wszMetaPath[ MAX_PATH ]	= {0};

	try
	{
		// ..
		if ( S_OK == ADsGetObject( CDEADSIOPIIS_WCHR_METAPATH_W3SVC, IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					memset( & stSiteInfo, 0, sizeof(stSiteInfo) );
					
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp( bstrValue, L"Filters" ) &&
							0 != _wcsicmp( bstrValue, L"Info" ) &&
							0 != _wcsicmp( bstrValue, L"AppPools" )
						)
						{
							//	Get Site Id
							WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue),
								stSiteInfo.szSiteId, sizeof(stSiteInfo.szSiteId), NULL, NULL );

							//	Get Metabase path
							_sntprintf( stSiteInfo.szMetaPath, sizeof(stSiteInfo.szMetaPath)/sizeof(TCHAR)-1,
								"IIS://localhost/w3svc/%s", stSiteInfo.szSiteId );
							_snwprintf( stSiteInfo.wszMetaPath, sizeof(stSiteInfo.wszMetaPath)/sizeof(WCHAR)-1,
								L"IIS://localhost/w3svc/%s", bstrValue );

							//	for root path
							_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s/%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC, bstrValue );

							VariantInit( &vtAppPoolId );
							VariantClear( &vtAppPoolId );
							if ( MetaGetData( wszMetaPath, _T("IIsWebVirtualDir"), _T("Root"), _T("AppPoolId"), vtAppPoolId ) )
							{
								//	Get AppPoolId
								WideCharToMultiByte( CP_ACP, 0, vtAppPoolId.bstrVal, (int)wcslen(vtAppPoolId.bstrVal),
									stSiteInfo.szAppPoolId, sizeof(stSiteInfo.szAppPoolId), NULL, NULL );
							}

							VariantInit( &vtServerComment );
							VariantInit( &vtRootPath );
							if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_W3SVC, _T("IIsWebServer"), stSiteInfo.szSiteId, _T("ServerComment"), vtServerComment ) &&
								MetaGetData( wszMetaPath, _T("IIsWebVirtualDir"), _T("Root"), _T("Path"), vtRootPath ) )
							{
								//	Get ServerComment
								WideCharToMultiByte( CP_ACP, 0, vtServerComment.bstrVal, (int)wcslen(vtServerComment.bstrVal),
									stSiteInfo.szServerComment, sizeof(stSiteInfo.szServerComment), NULL, NULL );

								//	Get RootPath
								WideCharToMultiByte( CP_ACP, 0, vtRootPath.bstrVal, (int)wcslen(vtRootPath.bstrVal),
									stSiteInfo.szRootPath, sizeof(stSiteInfo.szRootPath), NULL, NULL );

								stSiteInfo.uIndex = (ULONG)( vcSiteInfoAll.size() );
								vcSiteInfoAll.push_back( stSiteInfo );

								bRet = TRUE;
							}
							VariantClear( &vtServerComment );
							VariantClear( &vtRootPath );
							
						}
						
						pADs->Release();
					}
					
					VariantClear( &vtVar );
				}
				
				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}
			
			iContainer.Release();
		}
	}
	catch ( ... )
	{
	}	

	return bRet;
}


/**
 *	@ Public
 *	获取所有 HOST
 */
BOOL CDeAdsiOpIIS::GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll, LPCTSTR lpcszSpecSites /*=NULL*/, BOOL bMustServerAutoStart /*=TRUE*/ )
{
	//
	//	vcHostInfoAll		- [out]    返回所有的 HOST 列表
	//	lpcszSpecSites		- [in/opt] 给出一个被保护的站点(siteid)列表，只给出给定列表中的站点
	//					   格式如下："|22|33|44|"
	//	bMustServerAutoStart	- [in/opt] 站点的状态必须是启动的
	//	RETURN			- TRUE / FALSE
	//

	BOOL bRet				= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum			= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched				= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;
	BOOL  bSpecSite;
	TCHAR szSiteId[ 64 ]			= {0};
	TCHAR szTmpSiteId[ 128 ]		= {0};
	STHOSTINFO stHostInfo;
	BOOL  bSaveItem;

	// ..
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		// Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				memset( & stHostInfo, 0, sizeof(stHostInfo) );

				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
						0 != _wcsicmp( bstrValue,L"Filters" ) &&
						0 != _wcsicmp( bstrValue,L"Info" ) &&
						0 != _wcsicmp( bstrValue, L"AppPools" )
					)
					{
						if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							bRet = TRUE;

							bSaveItem	= TRUE;

							if ( bMustServerAutoStart )
							{
								//	要求站点状态为启动正在运行的
								VariantInit( &vtVarSub );
								if ( S_OK == iAdsSpec->Get( _bstr_t("ServerAutoStart"), &vtVarSub ) )
								{
									if ( 0 == vtVarSub.intVal || 0 == vtVarSub.iVal )
									{
										//	站点状态可能已经关闭
										bSaveItem = FALSE;
									}
								}
								VariantClear( &vtVarSub );
							}

							if ( bSaveItem )
							{
								//	Get Site Id
								memset( szSiteId, 0, sizeof(szSiteId) );
								WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue),
									szSiteId, sizeof(szSiteId), NULL, NULL );

								bSpecSite = TRUE;
								if ( lpcszSpecSites && strlen( lpcszSpecSites ) > 2 )
								{
									_sntprintf( szTmpSiteId, sizeof(szTmpSiteId)/sizeof(TCHAR)-1, _T("|%s|"), szSiteId );
									if ( ! strstr( lpcszSpecSites, szTmpSiteId ) )
									{
										bSpecSite = FALSE;
									}
								}

								//	Get ServerBindings
								VariantInit( &vtVarSub );

								//
								//	获取 ServerBindings 并且解析，最后 push 到 vcHostInfoAll
								//
								if ( S_OK == iAdsSpec->Get( _bstr_t("ServerBindings"), &vtVarSub ) )
								{
									//	Parse bindings and put them to vcHostInfoAll
									ParseBindingsAndPuts( szSiteId, vtVarSub, vcHostInfoAll, bSpecSite );
								}
								VariantClear( &vtVarSub );
							}

							//	release
							iAdsSpec.Release();
						}
					}
					
					pADs->Release();
				}
				
				VariantClear( &vtVar );
			}
			
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}
		
		iContainer.Release();
	}
	
	return bRet;
}


/**
 *	@ Public
 *	安装过滤器
 */
BOOL CDeAdsiOpIIS::InstallFilter( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterDesc, LPCTSTR lpcszFilterName, LPCTSTR lpcszFilterPath, BOOL bAutoRemoveAllFilter /*= TRUE*/ )
{
	//
	//	lpcszMetaPath		- [in] Metabase Path, for example:
	//					"IIS://localhost/w3svc/1"	: Filter will install at the site which siteid=1
	//					"IIS://localhost/w3svc"		: Filter will install at the global
	//	lpcszFilterDesc		- [in] Filter 的描述
	//	lpcszFilterName		- [in] Filter 的名称
	//	lpcszFilterPath		- [in] Filter 的硬盘路径
	//


//	if ( NULL == lpcszMetaPath || 0 == _tcslen(lpcszMetaPath) )
//		return FALSE;
	if ( NULL == lpcszFilterDesc || 0 == _tcslen(lpcszFilterDesc) )
	{
		return FALSE;
	}
	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszFilterPath || 0 == _tcslen(lpcszFilterPath) )
	{
		return FALSE;
	}
	if ( FALSE == PathFileExists(lpcszFilterPath) )
	{
		return FALSE;
	}


	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADsContainer> iContainer2;
	CComPtr<IADs> iAds;
	CComPtr<IADs> iAds2;
	VARIANT vLoadOrder;
	WCHAR wszFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};


	//
	//	如果是 X64 位操作系统，则需要修改 Metabase 数据库，让 IIS 支持 32 位程序在 X64 系统上兼容运行
	//	
	Enable32BitAppOnWin64();


	////////////////////////////////////////////////////////////
	//	转换窄字节的 MetaPath 到宽字节
	if ( lpcszMetaPath && _tcslen( lpcszMetaPath ) )
	{
		//	用户未指定，则安装到全局
		if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)_tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
		{
			return FALSE;
		}
	}
	else
	{
		//	使用默认值，安装到全局
		_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC );
	}


	////////////////////////////////////////////////////////////
	// 安装之前先清除所有已经安装的站点上的该 Filter
	if ( bAutoRemoveAllFilter )
	{
		while ( FindFilterMetaInfo( lpcszFilterPath,
				szFilterName, sizeof(szFilterName),
				szMetaPath, sizeof(szMetaPath),
				szFilterFilePath, sizeof(szFilterFilePath) ) )
		{
			UnInstallSpecFilter( szFilterName, szMetaPath );

			// ..
			if ( dwUnInsCount++ > 500 )
				break;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t("IIsFilters"),_bstr_t("Filters"),(IDispatch**)&iAds) )
		{
			//	第一步：将 FilterLoadOrder 列表更新一下
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( _tcsstr( szBuffer, lpcszFilterName ) )
				{
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s"), szBuffer );
				}
				else
				{
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s,%s"), szBuffer, lpcszFilterName );
				}
			}
			else
			{
				_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s"), lpcszFilterName );
			}
			iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szFilterLoadOrder) );
			iAds->SetInfo();

			//	第二步：写入信息
			if ( S_OK == iAds->QueryInterface(IID_IADsContainer,(void**)&iContainer2) )
			{
				bRet = TRUE;
				
				if ( S_OK == iContainer2->Create(_bstr_t("IIsFilter"),_bstr_t(lpcszFilterName),(IDispatch**)&iAds2) )
				{
					iAds2->Put( _bstr_t("FilterPath"), _variant_t(lpcszFilterPath) );
					iAds2->SetInfo();
				}

				iAds2.Release();
			}

			iAds.Release();
		}

		iContainer.Release();
	}
	
	return bRet;
}

/**
 *	@ Public
 *	将制定的过滤器排序到第一位，使得加载更加优先
 */
BOOL CDeAdsiOpIIS::OrderFilterInFirst( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterName )
{
	//
	//	lpcszFilterName		- [in] Filter 的名称
	//

	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vLoadOrder;
	WCHAR wszFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};
	TCHAR szNewFilterLoadOrder[ MAX_PATH ]	= {0};


	////////////////////////////////////////////////////////////
	//	转换窄字节的 MetaPath 到宽字节
	if ( lpcszMetaPath && _tcslen( lpcszMetaPath ) )
	{
		//	用户未指定，则安装到全局
		if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)_tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
		{
			return FALSE;
		}
	}
	else
	{
		//	使用默认值，安装到全局
		_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s", CDEADSIOPIIS_WCHR_METAPATH_W3SVC );
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t("IIsFilters"),_bstr_t("Filters"),(IDispatch**)&iAds) )
		{
			//
			//	1，获取 FilterLoadOrder 列表
			//	2，删除函数传入参数的名称
			//	3，重新组装，将函数传入参数的名称放在第一位
			//	4，保存之
			//
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( _tcsstr( szBuffer, _T(",") ) && _tcsstr( szBuffer, lpcszFilterName ) )
				{
					if ( DeleteSpecStrFromList( szBuffer, lpcszFilterName, ",", szFilterLoadOrder, sizeof(szFilterLoadOrder) ) )
					{
						//	..
						_sntprintf( szNewFilterLoadOrder, sizeof(szNewFilterLoadOrder)/sizeof(TCHAR)-1, _T("%s,%s"), lpcszFilterName, szFilterLoadOrder );

						//	Save new FilterLoadOrder
						iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szNewFilterLoadOrder) );
						iAds->SetInfo();
					}
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}

/**
 *	@ Public
 *	卸载所有指定 DESC 的 Filter
 */
BOOL CDeAdsiOpIIS::UnInstallFilter( LPCTSTR lpcszFilterDescIn, LPCTSTR lpcszFilterNameIn, LPCTSTR lpcszFilterPathIn )
{
	//
	//	lpcszFilterDescIn	- [in] 过滤器描述
	//	lpcszFilterNameIn	- [in] 过滤器名称，例如 "VirtualWall"
	//	lpcszFilterPathIn	- [in] 过滤器文件全路径，例如 "c:\\VirtualWall\\VirtualWall.dll"
	//	RETURN			- TRUE / FLASE
	//
	
	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};

	////////////////////////////////////////
	// 卸载所有
	while ( FindFilterMetaInfo( lpcszFilterPathIn,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) )
	)
	{
		bRet = TRUE;
		UnInstallSpecFilter( szFilterName, szMetaPath );

		// ..
		if ( dwUnInsCount++ > 100 )
			break;
	}

	return bRet;
}


/**
 *	@ Public
 *	UnInstall Special Filter
 */
BOOL CDeAdsiOpIIS::UnInstallSpecFilter( LPCTSTR lpcszFilterName, LPCTSTR lpcszMetaPath )
{
	if ( NULL == lpcszFilterName || 0 == _tcslen(lpcszFilterName) )
		return FALSE;
	if ( NULL == lpcszMetaPath || 0 == _tcslen(lpcszMetaPath) )
		return FALSE;


	BOOL bRet = FALSE;

	CComPtr<IADsContainer> iContainer;
	CComPtr<IADsContainer> iContainer2;
	CComPtr<IADs> iAds;
	CComPtr<IADs> iAds2;
	VARIANT vLoadOrder;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};
	TCHAR szFilterLoadOrder[ MAX_PATH ]	= {0};

	if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, (int)( _tcslen(lpcszMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
		{
			//	第一步：将 FilterLoadOrder 列表更新一下
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( DeleteSpecStrFromList( szBuffer, lpcszFilterName, ",", szFilterLoadOrder, sizeof(szFilterLoadOrder) ) )
				{
					//	..
					iAds->Put( _bstr_t("FilterLoadOrder"), _variant_t(szFilterLoadOrder) );
					iAds->SetInfo();

					//	第二步：删除 Filter 信息
					if ( S_OK == iAds->QueryInterface( IID_IADsContainer, (void**)&iContainer2 ) )
					{
						if ( S_OK  == iContainer2->Delete( _bstr_t("IIsFilter"), _bstr_t(lpcszFilterName) ) )
						{
							bRet = TRUE;
						}
						//	..
						iContainer2.Release();
					}
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}


/**
 *	@ Public
 *	Get Filter State
 */
DWORD CDeAdsiOpIIS::GetFilterState( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return 0;
	}

	DWORD dwRet	= 0;

	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vState;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};


	//	Firstly, Find metabase path by filter name
	if ( FindFilterMetaInfo( lpcszFilterFilePathIn,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		if ( lpszFilterFilePath && dwFfpSize )
		{
			_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
		}

		//	Get info ...
		_tcscat( szMetaPath, "/Filters" );
		if ( MultiByteToWideChar( CP_ACP, 0, szMetaPath, (int)( _tcslen(szMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) > 0 )
		{
			if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
			{
				VariantInit( &vState );
				if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"),_bstr_t(szFilterName),(IDispatch**)&iAds ) )
				{
					iAds->Get( _bstr_t("FilterState"), &vState );
					dwRet = vState.lVal;

					iAds.Release();
				}
				VariantClear( &vState );

				iContainer.Release();
			}
		}
	}

	return dwRet;
}

/**
 *	@ Public
 *	Get Filter Enabled
 */
DWORD CDeAdsiOpIIS::GetFilterEnabled( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return 0;
	}
	
	DWORD dwRet	= 0;
	
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vState;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	
	
	//	Firstly, Find metabase path by filter name
	if ( FindFilterMetaInfo( lpcszFilterFilePathIn,
		szFilterName, sizeof(szFilterName),
		szMetaPath, sizeof(szMetaPath),
		szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		if ( lpszFilterFilePath && dwFfpSize )
		{	
			_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
		}	
		
		//	Get info ...
		_tcscat( szMetaPath, "/Filters" );
		if ( MultiByteToWideChar( CP_ACP, 0, szMetaPath, (int)( _tcslen(szMetaPath)+sizeof(TCHAR) ), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) > 0 )
		{
			if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
			{
				//VariantInit( &vState );
				if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"),_bstr_t(szFilterName),(IDispatch**)&iAds ) )
				{
					iAds->Get( _bstr_t("FilterEnabled"), &vState );
					dwRet = vState.bVal ? 1 : 0;

					iAds.Release();
				}
				//VariantClear( &vState );

				iContainer.Release();
			}
		}
	}

	return dwRet;
}

/**
 *	@ Public
 *	过滤器是否被装载
 */
BOOL CDeAdsiOpIIS::IsFilterLoaded( LPCTSTR lpcszFilterFilePathIn, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	//
	//	lpcszFilterFilePathIn	- [in]  指定过滤器文件路径
	//	lpszFilterFilePath	- [out] 返回的路径，其实没有啥用，瞎扯
	//	dwFfpSize		- [in]  返回缓冲区大小
	//

	if ( NULL == lpcszFilterFilePathIn || 0 == _tcslen(lpcszFilterFilePathIn) )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	DWORD dwState	= 0;
	ENUMDEWINDOWSSYSTYPE osver;


	osver	= delib_get_shellsystype();
	if ( _DEOS_WINDOWS_2008 == osver || _DEOS_WINDOWS_VISTA == osver || _DEOS_WINDOWS_2008_R2 == osver )
	{
		if ( GetFilterEnabled( lpcszFilterFilePathIn, lpszFilterFilePath, dwFfpSize ) )
		{
			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}
	}
	else
	{
		dwState = GetFilterState( lpcszFilterFilePathIn, lpszFilterFilePath, dwFfpSize );
		if ( MD_FILTER_STATE_LOADED == dwState )
		{
			bRet = TRUE;
		}
		else if ( MD_FILTER_STATE_UNLOADED == dwState )
		{
			bRet = FALSE;
		}
	}
	// ..
	return bRet;
}


//
//	for extension
//
BOOL CDeAdsiOpIIS::InstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] 扩展的名称，建议最好是英文
	//	lpcszExtensionFile	- [in] 扩展物理文件全路径
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bSaveScriptMaps;
	BOOL bSaveWebSvcExt;

//	VARIANT vArrScriptMaps;
//	VARIANT vArrScriptMapsNew;
//	vector<STMETASCRIPTMAP> vcScriptMaps;
//	STMETASCRIPTMAP stMapNew;
	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	VARIANT vArrWebSvcExt;
	VARIANT vArrWebSvcExtNew;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;
	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;


	//
	//	如果是 X64 位操作系统，则需要修改 Metabase 数据库，让 IIS 支持 32 位程序在 X64 系统上兼容运行
	//	
	Enable32BitAppOnWin64();


	bSaveScriptMaps	= FALSE;
	bSaveWebSvcExt	= FALSE;

	//
	//	安装到“通配符应用程序映射”
	//
	if ( AddOrUpdateScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), 0, _T("*"), lpcszExtensionFile, "" ) )
	{
		//	应用到所有子站点
		vcSiteInfoAll.clear();
		if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
		{
			for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
			{
				//pstSite = (STSITEINFO*)its;
				AddOrUpdateScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), 0, _T("*"), lpcszExtensionFile, "" );
			}
		}

		bSaveScriptMaps = TRUE;
	}

	//
	//	安装到“Web服务扩展”
	//
	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	如果文件已经存在，则干掉它(因为有可能路径不相同)
			//
			if ( IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
			{
				DeleteFilenameFromWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile );
			}

			memset( & stWebSvcExt, 0, sizeof(stWebSvcExt) );
			
			stWebSvcExt.dwAllowDenyFlag	= 1;		//	默认设置为允许
			_sntprintf( stWebSvcExt.szExtensionPath, sizeof(stWebSvcExt.szExtensionPath)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionFile );
			stWebSvcExt.dwUIDeletableFlag	= 1;		//	允许用户自己删除
			_tcscpy( stWebSvcExt.szGroupID, _T("") );	//	没有组
			_sntprintf( stWebSvcExt.szApplicationName, sizeof(stWebSvcExt.szApplicationName)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionName );
			
			vcWebSvcExt.push_back( stWebSvcExt );

			if ( BuildWebSvcExtRestrictionList( vcWebSvcExt, vArrWebSvcExtNew ) )
			{
				//
				//	保存到 Metabase
				//
				bSaveWebSvcExt = MetaSetData
				(
					CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST,
					_T("IIsWebService"),
					_T("W3SVC"),
					_T("WebSvcExtRestrictionList"),
					vArrWebSvcExtNew
				);
			}
		}
	}

	return ( bSaveScriptMaps && bSaveWebSvcExt );
}

/**
 *	@ Public
 *	将主站上的 ScriptMaps 映射到子站点
 */
BOOL CDeAdsiOpIIS::ApplyExtensionScriptMapsToChildrenSite( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] 扩展的名称，建议最好是英文
	//	lpcszExtensionFile	- [in] 扩展物理文件全路径
	//	RETURN			- TRUE / FALSE
	//
	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	if ( IsExtensionInstalled( lpcszExtensionName, lpcszExtensionFile ) )
	{
		if ( AddOrUpdateScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), 0, _T("*"), lpcszExtensionFile, "" ) )
		{
			//	应用到所有子站点
			vcSiteInfoAll.clear();
			if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
			{
				for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
				{
					//pstSite = (STSITEINFO*)its;
					AddOrUpdateScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), 0, _T("*"), lpcszExtensionFile, "" );
				}
			}

			bRet = TRUE;
		}
	}

	return bRet;
}


/**
 *	@ Public
 *	卸载扩展
 */
BOOL CDeAdsiOpIIS::UninstallExtension( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] 扩展的名称，建议最好是英文
	//	lpcszExtensionFile	- [in] 扩展物理文件全路径
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bSaveScriptMaps;
	BOOL bSaveWebSvcExt;
//	CComPtr<IADsContainer> iContainer;
//	CComPtr<IADs> iAds;

//	VARIANT vArrScriptMaps;
//	VARIANT vArrScriptMapsNew;
//	vector<STMETASCRIPTMAP> vcScriptMaps;
//	STMETASCRIPTMAP stMapNew;
	vector<STSITEINFO> vcSiteInfoAll;
	vector<STSITEINFO>::iterator its;
	//STSITEINFO * pstSite;

	VARIANT vArrWebSvcExt;
	VARIANT vArrWebSvcExtNew;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;
	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;


	bSaveScriptMaps	= FALSE;
	bSaveWebSvcExt	= FALSE;

	//
	//	卸载所有 ScriptMaps
	//
	if ( DeleteScriptMaps( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), lpcszExtensionFile ) )
	{
		//	应用到所有子站点
		vcSiteInfoAll.clear();
		if ( GetAllSiteInfo( vcSiteInfoAll ) && vcSiteInfoAll.size() )
		{
			for ( its = vcSiteInfoAll.begin(); its != vcSiteInfoAll.end(); its ++ )
			{
				//pstSite = (STSITEINFO*)its;
				DeleteScriptMaps( (*its).wszMetaPath, _T("IIsWebVirtualDir"), _T("root"), lpcszExtensionFile );
			}
		}
		
		bSaveScriptMaps = TRUE;
	}

	//
	//	卸载“Web服务扩展”
	//
	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	如果列表中存在，那么就从列表中删除该文件
			//
			if ( IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
			{
				if ( DeleteFilenameFromWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile ) )
				{
					//
					//	创建新的列表
					//
					if ( BuildWebSvcExtRestrictionList( vcWebSvcExt, vArrWebSvcExtNew ) )
					{
						//
						//	保存到 Metabase
						//
						bSaveWebSvcExt = MetaSetData
						(
							CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST,
							_T("IIsWebService"),
							_T("W3SVC"),
							_T("WebSvcExtRestrictionList"),
							vArrWebSvcExtNew
						);
					}
				}
			}
			else
			{
				bSaveWebSvcExt = TRUE;
			}
		}
	}

	return ( bSaveScriptMaps && bSaveWebSvcExt );
}

BOOL CDeAdsiOpIIS::IsExtensionInstalled( LPCTSTR lpcszExtensionName, LPCTSTR lpcszExtensionFile )
{
	//
	//	lpcszExtensionName	- [in] 扩展的名称，建议最好是英文
	//	lpcszExtensionFile	- [in] 扩展物理文件全路径
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszExtensionFile ) )
	{
		return FALSE;
	}

	BOOL bScriptMapsExist;
	BOOL bWebSvcExtExist;

	VARIANT vArrScriptMaps;
	vector<STMETASCRIPTMAP> vcScriptMaps;

	VARIANT vArrWebSvcExt;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST> vcWebSvcExt;


	//
	//	初始化
	//
	bScriptMapsExist	= FALSE;
	bWebSvcExtExist		= FALSE;

	//
	//	开始检查
	//
	VariantInit( &vArrScriptMaps );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("ScriptMaps"), vArrScriptMaps ) )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	看看这个 map 文件是否存在
			//
			bScriptMapsExist = IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile );
		}
		vcScriptMaps.clear();
	}
	VariantClear( &vArrScriptMaps );

	VariantInit( &vArrWebSvcExt );
	if ( MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, _T("IIsWebService"), _T("W3SVC"), _T("WebSvcExtRestrictionList"), vArrWebSvcExt ) )
	{
		vcWebSvcExt.clear();
		if ( ParseWebSvcExtRestrictionList( vArrWebSvcExt, vcWebSvcExt ) )
		{
			//
			//	看看这个 map 文件是否存在
			//
			bWebSvcExtExist = IsFilenameExistInWebSvcExtRestrictionList( vcWebSvcExt, lpcszExtensionFile );
		}
		vcWebSvcExt.clear();
	}
	VariantClear( &vArrWebSvcExt );

	return ( bScriptMapsExist && bWebSvcExtExist );
}





//////////////////////////////////////////////////////////////////////////
//	Private








//////////////////////////////////////////////////////////////////////////
// Function:
//	Find Metainfo by lpcszFilterDesc
//
BOOL CDeAdsiOpIIS::FindFilterMetaInfo( LPCTSTR lpcszFilterPathIn,
				    LPTSTR lpszFilterName, DWORD dwFnSize,
				    LPTSTR lpszFilterParentPath, DWORD dwFpSize,
				    LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	//
	//	lpcszFilterPathIn	- [in]  过滤器文件全路径
	//	lpszFilterName		- [out] Pointer to a character null-terminated string containing the Filter name
	//	dwFnSize		- [in]  Specifies the length of the lpszFilterName buffer
	//	lpszFilterParentPath	- [out] Pointer to a character null-terminated string containing the Filter parent path
	//	dwFpSize		- [in]  Specifies the length of the lpszFilterParentPath buffer
	//	lpszFilterFilePath	- [out] Pointer to a character null-terminated string containing the Filter file path
	//	dwFfpSize		- [in]  Specifies the length of the lpszFilterFilePath buffer
	//	RETURN			- TRUE / FALSE
	//


	if ( NULL == lpcszFilterPathIn )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszFilterPathIn) )
	{
		return FALSE;
	}

	BOOL bFind	= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT * pEnum = NULL;
	IADs * pADs;
	VARIANT vtVar;
	ULONG ulFetched	= 0L;
	CComBSTR bstrValue;

	CComPtr<IADs> iAds;
	VARIANT vLoadOrder;
	TCHAR szLoadOrder[ 1024 ]		= {0};

	CComPtr<IADs> iAdsSpec;
	VARIANT vtFilterDesc;
	VARIANT vtFilterFilePath;
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szBuffer[ MAX_PATH ]		= {0};

	TCHAR szSiteId[ MAX_PATH ]	= {0};
	TCHAR szFindFilterPath[ MAX_PATH ]	= {0};
	WCHAR wszFindFilterPath[ MAX_PATH ]	= {0};
	CComPtr<IADsContainer> iContainerSub;
	IEnumVARIANT * pEnumSub = NULL;
	IADs * pADsSub;
	VARIANT vtVarSub;


	//	先读取全局站点的 Filter 的 LoadOrder 列表
	memset( szLoadOrder, 0, sizeof(szLoadOrder) );
	VariantClear( &vLoadOrder );
	if ( MetaGetData( L"IIS://localhost/w3svc", _T("IIsFilters"), _T("Filters"), _T("FilterLoadOrder"), vLoadOrder ) )
	{
		WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
	}
	VariantClear( &vLoadOrder );


	// [1] 先检查全局的 IIsFilters 里是否有
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc/Filters", IID_IADsContainer, (void**)&iContainer ) )
	{
		//	Create an enumerator object in the container.
		if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
		{
			// Get the next contained object.
			while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
			{
				if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
				{
					if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
					{
						memset( szFilterName, 0, sizeof(szFilterName) );
						WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
 						if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
							memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, (int)wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
							//	..
							iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
							memset( szBuffer, 0, sizeof(szBuffer) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, (int)wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
							
							// ############################################################
							// 比较 文件名 算法，只要找到文件名相同的即可
							if ( _tcslen(szFilterFilePath) &&
								0 == _tcsicmp( PathFindFileName(szFilterFilePath), PathFindFileName(lpcszFilterPathIn) ) &&
								strstr( szLoadOrder, szFilterName ) )
							{
								if ( lpszFilterParentPath && dwFpSize )
								{
									_sntprintf( lpszFilterParentPath, dwFpSize/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc") );
								}
								if ( lpszFilterName && dwFnSize )
								{
									_sntprintf( lpszFilterName, dwFnSize/sizeof(TCHAR)-1, _T("%s"), szFilterName );
								}
								if ( lpszFilterFilePath && dwFfpSize )
								{
									_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, _T("%s"), szFilterFilePath );
								}

								bFind = TRUE;
							}

							VariantClear( &vtFilterFilePath );
							VariantClear( &vtFilterDesc );
							
							iAdsSpec.Release();

							//SysFreeString( bstrValue );
							//bstrValue.Detach();
						}

						//SysFreeString( bstrValue );
						//bstrValue.Detach();
					}

					pADs->Release();
				}

				// Release the VARIANT.
				VariantClear( &vtVar );

				if ( bFind )
				{
					break;
				}
			}

			// Free Enumer
			ADsFreeEnumerator( pEnum );
			pEnum = NULL;
		}

		iContainer.Release();
	}


	// [2] 检查每个站点的 Filter
	if ( ! bFind )
	{
		if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
		{
			// Create an enumerator object in the container.
			if ( S_OK == ADsBuildEnumerator( iContainer, &pEnum ) )
			{
				// Get the next contained object.
				while( S_OK == ADsEnumerateNext( pEnum, 1, &vtVar, &ulFetched ) && ulFetched > 0 )
				{
					if ( S_OK == V_DISPATCH(&vtVar)->QueryInterface( IID_IADs, (void**)&pADs ) )
					{
						if ( S_OK == pADs->get_Name(&bstrValue) && wcslen(bstrValue) > 0 &&
							0 != _wcsicmp(bstrValue,L"Filters") && 0 != _wcsicmp(bstrValue,L"Info") )
						{
							memset( szSiteId, 0, sizeof(szSiteId) );
							WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szSiteId, sizeof(szSiteId), NULL, NULL );

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc/%s"), szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, (int)_tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );

							//	先读取该站点的 LoadOrder 列表
							memset( szLoadOrder, 0, sizeof(szLoadOrder) );
							if ( S_OK == ADsGetObject( wszFindFilterPath, IID_IADsContainer, (void**)&iContainerSub ) )
							{
								if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
								{
									iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
									WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, (int)wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
									//	..
									VariantClear( &vLoadOrder );
									iAds.Release();
								}
								iContainerSub.Release();
							}

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)/sizeof(TCHAR)-1, _T("IIS://localhost/w3svc/%s/Filters"), szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, (int)_tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );							

							//////////////////////////////////////////////////////////////////////////
							//	获取这个站点下的所有 Filter
							if ( S_OK == ADsGetObject( wszFindFilterPath, IID_IADsContainer, (void**)&iContainerSub ) )
							{
								//	Create an enumerator object in the container.
								if ( S_OK == ADsBuildEnumerator( iContainerSub, &pEnumSub ) )
								{
									//	Get the next contained object.
									while( S_OK == ADsEnumerateNext( pEnumSub, 1, &vtVarSub, &ulFetched ) && ulFetched > 0 )
									{
										if ( S_OK == V_DISPATCH(&vtVarSub)->QueryInterface( IID_IADs, (void**)&pADsSub ) )
										{
											if ( S_OK == pADsSub->get_Name(&bstrValue) && wcslen(bstrValue) > 0 )
											{
												// ..
												memset( szFilterName, 0, sizeof(szFilterName) );
												WideCharToMultiByte( CP_ACP, 0, bstrValue, (int)wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
												if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
												{
													iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
													memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, (int)wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
													//	..													
													iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
													memset( szBuffer, 0, sizeof(szBuffer) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, (int)wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
													
													// ############################################################
													//	比较 文件名 算法，只要找到文件名相同的即可
													if ( _tcslen(szFilterFilePath) &&
														0 == _tcsicmp( PathFindFileName(szFilterFilePath), PathFindFileName(lpcszFilterPathIn) ) &&
														strstr( szLoadOrder, szFilterName ) )
													{
														if ( lpszFilterParentPath && dwFpSize )
														{	
															_sntprintf( lpszFilterParentPath, dwFpSize/sizeof(TCHAR)-1, "IIS://localhost/w3svc/%s", szSiteId );
														}	
														if ( lpszFilterName && dwFnSize )
														{	
															_sntprintf( lpszFilterName, dwFnSize/sizeof(TCHAR)-1, "%s", szFilterName );
														}	
														if ( lpszFilterFilePath && dwFfpSize )
														{	
															_sntprintf( lpszFilterFilePath, dwFfpSize/sizeof(TCHAR)-1, "%s", szFilterFilePath );
														}	

														bFind = TRUE;
													}
													
													VariantClear( &vtFilterFilePath );
													VariantClear( &vtFilterDesc );
													iAdsSpec.Release();

													//SysFreeString( bstrValue );
													//bstrValue.Detach();
												}

												//SysFreeString( bstrValue );
												//bstrValue.Detach();
											}
											pADsSub->Release();
										}
										VariantClear( &vtVarSub );

										if ( bFind )
										{	
											break;
										}
									}

									// Free Enumer
									ADsFreeEnumerator( pEnumSub );
									pEnumSub = NULL;
								}

								iContainerSub.Release();
							}
							//////////////////////////////////////////////////////////////////////////

							//SysFreeString( bstrValue );
							//bstrValue.Detach();
						}

						pADs->Release();
					}

					// Release the VARIANT.
					VariantClear( &vtVar );
				}

				// Free Enumer
				ADsFreeEnumerator( pEnum );
				pEnum = NULL;
			}

			iContainer.Release();
		}
	}

	return bFind;
}




/**
 *	@ private
 *	解析 binding 并且保存到 vector 中
 */
BOOL CDeAdsiOpIIS::ParseBindingsAndPuts( LPCTSTR lpcszSiteId, VARIANT & vtVarSub, vector<STHOSTINFO> & vcHostInfoAll, BOOL bSpecSite /* = FALSE */ )
{
	if ( NULL == lpcszSiteId )
	{
		return FALSE;
	}
	if ( 0 == strlen(lpcszSiteId) )
	{
		return FALSE;
	}

	STHOSTINFO stHostInfo;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szBinding[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};
	LPTSTR lpszMove			= NULL;
	LPTSTR lpszHead			= NULL;


	if ( vtVarSub.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarSub.parray;
		if ( S_OK == SafeArrayAccessData( psa, (VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( varArray[i].vt == VT_BSTR )
					{
						memset( & stHostInfo, 0, sizeof(stHostInfo) );
						_sntprintf( stHostInfo.szSiteId, sizeof(stHostInfo.szSiteId)/sizeof(TCHAR)-1, _T("%s"), lpcszSiteId );

						_bstr_t bstmp( varArray[i].bstrVal, true );
						_sntprintf( szBinding, sizeof(szBinding)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
						if ( strlen(szBinding) )
						{
							lpszHead = szBinding;
							lpszMove = strstr( lpszHead, ":" );
							if ( lpszMove )
							{
								if ( 0 != strcmp( lpszHead, lpszMove ) )
								{
									//	127.0.0.1:80:xing.com
									memcpy( stHostInfo.szIpAddr, lpszHead, lpszMove-lpszHead );
								}
								else
								{
									//	:80:xing.com
									strcpy( stHostInfo.szIpAddr, "" );
								}

								lpszHead = lpszMove + sizeof(TCHAR);
								lpszMove = strstr( lpszHead, ":" );
								if ( lpszMove )
								{
									memset( szTemp, 0, sizeof(szTemp) );
									memcpy( szTemp, lpszHead, lpszMove-lpszHead );
									stHostInfo.uPort = atol(szTemp);

									lpszHead = lpszMove + sizeof(TCHAR);
									_sntprintf( stHostInfo.szHostName, sizeof(stHostInfo.szHostName)/sizeof(TCHAR)-1, _T("%s"), lpszHead );
									if ( 0 == strlen( stHostInfo.szHostName ) )
									{
										strcpy( stHostInfo.szHostName, "localhost" );
									}

									//	是否指定的
									stHostInfo.lnUse = ( bSpecSite ? 1 : 0 );

									//	..
									vcHostInfoAll.push_back( stHostInfo );
								}
							}
						}
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}

	return TRUE;
}


/**
 *	@ private
 *	添加新 ScriptMaps
 */
BOOL CDeAdsiOpIIS::AddOrUpdateScriptMaps(
	LPCWSTR	lpcwszPathName,
	LPCTSTR lpcszKeyType,
	LPCTSTR	lpcszKeyName,
	DWORD	dwFlag,
	LPCTSTR lpcszExtensionName,
	LPCTSTR lpcszExtensionFile,
	LPCTSTR lpcszVerbs )
{
	//
	//
	//	ScriptMaps
	//	<Extension>, <ScriptProcessor>, <Flags>, <IncludedVerbs>
	//	<Flags> is an integer represnting a bitmask with one of the following values:
	//	--------------------------------------------------------------------------------
	//	MD_SCRIPTMAPFLAG_SCRIPT			= 1	The script is allowed to run in directories given Script permission. If this value is not set, then the script can only be executed in directories that are flagged for Execute permission.
	//	MD_SCRIPTMAPFLAG_CHECK_PATH_INFO	= 4	The server attempts to access the PATH_INFO portion of the URL, as a file, before starting the scripting engine. If the file can't be opened, or doesn't exist, an error is returned to the client.
	//	MD_SCRIPTMAPFLAG_SCRIPT |
	//	MD_SCRIPTMAPFLAG_CHECK_PATH_INFO	= 5	Both of the above conditions are TRUE.
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionName || 0 == _tcslen(lpcszExtensionName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	BOOL bReadScriptMaps	= FALSE;
	VARIANT vArrScriptMaps;
	VARIANT vArrScriptMapsNew;
	vector<STMETASCRIPTMAP> vcScriptMaps;
	STMETASCRIPTMAP stMapNew;

	//
	//	安装到“通配符应用程序映射”
	//
	VariantInit( &vArrScriptMaps );
	bReadScriptMaps = MetaGetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMaps );
	if ( ( VT_ARRAY|VT_VARIANT ) == vArrScriptMaps.vt )
	{
		bReadScriptMaps = TRUE;
	}
	else
	{
		if ( 0 != _wcsnicmp( lpcwszPathName, CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, wcslen(CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST) ) )
		{
			//	如果不是读取根目录，全局站
			//	则，从全局站读取来，复制给子站点
			VariantInit( &vArrScriptMaps );
			bReadScriptMaps = MetaGetData( CDEADSIOPIIS_WCHR_METAPATH_LOCALHOST, lpcszKeyType, _T("W3SVC"), _T("ScriptMaps"), vArrScriptMaps );
		}
	}

	if ( bReadScriptMaps )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	如果 map 已经存在，则干掉它(因为有可能路径不相同)
			//
			if ( IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
			{
				DeleteFilenameFromScriptMaps( vcScriptMaps, lpcszExtensionFile );
			}
			
			memset( & stMapNew, 0, sizeof(stMapNew) );

			stMapNew.dwFlag	= dwFlag;
			_sntprintf( stMapNew.szExtension, sizeof(stMapNew.szExtension)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionName );
			_sntprintf( stMapNew.szFilename, sizeof(stMapNew.szFilename)/sizeof(TCHAR)-1, _T("%s"), lpcszExtensionFile );
			_sntprintf( stMapNew.szVerbs, sizeof(stMapNew.szVerbs)/sizeof(TCHAR)-1, _T("%s"), lpcszVerbs );
			
			vcScriptMaps.push_back( stMapNew );

			if ( BuildScriptMaps( vcScriptMaps, vArrScriptMapsNew ) )
			{
				//
				//	保存到 MetaBase
				//
				bRet = MetaSetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMapsNew );
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	删除 ScriptMaps
 */
BOOL CDeAdsiOpIIS::DeleteScriptMaps( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszExtensionFile || 0 == _tcslen(lpcszExtensionFile) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	VARIANT vArrScriptMaps;
	VARIANT vArrScriptMapsNew;
	vector<STMETASCRIPTMAP> vcScriptMaps;
	STMETASCRIPTMAP stMapNew;

	//
	//	卸载所有 ScriptMaps
	//
	VariantInit( &vArrScriptMaps );
	if ( MetaGetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMaps ) )
	{
		vcScriptMaps.clear();
		if ( ParseScriptMaps( vArrScriptMaps, vcScriptMaps ) )
		{
			//
			//	如果列表中存在，那么就从列表中删除该文件
			//
			if ( IsFilenameExistInScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
			{
				if ( DeleteFilenameFromScriptMaps( vcScriptMaps, lpcszExtensionFile ) )
				{
					//
					//	创建新的列表
					//
					if ( BuildScriptMaps( vcScriptMaps, vArrScriptMapsNew ) )
					{
						//
						//	保存到 MetaBase
						//
						bRet = MetaSetData( lpcwszPathName, lpcszKeyType, lpcszKeyName, _T("ScriptMaps"), vArrScriptMapsNew );
					}
				}
			}
			else
			{
				bRet = TRUE;
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	解析 ScriptMaps
 */
BOOL CDeAdsiOpIIS::ParseScriptMaps( VARIANT & vtVarData, vector<STMETASCRIPTMAP> & vcScriptMaps )
{
	STMETASCRIPTMAP stMap;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	
	BOOL bParseSucc;
	STTEXTLINE * pstParseStrList	= NULL;
	INT nParseStrCount;
	INT nPsi;
	INT nVerbsLen;
	

	if ( vtVarData.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarData.parray;
		if ( S_OK == SafeArrayAccessData( psa,(VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( VT_BSTR != varArray[i].vt )
					{
						continue;
					}

					memset( &stMap, 0, sizeof(stMap) );
					bParseSucc	= FALSE;

					_bstr_t bstmp( varArray[i].bstrVal, true );
					_sntprintf( stMap.szLine, sizeof(stMap.szLine)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
					if ( _tcslen(stMap.szLine) )
					{
						//
						//	解析字符串
						//
						nParseStrCount = delib_split_string( stMap.szLine, _T(","), NULL, FALSE );
						if ( nParseStrCount >= 3 )
						{
							pstParseStrList = new STTEXTLINE[ nParseStrCount ];
							if ( pstParseStrList )
							{
								//
								//	到此认为解析一个 map 数据成功
								//
								bParseSucc = TRUE;


								memset( pstParseStrList, 0, sizeof(STTEXTLINE)*nParseStrCount );
								delib_split_string( stMap.szLine, _T(","), pstParseStrList, FALSE );

								//	获取 extension 名称
								_sntprintf( stMap.szExtension, sizeof(stMap.szExtension)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 0 ].szLine );

								//	获取文件路径
								_sntprintf( stMap.szFilename, sizeof(stMap.szFilename)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 1 ].szLine );

								//	获取 标记 名称
								stMap.dwFlag	= atoi( pstParseStrList[ 2 ].szLine );

								//	获取 szVerbs
								if ( nParseStrCount > 3 )
								{
									nVerbsLen = 0;
									for ( nPsi = 3; nPsi < nParseStrCount; nPsi ++ )
									{
										nVerbsLen += (INT)( _tcslen(pstParseStrList[ nPsi ].szLine) + sizeof(TCHAR) );
									}
									if ( nVerbsLen < sizeof(stMap.szVerbs) )
									{
										for ( nPsi = 3; nPsi < nParseStrCount; nPsi ++ )
										{
											_tcscat( stMap.szVerbs, pstParseStrList[ nPsi ].szLine );
											if ( nPsi != ( nParseStrCount - 1 ) )
											{
												_tcscat( stMap.szVerbs, _T(",") );
											}
										}
									}
								}

								delete [] pstParseStrList;
								pstParseStrList = NULL;
							}
						}

					}

					//
					//	保存数据到 vector
					//
					if ( bParseSucc )
					{
						vcScriptMaps.push_back( stMap );
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}
	
	return TRUE;
}


/**
 *	@ private
 *	根据数组创建 ScriptMaps
 */
BOOL CDeAdsiOpIIS::BuildScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, VARIANT & vtVarData )
{
	if ( 0 == vcScriptMaps.size() )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	//STMETASCRIPTMAP * pstMap;
//	HRESULT hr;
	SAFEARRAY * psa;
	SAFEARRAYBOUND aDim[1];			//	a one dimensional array
	vector<STMETASCRIPTMAP>::iterator it;
	LONG ArrLong[ 1 ];
	LONG lnIndex;
	LONG lnEnd;

	//
	//	Create SafeArray of VARIANT BSTRs
	//
	aDim[0].lLbound		= 0;  // Visual Basic arrays start with index 0
	aDim[0].cElements	= (ULONG)( vcScriptMaps.size() );

	psa = SafeArrayCreate( VT_VARIANT, 1, aDim );	//	create a 1D SafeArray of VARIANTS
	if ( psa )
	{
		lnIndex	= aDim[0].lLbound;
		lnEnd	= aDim[0].lLbound + aDim[0].cElements;

		for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
		{
			if ( lnIndex >= lnEnd )
			{
				break;
			}

			//pstMap = (STMETASCRIPTMAP*)it;
			//if ( (*it) )
			{
				_sntprintf
				(
					(*it).szLine,
					sizeof((*it).szLine)/sizeof(TCHAR)-1,
					_T("%s,%s,%d%s%s"),
					(*it).szExtension,
					(*it).szFilename,
					(*it).dwFlag,
					( _tcslen((*it).szVerbs) ? "," : "" ),
					(*it).szVerbs
				);

				VARIANT vOut;
				BSTR  bstrTmp	= A2BSTR( _T("") );

				VariantInit(&vOut);

				bstrTmp	= A2BSTR( _T((*it).szLine) );
				vOut.vt = VT_BSTR;
				vOut.bstrVal = bstrTmp;

				//	定义索引值
				ArrLong[0]	= lnIndex;

				if ( S_OK == SafeArrayPutElement( psa, ArrLong, &vOut ) )
				{
					bRet = TRUE;
				}

				VariantClear( &vOut );
				// does a deep destroy of source VARIANT
			}

			//
			//	索引增加
			//
			lnIndex ++;
		}

		vtVarData.vt		= ( VT_ARRAY|VT_VARIANT );
		vtVarData.parray	= psa;
	}

	//
	//	clean up here only if you do not return SafeArray as an [out, retval]
	//
	//SafeArrayDestroy( psa );	// again does a deep destroy
	
	return bRet;
}

/**
 *	@ private
 *	检查文件名是否存在于 ScriptMap 中
 */
BOOL CDeAdsiOpIIS::IsFilenameExistInScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	vector<STMETASCRIPTMAP>::iterator it;
	//STMETASCRIPTMAP * pstMap;

	bRet	= FALSE;
	for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
	{
		//pstMap = it;
		//if ( (*it) )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szFilename), PathFindFileName(lpcszExtensionFile) ) )
			{
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}

/**
 *	@ private
 *	从 ScriptMap 中删除指定文件名
 */
BOOL CDeAdsiOpIIS::DeleteFilenameFromScriptMaps( vector<STMETASCRIPTMAP> & vcScriptMaps, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETASCRIPTMAP>::iterator it;
	//STMETASCRIPTMAP * pstMap;

	bRet	= FALSE;
	for ( it = vcScriptMaps.begin(); it != vcScriptMaps.end(); it ++ )
	{
		//pstMap = it;
		//if ( (*it) )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szFilename), PathFindFileName(lpcszExtensionFile) ) )
			{
				//	从 vector 中删除
				vcScriptMaps.erase( it );

				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}


/**
 *	@ private
 *	解析 WebSvcExtRestrictionList
 */
BOOL CDeAdsiOpIIS::ParseWebSvcExtRestrictionList( VARIANT & vtVarData, vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt )
{
	//
	//	vtVarData	- [in]
	//	vcWebSvcExt	- [out] vector 返回数组
	//	RETURN
	//
	//	WebSvcExtRestrictionList 格式:
	//		AllowDenyFlag,ExtensionPath,UIDeletableFlag,GroupID,ApplicationName
	//	例如:
	//		0,*.exe
	//		0,*.dll
	//		1,C:\WINDOWS\system32\inetsrv\ssinc.dll,0,SSINC,在服务器端的包含文件
	//		0,C:\WINDOWS\system32\inetsrv\httpext.dll,0,WEBDAV,WebDAV
	//		1,C:\Inetpub\wwwroot\DeInfoMFilter\DeInfoMFilter.dll,1,,DeInfoMFilter
	//

	STMETAWEBSVCEXTRESTRICTIONLIST stWebSvcExt;
	SAFEARRAY * psa			= NULL;
	VARIANT * varArray		= NULL;
	UINT uDim			= 0;
	LONG lLbound			= 0;
	LONG lRbound			= 0;
	LONG i				= 0;
	TCHAR szTemp[ MAX_PATH ]	= {0};

	BOOL bParseSucc;
	STTEXTLINE * pstParseStrList	= NULL;
	INT nParseStrCount;


	if ( vtVarData.vt == ( VT_ARRAY|VT_VARIANT ) )
	{
		psa = vtVarData.parray;
		if ( S_OK == SafeArrayAccessData( psa,(VOID**)&varArray ) )
		{
			uDim = SafeArrayGetDim( psa );
			if ( 1 == uDim )
			{
				SafeArrayGetLBound( psa, 1, &lLbound );
				SafeArrayGetUBound( psa, 1, &lRbound );

				for ( i = lLbound; i <= lRbound; i++ )
				{
					if ( VT_BSTR != varArray[i].vt )
					{
						continue;
					}

					memset( &stWebSvcExt, 0, sizeof(stWebSvcExt) );
					bParseSucc	= FALSE;

					_bstr_t bstmp( varArray[i].bstrVal, true );
					_sntprintf( stWebSvcExt.szLine, sizeof(stWebSvcExt.szLine)/sizeof(TCHAR)-1, _T("%s"), (LPCTSTR)bstmp );
					if ( _tcslen(stWebSvcExt.szLine) )
					{
						//
						//	解析字符串
						//
						nParseStrCount = delib_split_string( stWebSvcExt.szLine, _T(","), NULL, TRUE );
						if ( nParseStrCount >= 2 )
						{
							pstParseStrList = new STTEXTLINE[ nParseStrCount ];
							if ( pstParseStrList )
							{
								//
								//	到此认为解析一个 map 数据成功
								//
								bParseSucc = TRUE;


								memset( pstParseStrList, 0, sizeof(STTEXTLINE)*nParseStrCount );
								delib_split_string( stWebSvcExt.szLine, _T(","), pstParseStrList, TRUE );

								stWebSvcExt.dwAllowDenyFlag = atoi( pstParseStrList[ 0 ].szLine );
								_sntprintf( stWebSvcExt.szExtensionPath, sizeof(stWebSvcExt.szExtensionPath)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 1 ].szLine );
								if ( nParseStrCount > 2 )
								{
									stWebSvcExt.dwUIDeletableFlag = atoi( pstParseStrList[ 2 ].szLine );
								}
								if ( nParseStrCount > 3 )
								{
									_sntprintf( stWebSvcExt.szGroupID, sizeof(stWebSvcExt.szGroupID)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 3 ].szLine );
								}
								if ( nParseStrCount > 4 )
								{
									_sntprintf( stWebSvcExt.szApplicationName, sizeof(stWebSvcExt.szApplicationName)/sizeof(TCHAR)-1, _T("%s"), pstParseStrList[ 4 ].szLine );
								}

								delete [] pstParseStrList;
								pstParseStrList = NULL;
							}
						}

					}

					//
					//	保存数据到 vector
					//
					if ( bParseSucc )
					{
						vcWebSvcExt.push_back( stWebSvcExt );
					}
				}
			}

			if ( varArray )
			{
				VariantClear( varArray );
				varArray = NULL;
			}
			SafeArrayUnaccessData( psa );
			psa = NULL;
		}
	}
	
	return TRUE;
}

/**
 *	@ private
 *	创建新的 WebSvcExtRestrictionList
 */
BOOL CDeAdsiOpIIS::BuildWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, VARIANT & vtVarData )
{
	//
	//
	//
	//	WebSvcExtRestrictionList 格式:
	//		AllowDenyFlag,ExtensionPath,UIDeletableFlag,GroupID,ApplicationName
	//	例如:
	//		0,*.exe
	//		0,*.dll
	//		1,C:\WINDOWS\system32\inetsrv\ssinc.dll,0,SSINC,在服务器端的包含文件
	//		0,C:\WINDOWS\system32\inetsrv\httpext.dll,0,WEBDAV,WebDAV
	//		1,C:\Inetpub\wwwroot\DeInfoMFilter\DeInfoMFilter.dll,1,,DeInfoMFilter
	//

	if ( 0 == vcWebSvcExt.size() )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	SAFEARRAY * psa;
	SAFEARRAYBOUND aDim[1];			//	a one dimensional array
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	LONG ArrLong[ 1 ];
	LONG lnIndex;
	LONG lnEnd;

	//
	//	Create SafeArray of VARIANT BSTRs
	//
	aDim[0].lLbound		= 0;			//	Visual Basic arrays start with index 0
	aDim[0].cElements	= (ULONG)( vcWebSvcExt.size() );

	psa = SafeArrayCreate( VT_VARIANT, 1, aDim );	//	create a 1D SafeArray of VARIANTS
	if ( psa )
	{
		lnIndex	= aDim[0].lLbound;
		lnEnd	= aDim[0].lLbound + aDim[0].cElements;

		for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
		{
			if ( lnIndex >= lnEnd )
			{
				break;
			}

			//pstWebSvcExt = it;
			//if ( pstWebSvcExt )
			{
				if ( _tcslen((*it).szApplicationName) )
				{
					//
					//	完整的
					//
					_sntprintf
					(
						(*it).szLine,
						sizeof((*it).szLine)/sizeof(TCHAR)-1,
						_T("%d,%s,%d,%s,%s"),
						(*it).dwAllowDenyFlag,
						(*it).szExtensionPath,
						(*it).dwUIDeletableFlag,
						(*it).szGroupID,
						(*it).szApplicationName
					);
				}
				else
				{
					//
					//	类似：0,*.exe
					//
					_sntprintf
					(
						(*it).szLine,
						sizeof((*it).szLine)/sizeof(TCHAR)-1,
						_T("%d,%s"),
						(*it).dwAllowDenyFlag,
						(*it).szExtensionPath
					);
				}

				VARIANT vOut;
				BSTR  bstrTmp	= A2BSTR( _T("") );
				
				VariantInit(&vOut);
				
				bstrTmp	= A2BSTR( _T((*it).szLine) );
				vOut.vt = VT_BSTR;
				vOut.bstrVal = bstrTmp;

				//	定义索引值
				ArrLong[0]	= lnIndex;
				
				if ( S_OK == SafeArrayPutElement( psa, ArrLong, &vOut ) )
				{
					bRet = TRUE;
				}
				
				VariantClear( &vOut );
				// does a deep destroy of source VARIANT
			}
			
			//
			//	索引增加
			//
			lnIndex ++;
		}
		
		vtVarData.vt		= ( VT_ARRAY|VT_VARIANT );
		vtVarData.parray	= psa;
	}
	
	//
	//	clean up here only if you do not return SafeArray as an [out, retval]
	//
	//SafeArrayDestroy( psa );	// again does a deep destroy
	
	return bRet;
}

/**
 *	@ private
 *	判断文件名在 WebSvcExtRestrictionList 中是否存在
 */
BOOL CDeAdsiOpIIS::IsFilenameExistInWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	
	bRet	= FALSE;
	for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
	{
		//pstWebSvcExt = it;
		//if ( pstWebSvcExt )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szExtensionPath), PathFindFileName(lpcszExtensionFile) ) )
			{
				bRet = TRUE;
				break;
			}
		}
	}
	
	return bRet;
}

/**
 *	@ private
 *	从 WebSvcExtRestrictionList 中删除指定文件名
 */
BOOL CDeAdsiOpIIS::DeleteFilenameFromWebSvcExtRestrictionList( vector<STMETAWEBSVCEXTRESTRICTIONLIST> & vcWebSvcExt, LPCTSTR lpcszExtensionFile )
{
	if ( NULL == lpcszExtensionFile )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	vector<STMETAWEBSVCEXTRESTRICTIONLIST>::iterator it;
	//STMETAWEBSVCEXTRESTRICTIONLIST * pstWebSvcExt;
	
	bRet	= FALSE;
	for ( it = vcWebSvcExt.begin(); it != vcWebSvcExt.end(); it ++ )
	{
		//pstWebSvcExt = it;
		//if ( pstWebSvcExt )
		{
			if ( 0 == _tcsicmp( PathFindFileName((*it).szExtensionPath), PathFindFileName(lpcszExtensionFile) ) )
			{
				//	从 vector 中删除
				vcWebSvcExt.erase( it );

				bRet = TRUE;
				break;
			}
		}
	}
	
	return bRet;
}





//////////////////////////////////////////////////////////////////////////
// 功能：
//	将指定字符串从一个长字符串中删除
// 参数
//	lpcszStringInput	长字符串
//	lpcszSpecStr		子字符串
//	lpszStringOut		返回的新 LoadOrder 缓冲区地址
//	dwSize			返回缓冲区大小
BOOL CDeAdsiOpIIS::DeleteSpecStrFromList( LPCTSTR lpcszStringInput, LPCTSTR lpcszSpecStr, LPCTSTR lpcszSplitStr, LPTSTR lpszStringOut, DWORD dwSize )
{
	/*
		lpcszStringInput	- [in]  Specifies the lpcszStringInput
		lpcszSpecStr		- [in]  Specifies the lpcszSpecStr
		lpcszSplitStr		- [in]  Specifies lpcszSplitStr
		lpszStringOut		- [out] Pointer to a character null-terminated string containing the StringOut 
		dwSize			- [in]  Specifies the length of the lpszStringOut buffer
	*/

	if ( NULL == lpcszStringInput || 0 == _tcslen(lpcszStringInput) )
		return FALSE;
	if ( NULL == lpcszSpecStr || 0 == _tcslen(lpcszSpecStr) )
		return FALSE;
	if ( NULL == lpcszSplitStr || 0 == _tcslen(lpcszSplitStr) )
		return FALSE;
	if ( NULL == lpszStringOut || 0 == dwSize )
		return FALSE;

	// .mdb|.exe|.rar|.zip
	BOOL bRet	= FALSE;
	UINT uIndex	= 0;
	LPCTSTR lpszHead = NULL;
	LPTSTR lpszMove = NULL;
	TCHAR  szTmpExt[ MAX_PATH ] = {0};

	//	Only one Filter, set emtpy and return true
	if ( 0 == stricmp( lpcszStringInput, lpcszSpecStr ) )
	{
		strcpy( lpszStringOut, "" );
		return TRUE;
	}


	//	Init Exts List
	lpszHead = lpcszStringInput;
	lpszMove = NULL;
	while( lpszHead && _tcslen(lpszHead) > 0 )
	{
		_sntprintf( szTmpExt, sizeof(szTmpExt)/sizeof(TCHAR)-1, _T("%s"), lpszHead );
		lpszMove = _tcsstr( szTmpExt, lpcszSplitStr );
		if ( lpszMove )
		{
			*lpszMove = '\0';
			lpszHead += _tcslen(szTmpExt) + _tcslen(lpcszSplitStr);
		}
		else
		{
			lpszHead += _tcslen(szTmpExt);
		}
		
		if ( _tcslen(szTmpExt) > 0 && 0 != _tcsicmp( szTmpExt, lpcszSpecStr ) )
		{
			if ( _tcslen(lpszStringOut)+_tcslen(szTmpExt) < dwSize-sizeof(TCHAR)-1 )
			{
				bRet = TRUE;
				if ( 0 != uIndex )
					_tcscat( lpszStringOut, lpcszSplitStr );
				_tcscat( lpszStringOut, szTmpExt );
				
				uIndex ++;
			}
		}
	}
	return bRet;
}

/**
 *	获取指定键的值
 */
BOOL CDeAdsiOpIIS::MetaGetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub )
{
	//
	//	lpcwszPathName	- [in] Metabase 的路径，例如：L"IIS://localhost"
	//	lpcszKeyType	- [in] 要打开的 lpszPathName 的类型，例如：IIsWebService
	//	lpcszKeyName	- [in] 要打开的子 key 名称，例如：W3SVC
	//	lpcszValueName	- [in] 要获取值的键的名字，例如：ScriptMaps
	//	vtVarSub	- [out] 输出
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszValueName || 0 == _tcslen(lpcszValueName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;

	//
	//	开始安装
	//
	try
	{
		if ( S_OK == ADsGetObject( (LPWSTR)lpcwszPathName, IID_IADsContainer, (void**)&iContainer ) )
		{
			if ( S_OK == iContainer->GetObject(_bstr_t(lpcszKeyType),_bstr_t(lpcszKeyName),(IDispatch**)&iAds) )
			{
				bRet	= TRUE;
				
				//
				//	ScriptMaps
				//
				VariantInit( &vtVarSub );
				if ( S_OK == iAds->Get( _bstr_t(lpcszValueName), &vtVarSub ) )
				{
					bRet = TRUE;
				}
				//VariantClear( &vtVarSub );

				iAds.Release();
			}

			iContainer.Release();
		}
	}
	catch ( ... )
	{
	}

	return bRet;
}


/**
 *	设置指定键的值
 */
BOOL CDeAdsiOpIIS::MetaSetData( LPCWSTR lpcwszPathName, LPCTSTR lpcszKeyType, LPCTSTR lpcszKeyName, LPCTSTR lpcszValueName, VARIANT & vtVarSub )
{
	//
	//	lpcwszPathName	- [in] Metabase 的路径，例如：L"IIS://localhost"
	//	lpcszKeyType	- [in] 要打开的 lpszPathName 的类型，例如：IIsWebService
	//	lpcszKeyName	- [in] 要打开的子 key 名称，例如：W3SVC
	//	lpcszValueName	- [in] 要获取值的键的名字，例如：ScriptMaps
	//	vtVarSub	- [in] 要保存的值
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpcwszPathName || 0 == wcslen(lpcwszPathName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyType || 0 == _tcslen(lpcszKeyType) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKeyName || 0 == _tcslen(lpcszKeyName) )
	{
		return FALSE;
	}
	if ( NULL == lpcszValueName || 0 == _tcslen(lpcszValueName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	
	//
	//	开始安装
	//
	if ( S_OK == ADsGetObject( (LPWSTR)lpcwszPathName, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t(lpcszKeyType),_bstr_t(lpcszKeyName),(IDispatch**)&iAds) )
		{
			//
			//	保存到 MetaBase
			//
			if ( S_OK == iAds->Put( _bstr_t(lpcszValueName), vtVarSub ) )
			{
				if ( S_OK == iAds->SetInfo() )
				{
					bRet = TRUE;
				}
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}

/**
 *	获取指定键的值
 */
LONG CDeAdsiOpIIS::MetaGetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey )
{
	//
	//	lpcszSiteId	- [in] 站点 ID 例如： 1, 2
	//	lpcszKey	- [in] 要获取的值的名称，例如：“MaxBandwidth, MaxConnections”
	//
	//	ReturnValue	- the value you want. return -1 if failed, otherwise is the value that >= 0
	//

	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
		return -1;
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
		return -1;
	
	LONG lnRet	= -1;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vValue;

	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"),_bstr_t(lpcszSiteId),(IDispatch**)&iAds ) )
		{
			//	MaxBandwidth, MaxConnections
			iAds->Get( _bstr_t( lpcszKey ), &vValue );
			lnRet = vValue.lVal;

			iAds.Release();
		}

		iContainer.Release();
	}

	return lnRet;
}

/**
 *	设置指定键的值
 */
BOOL CDeAdsiOpIIS::MetaSetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey, LONG lnValue )
{
	//
	//	lpcszSiteId	- [in] 站点 ID 例如： 1, 2
	//	lpcszKey	- [in] 要获取的值的名称，例如：“MaxBandwidth, MaxConnections”
	//	lnValue		- [in] 要设置的值
	//
	//	ReturnValue	- true / false
	//
	
	if ( NULL == lpcszSiteId || 0 == _tcslen(lpcszSiteId) )
		return -1;
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
		return -1;
	
	BOOL bRet	= FALSE;
	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	//VARIANT vValue;
	TCHAR szTemp[ MAX_PATH ]	= {0};


	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"),_bstr_t(lpcszSiteId),(IDispatch**)&iAds ) )
		{
			_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%d"), lnValue );
			iAds->Put( _bstr_t(lpcszKey), _variant_t(szTemp) );
			if ( S_OK == iAds->SetInfo() )
			{
				bRet = TRUE;
			}

			iAds.Release();
		}

		iContainer.Release();
	}

	return bRet;
}


/**
 *	@ Private
 *	如果是 X64 位操作系统，则需要修改 Metabase 数据库，让 IIS 支持 32 位程序在 X64 系统上兼容运行
 */
BOOL CDeAdsiOpIIS::Enable32BitAppOnWin64()
{
	//
	//	命令行执行是：
	//	cscript.exe adsutil.vbs set W3SVC/AppPools/Enable32BitAppOnWin64 "true"
	//	RETURN		- TRUE/FALSE
	//

	BOOL bRet	= FALSE;

	if ( delib_is_wow64() )
	{
		bRet = MetaSetData
		(
			L"IIS://localhost/W3SVC",
			_T("IIsApplicationPools"),
			_T("AppPools"),
			_T("Enable32BitAppOnWin64"),
			_variant_t("1")
		);
	}

	return bRet;
}
