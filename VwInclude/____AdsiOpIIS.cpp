// AdsiOpIIS.cpp: implementation of the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdsiOpIIS.h"






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAdsiOpIIS::CAdsiOpIIS()
{
	CoInitialize(NULL);
}

CAdsiOpIIS::~CAdsiOpIIS()
{
	CoUninitialize();
}

//////////////////////////////////////////////////////////////////////////
// Function:
//	Find Metainfo by lpcszFilterDesc
//
BOOL CAdsiOpIIS::FindFilterMetaInfo( LPCTSTR lpcszFilterDesc,
				    LPTSTR lpszFilterName, DWORD dwFnSize,
				    LPTSTR lpszFilterParentPath, DWORD dwFpSize,
				    LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	/*
		lpcszFilterDesc		- [in]  Specifies the description of Filter to find
		lpszFilterName		- [out] Pointer to a character null-terminated string containing the Filter name
		dwFnSize		- [in]  Specifies the length of the lpszFilterName buffer
		lpszFilterParentPath	- [out] Pointer to a character null-terminated string containing the Filter parent path
		dwFpSize		- [in]  Specifies the length of the lpszFilterParentPath buffer
		lpszFilterFilePath	- [out] Pointer to a character null-terminated string containing the Filter file path
		dwFfpSize		- [in]  Specifies the length of the lpszFilterFilePath buffer
	*/


	if ( NULL == lpcszFilterDesc )
		return FALSE;
	if ( 0 == _tcslen(lpcszFilterDesc) )
		return FALSE;

	BOOL bFind	= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum = NULL;
	IADs *pADs;
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
	IEnumVARIANT *pEnumSub = NULL;
	IADs *pADsSub;
	VARIANT vtVarSub;


	//	先读取全局站点的 Filter 的 LoadOrder 列表
	memset( szLoadOrder, 0, sizeof(szLoadOrder) );
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
		{
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
			//	..
			VariantClear( &vLoadOrder );
			iAds.Release();
		}

		iContainer.Release();
	}


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
						WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
 						if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
							memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
							//	..
							iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
							memset( szBuffer, 0, sizeof(szBuffer) );
							WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
							// ############################################################
							// 比较 Desc 算法，只要找到即可
							if ( 0 == _tcsicmp( szBuffer, lpcszFilterDesc ) &&
								strstr( szLoadOrder, szFilterName ) )
							{
								if ( lpszFilterParentPath && dwFpSize )
									_sntprintf( lpszFilterParentPath, dwFpSize-sizeof(TCHAR), "IIS://localhost/w3svc" );
								if ( lpszFilterName && dwFnSize )
									_sntprintf( lpszFilterName, dwFnSize-sizeof(TCHAR), "%s", szFilterName );
								if ( lpszFilterFilePath && dwFfpSize )
									_sntprintf( lpszFilterFilePath, dwFfpSize-sizeof(TCHAR), "%s", szFilterFilePath );

								bFind = TRUE;
							}

							VariantClear( &vtFilterFilePath );
							VariantClear( &vtFilterDesc );
							iAdsSpec.Release();
						}
					}
					pADs->Release();
				}

				// Release the VARIANT.
				VariantClear( &vtVar );

				if ( bFind )
					break;
			}

			// Free Enumer
			ADsFreeEnumerator( pEnum );
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
							WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue), szSiteId, sizeof(szSiteId), NULL, NULL );

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)-sizeof(TCHAR), "IIS://localhost/w3svc/%s", szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, _tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );

							//	先读取该站点的 LoadOrder 列表
							memset( szLoadOrder, 0, sizeof(szLoadOrder) );
							if ( S_OK == ADsGetObject( wszFindFilterPath, IID_IADsContainer, (void**)&iContainerSub ) )
							{
								if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilters"), _bstr_t("Filters"),(IDispatch**)&iAds ) )
								{
									iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
									WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, wcslen(vLoadOrder.bstrVal), szLoadOrder, sizeof(szLoadOrder), NULL, NULL );
									//	..
									VariantClear( &vLoadOrder );
									iAds.Release();
								}
								iContainerSub.Release();
							}

							_sntprintf( szFindFilterPath, sizeof(szFindFilterPath)-sizeof(TCHAR), "IIS://localhost/w3svc/%s/Filters", szSiteId );
							MultiByteToWideChar( CP_ACP, 0, szFindFilterPath, _tcslen(szFindFilterPath)+sizeof(TCHAR), wszFindFilterPath, sizeof(wszFindFilterPath)/sizeof(wszFindFilterPath[0]) );							

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
												WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue), szFilterName, sizeof(szFilterName), NULL, NULL );
												if ( S_OK == iContainerSub->GetObject( _bstr_t("IIsFilter"), bstrValue, (IDispatch**)&iAdsSpec ) )
												{
													iAdsSpec->Get( _bstr_t("FilterPath"), &vtFilterFilePath );
													memset( szFilterFilePath, 0, sizeof(szFilterFilePath) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterFilePath.bstrVal, wcslen(vtFilterFilePath.bstrVal), szFilterFilePath, sizeof(szFilterFilePath), NULL, NULL );
													//	..													
													iAdsSpec->Get( _bstr_t("FilterDescription"), &vtFilterDesc );
													memset( szBuffer, 0, sizeof(szBuffer) );
													WideCharToMultiByte( CP_ACP, 0, vtFilterDesc.bstrVal, wcslen(vtFilterDesc.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
													// ############################################################
													// 比较 Desc 算法，只要找到即可
													if ( 0 == _tcsicmp( szBuffer, lpcszFilterDesc ) &&
														strstr( szLoadOrder, szFilterName ) )
													{
														if ( lpszFilterParentPath && dwFpSize )
															_sntprintf( lpszFilterParentPath, dwFpSize-sizeof(TCHAR), "IIS://localhost/w3svc/%s", szSiteId );
														if ( lpszFilterName && dwFnSize )
															_sntprintf( lpszFilterName, dwFnSize-sizeof(TCHAR), "%s", szFilterName );
														if ( lpszFilterFilePath && dwFfpSize )
															_sntprintf( lpszFilterFilePath, dwFfpSize-sizeof(TCHAR), "%s", szFilterFilePath );

														bFind = TRUE;
													}
													
													VariantClear( &vtFilterFilePath );
													VariantClear( &vtFilterDesc );
													iAdsSpec.Release();
												}
											}
											pADsSub->Release();
										}
										VariantClear( &vtVarSub );

										if ( bFind )
											break;
									}

									// Free Enumer
									ADsFreeEnumerator( pEnumSub );
								}

								iContainerSub.Release();
							}
							//////////////////////////////////////////////////////////////////////////

						}
						pADs->Release();
					}

					// Release the VARIANT.
					VariantClear( &vtVar );
				}

				// Free Enumer
				ADsFreeEnumerator( pEnum );
			}
		}
	}

	return bFind;
}

