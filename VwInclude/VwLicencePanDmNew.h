// VwLicencePanDmNew.h: interface for the CVwLicencePanDmNew class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWLICENCEPANDMNEW_HEADER__
#define __VWLICENCEPANDMNEW_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VwLicence.h"



/**
 *	class of CVwLicencePanDmNew
 */
class CVwLicencePanDmNew :
	public CVwLicence
{
public:
	CVwLicencePanDmNew();
	virtual ~CVwLicencePanDmNew();

	BOOL GetMLangPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize );
};

#endif // __VWLICENCEPANDMNEW_HEADER__






