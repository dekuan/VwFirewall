// VwLicencePanDmNew.cpp: implementation of the CVwLicencePanDmNew class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwLicencePanDmNew.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwLicencePanDmNew::CVwLicencePanDmNew()
{
}

CVwLicencePanDmNew::~CVwLicencePanDmNew()
{
}

/**
 *	@ Public
 *	根据产品代码获取产品名称
 *	多语言
 */
BOOL CVwLicencePanDmNew::GetMLangPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize )
{
	if ( NULL == lpcszPrType || NULL == lpszPrTypeName || 0 == dwSize )
	{
		return FALSE;
	}

	if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMADV, lpcszPrType ) )
	{
		//	高级公司版
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_COMADV, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMNORMAL, lpcszPrType ) )
	{
		//	普通公司版
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_COMNORMAL, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLUNLMT, lpcszPrType ) )
	{
		//	终身个人版
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLUNLMT, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLADV, lpcszPrType ) )
	{
		//	高级个人版
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLADV, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLNORMAL, lpcszPrType ) )
	{
		//	普通个人版
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLNORMAL, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLRENEW, lpcszPrType ) )
	{
		//	续费
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_PSLRENEW, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_UPGRADE, lpcszPrType ) )
	{
		//	升级
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_UPGRADE, lpszPrTypeName, dwSize );
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_REMOTEHELP, lpcszPrType ) )
	{
		//	远程协助服务
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_REMOTEHELP, lpszPrTypeName, dwSize );
	}
	else
	{
		//	免费评估版本
		LoadString( AfxGetInstanceHandle(), IDS_STRING_PRTYPE_EVALUATION, lpszPrTypeName, dwSize );
	}
	
	return TRUE;
}