//////////////////////////////////////////////////////////////////////////
// 获取站点的个数
//
DWORD CAdsiOpIIS::GetSiteCount()
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
		}
	}

	return dwCount;
}
/*
BOOL CAdsiOpIIS::GetAllSiteInfo( PVOID pDataInfoData, DWORD dwSiteCount )
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
								_sntprintf( pstSiteInfo[dwIndex].szMetaPath, sizeof(pstSiteInfo[dwIndex].szMetaPath)-sizeof(TCHAR), "IIS://localhost/w3svc/%s", pstSiteInfo[dwIndex].szSiteId );
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
			}

			iContainer.Release();
		}
	}

	return bRet;
}
*/
//////////////////////////////////////////////////////////////////////////
// Get All Sites
BOOL CAdsiOpIIS::GetAllSiteInfo( vector<STSITEINFO> & vcSiteInfoAll )
{
	BOOL bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum		= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched			= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;
	STSITEINFO stSiteInfo;

	// ..
	if ( S_OK == ADsGetObject( L"IIS://localhost/w3svc", IID_IADsContainer, (void**)&iContainer ) )
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
						0 != _wcsicmp( bstrValue,L"Filters" ) &&
						0 != _wcsicmp( bstrValue,L"Info" ) &&
						0 != _wcsicmp( bstrValue, L"AppPools" )
					)
					{
						if ( S_OK == iContainer->GetObject( _bstr_t("IIsWebServer"), bstrValue, (IDispatch**)&iAdsSpec ) )
						{
							iAdsSpec->Get( _bstr_t("ServerComment"), &vtVarSub );

							//	Get Site Id
							WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue),
								stSiteInfo.szSiteId, sizeof(stSiteInfo.szSiteId), NULL, NULL );
							//	Get Metabase path
							_sntprintf( stSiteInfo.szMetaPath, sizeof(stSiteInfo.szMetaPath)-sizeof(TCHAR),
								"IIS://localhost/w3svc/%s", stSiteInfo.szSiteId );
							//	Get ServerComment
							WideCharToMultiByte( CP_ACP, 0, vtVarSub.bstrVal, wcslen(vtVarSub.bstrVal),
								stSiteInfo.szServerComment, sizeof(stSiteInfo.szServerComment), NULL, NULL );

							//	..
							stSiteInfo.uIndex = vcSiteInfoAll.size();
							vcSiteInfoAll.push_back( stSiteInfo );

							bRet = TRUE;
							iAdsSpec.Release();
							VariantClear( &vtVarSub );
						}
					}
					
					pADs->Release();
				}
				
				VariantClear( &vtVar );
			}
			
			ADsFreeEnumerator( pEnum );
		}
		
		iContainer.Release();
	}
	
	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// Get All Hosts
