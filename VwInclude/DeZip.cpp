// DeZip.cpp: implementation of the CDeZip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeZip.h"
#include <stdio.h>
#include <stdlib.h>

CDeZip::CDeZip( PSP_UNCOMPRESS_CALLBACK pfnDetailInfo ) : m_pfnCallbackDetailInfo( pfnDetailInfo )
{
}

CDeZip::~CDeZip()
{
}


/**
 *	@ Public
 *	解压一个 ZIP 文件
 */
BOOL CDeZip::UnZipFiles( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir, BOOL bUnZipSubDir, BOOL bFirstEnter )
{
	/*
		lpZipFile		- [in] Zip 文件路径
		lpUnZipFileDir		- [in] 解压到的目录
		bUnZipSubDir		- [in/opt] 是否解压子目录 
		bFirstEnter		- [in/opt] 
	*/
	
	if ( NULL == lpZipFile || NULL == lpUnZipFileDir )
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	
	if ( MyFileExists(lpZipFile) )
	{
		//
		//	解压
		//
		if ( UnZip( lpZipFile, lpUnZipFileDir ) )
		{
			if ( ! bFirstEnter )
			{
				if ( DeleteFile( lpZipFile ) )
				{
					if ( m_pfnCallbackDetailInfo )
					{
						m_pfnCallbackDetailInfo( TRUE, OPERATOR_DELETE, lpZipFile, lpUnZipFileDir, ERROR_SUCCESS );
					}
				}
				else
				{
					if ( m_pfnCallbackDetailInfo )
					{
						m_pfnCallbackDetailInfo( FALSE, OPERATOR_DELETE, lpZipFile, lpUnZipFileDir, GetLastError() );
					}
					bRet = FALSE;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/**
 *	判断文件是否存在
 */ 
BOOL CDeZip::MyFileExists( LPCTSTR lpszFile )
{
	BOOL bRet = FALSE;
	if ( lpszFile && _tcslen(lpszFile) > 0 )
	{
		FILE * fp = fopen( lpszFile, "r" );
		if( fp )
		{
			fclose( fp );
			bRet = TRUE;
		}
	}
	return bRet;
}


/**
 *	替换字符串中指定的字符为新的字符
 */
BOOL CDeZip::StrReplace( char *lpSrc, char chOld, char chNew )
{
	//
	//	lpSrc	- [in] 指定字符串
	//	chOld	- [in] 指定字符
	//	chNew	- [in] 被替换为的新字符
	//

	BOOL bRet	= FALSE;
	char *p		= NULL;
	if ( lpSrc && chOld )
	{
		bRet = TRUE;
		// ..
		p = lpSrc;
		if ( NULL == chNew )
		{
			while( p && *p )
			{
				if ( *p == chOld )
				{
					memmove( p, p + 1, strlen(p + 1) + 1 );
				}
				else
					p++;
			}
		}
		else
		{
			while( p && *p )
			{
				if ( *p == chOld )
				{
					*p = chNew;
				}
				p++;
			}
		}
	}

	return bRet;
}


/**
 *	安全地创建多层目录
 */
BOOL CDeZip::MultiCreateDirectory( LPCTSTR lpcszDir )
{
	//
	//	lpcszDir	- [in] 需要创建的目录
	//

	BOOL bRet			= FALSE;
	TCHAR szDirTem[ MAX_URL ]	= {0};
	LPTSTR lpszMov			= NULL;

	_sntprintf( szDirTem, sizeof(szDirTem)-sizeof(TCHAR), "%s", lpcszDir );
	if ( '\\' == szDirTem[_tcslen(szDirTem)-sizeof(TCHAR)] )
	{
		szDirTem[_tcslen(szDirTem)-sizeof(TCHAR)] = 0;
		return MultiCreateDirectory( szDirTem );
	}

	// ..
	bRet = FALSE;
	while ( TRUE )
	{
		SetLastError( 0 );
		if ( CreateDirectory( szDirTem, NULL ))
		{
			//	创建文件夹成功，直接返回即可
			bRet = TRUE;
			break;
		}
		else
		{
			if ( ERROR_PATH_NOT_FOUND == GetLastError() )
			{
				//	发现父亲目录不存在，则直接取出父路径，递归创建之
				lpszMov = _tcsrchr( szDirTem, '\\' );
				if ( lpszMov )
				{
					*lpszMov = '\0';
					bRet = MultiCreateDirectory( szDirTem );
				}
			}
			else
			{
				//	如果不是“文件夹不存在”的错误，则直接跳出返回
				break;
			}
		}
	}

	return bRet;
}


/**
 *	解开一个 Zip 压缩包到指定目录下
 */
BOOL CDeZip::UnZip( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir )
{
	//
	//	lpZipFile	- [in] Zip 压缩包的全路径
	//	lpUnZipFileDir	- [in] 文件解压到目录
	//

	ZIPENTRY ze;
	BOOL bRet			= FALSE;
	INT nItemsNum			= 0;
	INT i				= 0;
	TCHAR szZipFilePath[ MAX_URL ]	= {0};

	HZIP hz = OpenZip( (PVOID)lpZipFile, 0, ZIP_FILENAME );
	if ( hz )
	{
		GetZipItem( hz, -1, &ze );
		nItemsNum = ze.index;

		for ( i = 0; i < nItemsNum; i++ )
		{
			GetZipItem( hz, i, &ze );

			if ( '\\' == lpUnZipFileDir[ _tcslen(lpUnZipFileDir) - sizeof(TCHAR) ] )
			{
				_sntprintf( szZipFilePath, sizeof(szZipFilePath)-sizeof(TCHAR), "%s%s", lpUnZipFileDir, ze.name );
			}
			else
			{
				_sntprintf( szZipFilePath, sizeof(szZipFilePath)-sizeof(TCHAR), "%s\\%s", lpUnZipFileDir, ze.name );
			}
			StrReplace( szZipFilePath, '/', '\\' );
			char * psSearch = _tcsrchr( szZipFilePath, '\\' );
			if ( psSearch )
			{
				//	先强制将完整的文件路径搞成其所在目录
				*psSearch = '\0';

				//	创建多层文件夹
				MultiCreateDirectory( szZipFilePath );

				//	再恢复其完整文件路径
				*psSearch = '\\';
			}

			//
			//	判断是否是文件
			//
			if ( '\\' != szZipFilePath[ _tcslen(szZipFilePath) - sizeof(TCHAR) ] )
			{
				//	从压缩包里提出文件到硬盘
				VerifyZip( i, hz, szZipFilePath );
				if ( m_pfnCallbackDetailInfo )
				{
					m_pfnCallbackDetailInfo( TRUE, OPERATOR_UNCOMPRESS, lpZipFile, szZipFilePath , ERROR_SUCCESS );
				}
			}
		}

		CloseZip( hz );
		bRet = TRUE;
	}

	return bRet;
}

/**
 *	...
 */
VOID CDeZip::VerifyZip(int nCount, HZIP hz, LPCTSTR lpszFile)
{
	if ( ! hz )
	{
		return;
	}
	if ( ! lpszFile )
	{
		return;
	}

	ZIPENTRY ze;
	INT index			= -1;
	ZRESULT zr			= 0;
	TCHAR szTargetName[ MAX_URL ]	= {0};

	memset( &ze, 0, sizeof(ze) );
	zr = FindZipItem( hz, lpszFile, TRUE, &index, &ze );
	_sntprintf( szTargetName, sizeof(szTargetName)-sizeof(TCHAR), "%s", lpszFile );
	::DeleteFile( szTargetName );
	zr = UnzipItem( hz, index, szTargetName, 0, ZIP_FILENAME );
}