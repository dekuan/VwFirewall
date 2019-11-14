// ServUPass.cpp: implementation of the ServUPass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServUPassword.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServUPassword::CServUPassword()
{
}

CServUPassword::~CServUPassword()
{
}

BOOL CServUPassword::IsPasswordsMatch( LPCTSTR lpszPwd, LPCTSTR lpszServUPwd )
{
	if ( NULL == lpszPwd || 0 == _tcslen(lpszPwd) )
	{
		return FALSE;
	}
	if ( NULL == lpszServUPwd || 0 == _tcslen(lpszServUPwd) )
	{
		return FALSE;
	}

	BOOL  bRet = FALSE;
	TCHAR szFst2Bit[ 4 ] = {0};
	TCHAR szMyServUPwd[ 35 ] = {0};

	_sntprintf( szFst2Bit, 2, _T("%s"), lpszServUPwd );
	GetServUPass( lpszPwd, szFst2Bit, szMyServUPwd, sizeof(szMyServUPwd) );
	if ( 0 == _tcsicmp( lpszServUPwd, szMyServUPwd ) )
	{
		bRet = TRUE;
	}
	return bRet;
}

BOOL CServUPassword::GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize )
{
	if ( NULL == lpszString || NULL == lpszMd5 )
		return FALSE;

	MD5_CTX	m_md5;
	unsigned char szEncrypt[16]	= {0};
	TCHAR szHexTmp[4]	= {0};
	TCHAR szOutput[64]	= {0};

	m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	m_md5.MD5Final( szEncrypt );
	
	for ( INT i = 0; i < 16; i++ )
	{
		memset( szHexTmp, 0, sizeof(szHexTmp) );
		_sntprintf( szHexTmp, sizeof(szHexTmp)-sizeof(TCHAR), _T("%02X"), szEncrypt[i] );
		_tcscat( szOutput, szHexTmp );
	}

	_sntprintf( lpszMd5, dwSize-sizeof(TCHAR), _T("%s"), szOutput );
	// ..
	return TRUE;
}

BOOL CServUPassword::GetServUPass( LPCTSTR lpszPwdIn, LPCTSTR lpszRand, LPTSTR lpszServUPwd, DWORD dwSPSize )
{
	TCHAR szRandNum[3]		= {0};
	TCHAR szRandNumTmp[3]		= {0};
	TCHAR szTmpPwd[ MAX_PATH ]	= {0};

	if ( NULL == lpszRand )
	{
		GetRandNum( szRandNum, sizeof(szRandNum) );
	}
	else
	{
		_sntprintf( szRandNum, 2, _T("%s"), lpszRand );
	}
	_tcscpy( szRandNumTmp, szRandNum );

	// hxFFAABB0F02F8B725C2C2FC749D8C9B9E
	TCHAR szHexTmp[4]	= {0};
	TCHAR szOutput[64]	= {0};

	_sntprintf( szTmpPwd, sizeof(szTmpPwd)-sizeof(TCHAR), _T("%s%s"), szRandNum, lpszPwdIn );
	GetStringMd5( szTmpPwd, szOutput, sizeof(szOutput) );

	// ..
	_sntprintf( lpszServUPwd, dwSPSize-sizeof(TCHAR), _T("%02s%32s"), szRandNum, szOutput );

	return TRUE;
}


BOOL CServUPassword::GetRandNum( LPTSTR lpszRandNum, DWORD dwSize )
{
	if ( NULL == lpszRandNum )
	{
		return FALSE;
	}

	srand( (unsigned)time(NULL) );
	INT iRandNum1	= rand() % 100; // 0-100之间的随机数;
	INT iRandNum2	= rand() % 100;;

	iRandNum1 += 60;
	iRandNum2 += 60;

	if ( iRandNum1 < 97 )
	{
		iRandNum1 = 97;
	}
	if ( iRandNum1 > 122 )
	{
		iRandNum1 = 122;
	}

	if ( iRandNum2 < 97 )
	{
		iRandNum2 = 97;
	}
	if ( iRandNum2 > 122 )
	{
		iRandNum2 = 122;
	}

	// ..
	_sntprintf( lpszRandNum, dwSize-sizeof(TCHAR), _T("%01c%01c"), iRandNum1, iRandNum2 );

	return TRUE;
}