BOOL CAdsiOpIIS::GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll, LPCTSTR lpcszSpecSites /*NULL*/ )
{
	/*
		vcHostInfoAll		- [out]    返回所有的 HOST 列表
		lpcszSpecSites		- [in/opt] 给出一个被保护的站点列表，只给出给定列表中的站点
						   格式如下："|22|33|44|"
	*/

	BOOL bRet				= FALSE;
	CComPtr<IADsContainer> iContainer;
	IEnumVARIANT *pEnum			= NULL;
	IADs *pADs;
	VARIANT vtVar;
	ULONG ulFetched				= 0L;
	CComBSTR bstrValue;
	CComPtr<IADs> iAdsSpec;
	VARIANT vtVarSub;
	BOOL  bReadyParse			= FALSE;
	TCHAR szSiteId[ 64 ]			= {0};
	TCHAR szTmpSiteId[ 128 ]		= {0};
	STHOSTINFO stHostInfo;

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
							//	Get Site Id
							memset( szSiteId, 0, sizeof(szSiteId) );
							WideCharToMultiByte( CP_ACP, 0, bstrValue, wcslen(bstrValue),
								szSiteId, sizeof(szSiteId), NULL, NULL );

							bReadyParse = TRUE;
							if ( lpcszSpecSites && strlen( lpcszSpecSites ) > 2 )
							{
								_sntprintf( szTmpSiteId, sizeof(szTmpSiteId)-sizeof(TCHAR), "|%s|", szSiteId );
								if ( ! strstr( lpcszSpecSites, szTmpSiteId ) )
								{
									bReadyParse = FALSE;
								}
							}

							//	Get ServerBindings
							VariantInit( &vtVarSub );

							if ( bReadyParse &&
								S_OK == iAdsSpec->Get( _bstr_t("ServerBindings"), &vtVarSub ) )
							{
								//	Parse bindings and put them to vcHostInfoAll
								ParseBindingsAndPuts( szSiteId, vtVarSub, vcHostInfoAll );
							}

							bRet = TRUE;
							iAdsSpec.Release();
							VariantClear( &vtVarSub );
						}
					}
					
					pADs->Release();
				}
				
				VariantClear( &vtVar );
			}
			
			ADsFreeEnumerator( pEnum );
		}
		
		iContainer.Release();
	}
	
	return bRet;
}

