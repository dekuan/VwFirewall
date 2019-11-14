// DeLicence.h: interface for the CDeLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DELICENCE_HEADER__
#define __DELICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



/**
 *	DeRemoteMBase.ini 文件中 [License] 节内信息
 */
typedef struct tagDeLicenseInfo
{
	tagDeLicenseInfo()
	{
		memset( this, 0, sizeof(tagDeLicenseInfo) );
	}

	TCHAR szKey[ MAX_PATH ];
	TCHAR szGene[ 128 ];
	TCHAR szRegCode[ 128 ];
	TCHAR szActCode[ 128 ];

}STDELICENSEINFO, *PSTDELICENSEINFO;




/**
 *	class of CDeLicence
 */
class CDeLicence
{
public:
	CDeLicence();
	virtual ~CDeLicence();

protected:
	BOOL InitData( IN HINSTANCE hCallerInstance, IN LPCTSTR lpcszKey, IN BOOL bCreateGeneWithDiskSN );
	BOOL IsValidLicence( LPCTSTR lpszActCode );

public:
	BOOL GetGene( OUT LPTSTR lpszGene, IN DWORD dwSize );
	BOOL GetRegCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszRegCode, IN DWORD dwSize );
	BOOL GetActCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszActCode, IN DWORD dwSize );

	BOOL GetMacAddress( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize );
	BOOL GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize );
	BOOL GetCpuId( LPTSTR lpszCPUID, DWORD dwSize );
	BOOL GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );
	BOOL GetCookieValue( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );
	BOOL IsEqualString( LPCTSTR lpcszStr1, LPCTSTR lpcszStr2 );



private:
	BOOL GetFormatLicenceString( LPCTSTR lpcszInput, LPTSTR lpszLicenceString, DWORD dwSize );

public:
	STDELICENSEINFO mb_stLcInfo;
	BOOL  m_bCreateGeneWithDiskSN;

	TCHAR m_szDecCDisk[ 32 ];
	TCHAR m_szDecFmt1[ 32 ];
	TCHAR m_szDecFmtRegCode[ 64 ];
	TCHAR m_szDecFmtActCode[ 64 ];
	TCHAR m_szDecFmtMacAddress[ 64 ];
	TCHAR m_szDecFmtCpuId[ 64 ];

//	TCHAR mb_szKey[ MAX_PATH ];		//	加密 key
//	TCHAR mb_szGene[ MAX_PATH ];		//	特征因子
//	TCHAR mb_szRegCode[ 128 ];		//	regcode 用于在线获取 ActCode 的注册码
//	TCHAR mb_szActCode[ 128 ];		//	actcode 真正的注册码

};


#endif // __DELICENCE_HEADER__


