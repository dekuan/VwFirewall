// VwLicenceAntiLeechs.h: interface for the CVwLicenceAntiLeechs class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWLICENCEANTILEECHS_HEADER__
#define __VWLICENCEANTILEECHS_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VwLicence.h"



/**
 *	class of CVwLicenceAntiLeechs
 */
class CVwLicenceAntiLeechs :
	public CVwLicence
{
public:
	CVwLicenceAntiLeechs();
	virtual ~CVwLicenceAntiLeechs();

	BOOL GetMLangPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize );
};

#endif // __VWLICENCEANTILEECHS_HEADER__