BOOL CAdsiOpIIS::ParseBindingsAndPuts( LPCTSTR lpcszSiteId, VARIANT & vtVarSub, vector<STHOSTINFO> & vcHostInfoAll )
{
	if ( NULL == lpcszSiteId )
		return FALSE;
	if ( 0 == strlen(lpcszSiteId) )
		return FALSE;

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
		SafeArrayAccessData( psa,(VOID**)&varArray );
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
					_sntprintf( stHostInfo.szSiteId, sizeof(stHostInfo.szSiteId)-sizeof(TCHAR), "%s", lpcszSiteId );

					_bstr_t bstmp( varArray[i].bstrVal, true );
					_sntprintf( szBinding, sizeof(szBinding)-sizeof(TCHAR), "%s", (LPCTSTR)bstmp );
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
								memcpy( szTemp, lpszHead, lpszMove-lpszHead );
								stHostInfo.uPort = atol(szTemp);

								lpszHead = lpszMove + sizeof(TCHAR);
								_sntprintf( stHostInfo.szHostName, sizeof(stHostInfo.szHostName)-sizeof(TCHAR), "%s", lpszHead );
								if ( 0 == strlen( stHostInfo.szHostName ) )
								{
									strcpy( stHostInfo.szHostName, "localhost" );
								}

								//	..
								vcHostInfoAll.push_back( stHostInfo );
							}
						}
					}
				}
			}
			SafeArrayUnaccessData(psa);
		}
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Install Filter
BOOL CAdsiOpIIS::InstallFilter( LPCTSTR lpcszMetaPath, LPCTSTR lpcszFilterDesc, LPCTSTR lpcszFilterName, LPCTSTR lpcszFilterPath )
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


	////////////////////////////////////////////////////////////
	//	转换窄字节的 MetaPath 到宽字节
	if ( lpcszMetaPath && _tcslen( lpcszMetaPath ) )
	{
		//	用户未指定，则安装到全局
		if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, _tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
		{
			return FALSE;
		}
	}
	else
	{
		//	使用默认值，安装到全局
		_snwprintf( wszMetaPath, sizeof(wszMetaPath)-sizeof(WCHAR), L"%s", CADSIOPIIS_WCHR_METAPATH_W3SVC );
	}


	////////////////////////////////////////////////////////////
	// 安装之前先清除所有已经安装的站点上的该 Filter
	while ( FindFilterMetaInfo( lpcszFilterDesc,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		UnInstallSpecFilter( szFilterName, szMetaPath );

		// ..
		if ( dwUnInsCount++ > 100 )
			break;
	}


	//////////////////////////////////////////////////////////////////////////
	//
	if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
	{
		if ( S_OK == iContainer->GetObject(_bstr_t("IIsFilters"),_bstr_t("Filters"),(IDispatch**)&iAds) )
		{
			//	第一步：将 FilterLoadOrder 列表更新一下
			iAds->Get( _bstr_t("FilterLoadOrder"), &vLoadOrder );
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
			if ( _tcslen(szBuffer) > 0 )
			{
				if ( _tcsstr( szBuffer, lpcszFilterName ) )
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)-sizeof(TCHAR), "%s", szBuffer );
				else
					_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)-sizeof(TCHAR), "%s,%s", szBuffer, lpcszFilterName );
			}
			else
			{
				_sntprintf( szFilterLoadOrder, sizeof(szFilterLoadOrder)-sizeof(TCHAR), "%s", lpcszFilterName );
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
			}
		}
	}
	
	return bRet;
}


/**
 *	卸载所有指定 DESC 的 Filter
 */
