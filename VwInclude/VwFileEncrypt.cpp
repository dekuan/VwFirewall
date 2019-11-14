
#include "stdafx.h"
#include "VwFileEncrypt.h"


CVwFileEncrypt::CVwFileEncrypt()
{
}

CVwFileEncrypt::~CVwFileEncrypt()
{
}

/**
 *	@ Public
 *	检测是否是已加密文件
 */
BOOL CVwFileEncrypt::IsEncryptedFile( LPCTSTR lpcszFilePath )
{
	if ( NULL == lpcszFilePath || NULL == lpcszFilePath )
	{
		return FALSE;
	}

	BOOL   bRet			= FALSE;
	HANDLE hfile			= NULL;
	UINT   uFileLen			= 0;
	DWORD  dwDummy			= 0;
	BYTE   btHeader[ VWFILEENCRYPT_HEADER_LENGTH ]	= {0};
	HANDLE hmap			= NULL;
	BYTE * pView			= NULL;
	STVWENCRYPTEDFILEHEADER stHeader;
	
	hfile = CreateFile( lpcszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH )
			{
				uFileLen = CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH;
			}
			
			hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					//	读取头信息
					memcpy( btHeader, pView, VWFILEENCRYPT_HEADER_LENGTH );
					if ( IsEncryptedFileByByte( btHeader ) && GetHeaderInfo( btHeader, stHeader ) )
					{
						bRet = TRUE;
					}

					//	close view
					UnmapViewOfFile( pView );
				}

				//	close map
				CloseHandle( hmap );
			}
		}

		//	close handle
		CloseHandle( hfile );
	}

	return bRet;
}

/**
 *	@ Public
 *	制作加密数据文件
 */
BOOL CVwFileEncrypt::EncryptFile( LPCTSTR lpcszSrcFilePath, LPCTSTR lpcszDstFilePath )
{
	if ( NULL == lpcszSrcFilePath || NULL == lpcszDstFilePath )
	{
		return FALSE;
	}
	
	BOOL   bRet			= FALSE;
	HANDLE hfile			= NULL;
	HANDLE hFileEnc			= NULL;
	UINT   uFileLen			= 0;
	DWORD  dwDummy			= 0;
	BYTE   btHeader[ VWFILEENCRYPT_HEADER_LENGTH ]	= {0};
	PUCHAR lpszBuffer		= NULL;
	PUCHAR lpszBufferOut		= NULL;
	ULONG  dwKey			= 89526475;
	DWORD  dwBufferLen		= 0;
	HANDLE hmap			= NULL;
	BYTE * pView			= NULL;
	STVWENCRYPTEDFILEHEADER stHeader;


	hfile = CreateFile( lpcszSrcFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH )
			{
				uFileLen = CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH;
			}

			hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					//	读取头信息
					memcpy( btHeader, pView, VWFILEENCRYPT_HEADER_LENGTH );
					if ( IsEncryptedFileByByte( btHeader ) && GetHeaderInfo( btHeader, stHeader ) )
					{
						//	此为加密后文件，需要解密
						lpszBuffer	= pView + VWFILEENCRYPT_HEADER_LENGTH;
						dwBufferLen	= uFileLen - VWFILEENCRYPT_HEADER_LENGTH;

						lpszBufferOut = new UCHAR[ dwBufferLen ];
						if ( lpszBufferOut )
						{
							memcpy( lpszBufferOut, lpszBuffer, dwBufferLen );
							EncryptBuffer( (PUCHAR)lpszBufferOut, dwBufferLen, stHeader.dwKey );

							// ..
							hFileEnc = CreateFile( lpcszDstFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
							if ( INVALID_HANDLE_VALUE != hFileEnc )
							{
								bRet = TRUE;

								//	写数据
								SetFilePointer( hFileEnc, 0, 0, FILE_BEGIN );
								WriteFile( hFileEnc, lpszBufferOut, dwBufferLen, &dwDummy, NULL );

								//	..
								CloseHandle( hFileEnc );
							}
							
							delete [] lpszBufferOut;
						}
					}
					else
					{
						//	此为明文，需要加密
						lpszBuffer	= pView;
						dwBufferLen	= uFileLen;

						lpszBufferOut = new UCHAR[ dwBufferLen ];
						if ( lpszBufferOut )
						{
							memcpy( lpszBufferOut, lpszBuffer, dwBufferLen );
							EncryptBuffer( (PUCHAR)lpszBufferOut, dwBufferLen, dwKey );

							// ..
							hFileEnc = CreateFile( lpcszDstFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
							if ( INVALID_HANDLE_VALUE != hFileEnc )
							{
								bRet = TRUE;

								stHeader.dwKey	= dwKey;

								//	写入头
								SetFilePointer( hFileEnc, 0, 0, FILE_BEGIN );
								WriteFile( hFileEnc, &stHeader, VWFILEENCRYPT_HEADER_LENGTH, &dwDummy, NULL );

								//	写数据
								SetFilePointer( hFileEnc, VWFILEENCRYPT_HEADER_LENGTH, 0, FILE_BEGIN);
								WriteFile( hFileEnc, lpszBufferOut, dwBufferLen, &dwDummy, NULL );

								//	..
								CloseHandle( hFileEnc );
							}

							delete [] lpszBufferOut;
						}
					}

					//	close view
					UnmapViewOfFile( pView );
				}

				//	close map
				CloseHandle( hmap );
			}
		}

		//	close handle
		CloseHandle( hfile );
	}

	return bRet;
}

INT CVwFileEncrypt::myrand( unsigned int * pSeed )
{
	return ((( *pSeed = (*pSeed) * 214013L + 2531011L) >> 16) & 0x7fff);
}

VOID CVwFileEncrypt::EncryptBuffer( BYTE * pBuf, int nLen, DWORD dwKey )
{
	int i;
	unsigned int nSeed;
	nSeed = dwKey;

	for ( i = 0; i < nLen; i++ )
	{
		pBuf[i] = (BYTE)( pBuf[i] ^ ( (BYTE)( myrand(&nSeed) + BASE_PLUS_VALUE ) ) ^ BASE_XOR_VALUE );
	}
}


/**
 *	判断是否是已经被加密后的文件
 */
BOOL CVwFileEncrypt::IsEncryptedFileByByte( BYTE * pbHeaderBuffer )
{
	if ( NULL == pbHeaderBuffer )
	{
		return FALSE;
	}
	LPTSTR lpszHeaderFlag = (LPTSTR)( pbHeaderBuffer + sizeof(DWORD)*3 );
	if ( lpszHeaderFlag )
	{
		return strstr( lpszHeaderFlag, VWFILEENCRYPT_HEADER_FLAG ) ? TRUE : FALSE;
	}
	return FALSE;
}

/**
 *	读取头信息
 */
BOOL CVwFileEncrypt::GetHeaderInfo( BYTE * pbHeaderBuffer, STVWENCRYPTEDFILEHEADER & stHeader )
{
	if ( NULL == pbHeaderBuffer )
	{
		return FALSE;
	}
	STVWENCRYPTEDFILEHEADER * pstHeader = (STVWENCRYPTEDFILEHEADER*)pbHeaderBuffer;
	if ( pstHeader )
	{
		stHeader = *pstHeader;
		return TRUE;
	}
	return FALSE;
}