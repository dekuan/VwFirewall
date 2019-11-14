// HardwareInfo.h: interface for the CHardwareInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HARDWAREINFO_HEADER__
#define __HARDWAREINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


class CHardwareInfo
{
public:
	CHardwareInfo();
	virtual ~CHardwareInfo();

	BOOL GetCpuId( LPTSTR lpszCPUID, DWORD dwSize );
	BOOL GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize );

};

#endif // __HARDWAREINFO_HEADER__