BOOL CAdsiOpIIS::UnInstallFilter( LPCTSTR lpcszFilterDesc )
{
	/*
		lpcszFilterDesc		- [in] 过滤器描述
	*/
	
	BOOL bRet = FALSE;

	DWORD dwUnInsCount			= 0;
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};

	////////////////////////////////////////
	// 卸载所有
	while ( FindFilterMetaInfo( lpcszFilterDesc,
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

//////////////////////////////////////////////////////////////////////////
// UnInstall Special Filter
BOOL CAdsiOpIIS::UnInstallSpecFilter( LPCTSTR lpcszFilterName, LPCTSTR lpcszMetaPath )
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

	if ( 0 == MultiByteToWideChar( CP_ACP, 0, lpcszMetaPath, _tcslen(lpcszMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) )
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
			WideCharToMultiByte( CP_ACP, 0, vLoadOrder.bstrVal, wcslen(vLoadOrder.bstrVal), szBuffer, sizeof(szBuffer), NULL, NULL );
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


//////////////////////////////////////////////////////////////////////////
// Get Filter State
DWORD CAdsiOpIIS::GetFilterState( LPCTSTR lpcszFilterDesc, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterDesc || 0 == _tcslen(lpcszFilterDesc) )
		return 0;

	DWORD dwRet	= 0;

	CComPtr<IADsContainer> iContainer;
	CComPtr<IADs> iAds;
	VARIANT vState;
	WCHAR wszMetaPath[ MAX_PATH ]		= {0};
	TCHAR szMetaPath[ MAX_PATH ]		= {0};
	TCHAR szFilterName[ MAX_PATH ]		= {0};
	TCHAR szFilterFilePath[ MAX_PATH ]	= {0};


	//	Firstly, Find metabase path by filter name
	if ( FindFilterMetaInfo( lpcszFilterDesc,
			szFilterName, sizeof(szFilterName),
			szMetaPath, sizeof(szMetaPath),
			szFilterFilePath, sizeof(szFilterFilePath) ) )
	{
		if ( lpszFilterFilePath && dwFfpSize )
			_sntprintf( lpszFilterFilePath, dwFfpSize-sizeof(TCHAR), "%s", szFilterFilePath );

		//	Get info ...
		_tcscat( szMetaPath, "/Filters" );
		if ( MultiByteToWideChar( CP_ACP, 0, szMetaPath, _tcslen(szMetaPath)+sizeof(TCHAR), wszMetaPath, sizeof(wszMetaPath)/sizeof(wszMetaPath[0]) ) > 0 )
		{
			if ( S_OK == ADsGetObject( wszMetaPath, IID_IADsContainer, (void**)&iContainer ) )
			{
				if ( S_OK == iContainer->GetObject( _bstr_t("IIsFilter"),_bstr_t(szFilterName),(IDispatch**)&iAds ) )
				{
					iAds->Get( _bstr_t("FilterState"), &vState );
					dwRet = vState.lVal;
				}
			}
		}
	}

	return dwRet;
}

BOOL CAdsiOpIIS::IsFilterLoaded( LPCTSTR lpcszFilterDesc, LPTSTR lpszFilterFilePath, DWORD dwFfpSize )
{
	if ( NULL == lpcszFilterDesc || 0 == _tcslen(lpcszFilterDesc) )
		return FALSE;

	BOOL  bRet	= FALSE;
	DWORD dwState	= 0;

	dwState = GetFilterState( lpcszFilterDesc, lpszFilterFilePath, dwFfpSize );
	if ( MD_FILTER_STATE_LOADED == dwState )
	{
		bRet = TRUE;
	}
	else if ( MD_FILTER_STATE_UNLOADED == dwState )
	{
		bRet = FALSE;
	}
	// ..
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
BOOL CAdsiOpIIS::DeleteSpecStrFromList( LPCTSTR lpcszStringInput, LPCTSTR lpcszSpecStr, LPCTSTR lpcszSplitStr, LPTSTR lpszStringOut, DWORD dwSize )
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
		_sntprintf( szTmpExt, sizeof(szTmpExt)-sizeof(TCHAR), "%s", lpszHead );
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
LONG CAdsiOpIIS::MetaGetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey )
{
	/*
		lpcszSiteId	- [in] 站点 ID 例如： 1, 2
		lpcszKey	- [in] 要获取的值的名称，例如：“MaxBandwidth, MaxConnections”

		ReturnValue	- the value you want. return -1 if failed, otherwise is the value that >= 0
	*/
	
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
		}
	}

	return lnRet;
}

/**
 *	设置指定键的值
 */
BOOL CAdsiOpIIS::MetaSetValueByKeyInt( LPCTSTR lpcszSiteId, LPCTSTR lpcszKey, LONG lnValue )
{
	/*
		lpcszSiteId	- [in] 站点 ID 例如： 1, 2
		lpcszKey	- [in] 要获取的值的名称，例如：“MaxBandwidth, MaxConnections”
		lnValue		- [in] 要设置的值

		ReturnValue	- true / false
	*/
	
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
			_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%d", lnValue );
			iAds->Put( _bstr_t(lpcszKey), _variant_t(szTemp) );
			if ( S_OK == iAds->SetInfo() )
			{
				bRet = TRUE;
			}
		}

		iContainer.Release();
	}

	return bRet;
}