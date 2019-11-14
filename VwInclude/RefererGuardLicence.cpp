// RefererGuardLicence.cpp: implementation of the CRefererGuardLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefererGuardLicence.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRefererGuardLicence::CRefererGuardLicence()
{

}

CRefererGuardLicence::~CRefererGuardLicence()
{

}


/**
 *	@ Public
 *	获取最大保护站点数量
 */
UINT CRefererGuardLicence::GetLimitedMaxHostCount( LPCTSTR lpcszRegPrType )
{
	UINT uRet;
	UINT nTemp1, nTemp2, nTemp3;

	if ( NULL == lpcszRegPrType )
	{
		return 1;
	}

	if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMADV, lpcszRegPrType ) )
	{
		//uRet = 200;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 185;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMNORMAL, lpcszRegPrType ) )
	{
		//uRet = 100;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 85;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLUNLMT, lpcszRegPrType ) )
	{
		//uRet = 50;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 35;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLEXTREME, lpcszRegPrType ) )
	{
		//uRet = 50;

		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 35;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLADV, lpcszRegPrType ) )
	{
		//uRet = 20;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 15;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLNORMAL, lpcszRegPrType ) )
	{
		//uRet = 10;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 5;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLBASIC, lpcszRegPrType ) )
	{
		//uRet = 5;
		
		nTemp1	= 998765438;
		nTemp2	= 998765430;
		nTemp3	= 3;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLSMART, lpcszRegPrType ) )
	{
		//uRet = 3;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 1;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLSGLSITE, lpcszRegPrType ) )
	{
		//uRet = 1;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 4;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_EVALUATION, lpcszRegPrType ) )
	{
		//uRet = 1;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 4;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_UNKNOWN, lpcszRegPrType ) )
	{
		//uRet = 1;

		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 4;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}

	return uRet;
}

/**
 *	@ Public
 *	获取最大黑名单数量
 */
UINT CRefererGuardLicence::GetLimitedMaxBlacklistCount( LPCTSTR lpcszRegPrType )
{
	UINT uRet;
	UINT nTemp1, nTemp2, nTemp3;
	
	if ( NULL == lpcszRegPrType )
	{
		return 1;
	}
	
	if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMADV, lpcszRegPrType ) )
	{
		//uRet = 1000;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 185 + 50;
		uRet	= ( nTemp1 - nTemp2 + nTemp3 ) * 2 * 2;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_COMNORMAL, lpcszRegPrType ) )
	{
		//uRet = 500;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 185 + 50;
		uRet	= ( nTemp1 - nTemp2 + nTemp3 ) * 2;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLUNLMT, lpcszRegPrType ) )
	{
		//uRet = 250;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 185 + 50;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLEXTREME, lpcszRegPrType ) )
	{
		//uRet = 250;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 185 + 50;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLADV, lpcszRegPrType ) )
	{
		//uRet = 100;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 85;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLNORMAL, lpcszRegPrType ) )
	{
		//uRet = 50;
		
		nTemp1	= 998765445;
		nTemp2	= 998765430;
		nTemp3	= 35;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLBASIC, lpcszRegPrType ) )
	{
		//uRet = 15;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 10;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLSMART, lpcszRegPrType ) )
	{
		//uRet = 10;
		
		nTemp1	= 998765435;
		nTemp2	= 998765430;
		nTemp3	= 5;
		uRet	= nTemp1 - nTemp2 + nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_PSLSGLSITE, lpcszRegPrType ) )
	{
		//uRet = 5;
		
		nTemp1	= 998765438;
		nTemp2	= 998765430;
		nTemp3	= 3;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_EVALUATION, lpcszRegPrType ) )
	{
		//uRet = 5;
		
		nTemp1	= 998765438;
		nTemp2	= 998765430;
		nTemp3	= 3;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	else if ( 0 == _tcsicmp( CVWLICENCE_PRODUCTTYPE_UNKNOWN, lpcszRegPrType ) )
	{
		//uRet = 5;
		
		nTemp1	= 998765438;
		nTemp2	= 998765430;
		nTemp3	= 3;
		uRet	= nTemp1 - nTemp2 - nTemp3;
	}
	
	return uRet;
}
