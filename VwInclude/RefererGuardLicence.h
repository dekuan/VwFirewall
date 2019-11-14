// RefererGuardLicence.h: interface for the CRefererGuardLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __REFERERGUARDLICENCE_HEADER__
#define __REFERERGUARDLICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VwLicence.h"


/**
 *	class of CRefererGuardLicence
 */
class CRefererGuardLicence :
	public CVwLicence
{
public:
	CRefererGuardLicence();
	virtual ~CRefererGuardLicence();

	UINT GetLimitedMaxHostCount( LPCTSTR lpcszRegPrType );
	UINT GetLimitedMaxBlacklistCount( LPCTSTR lpcszRegPrType );

	
};



#endif // __REFERERGUARDLICENCE_HEADER__
