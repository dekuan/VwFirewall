
#include "stdafx.h"
#include "DeFileEncrypt.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


CDeFileEncrypt::CDeFileEncrypt()
{
	m_dwKey = 89526475;
}

CDeFileEncrypt::~CDeFileEncrypt()
{
}

/**
 *	@ Public
 *	检测是否是已加密文件
 */
BOOL CDeFileEncrypt::IsEncryptedFile( LPCTSTR lpcszFilePath )
{
	if ( NULL == lpcszFilePath || NULL == lpcszFilePath )
	{
		return FALSE;
	}

	BOOL   bRet			= FALSE;
	HANDLE hfile			= NULL;
	UINT   uFileLen			= 0;
	DWORD  dwDummy			= 0;
	BYTE   btHeader[ DEFILEENCRYPT_HEADER_LENGTH ]	= {0};
	HANDLE hmap			= NULL;
	BYTE * pView			= NULL;
	STDEENCRYPTEDFILEHEADER stHeader;


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
					memcpy( btHeader, pView, DEFILEENCRYPT_HEADER_LENGTH );
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
 *	将加密文件解密到一个缓内存冲区
 */
BOOL CDeFileEncrypt::GetDecodedBufferFromFile( IN LPCTSTR lpcszFilePath, OUT LPTSTR lpszDecodedBuffer, IN DWORD dwSize )
{
	if ( NULL == lpcszFilePath || NULL == lpszDecodedBuffer || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL   bRet			= FALSE;
	HANDLE hfile			= NULL;
	HANDLE hFileEnc			= NULL;
	UINT   uFileLen			= 0;
	DWORD  dwDummy			= 0;
	BYTE   btHeader[ DEFILEENCRYPT_HEADER_LENGTH ]	= {0};
	PUCHAR lpszBuffer		= NULL;
	PUCHAR lpszBufferOut		= NULL;
	//ULONG  dwKey			= 89526475;
	DWORD  dwBufferLen		= 0;
	HANDLE hmap			= NULL;
	BYTE * pView			= NULL;
	STDEENCRYPTEDFILEHEADER stHeader;


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
					memcpy( btHeader, pView, DEFILEENCRYPT_HEADER_LENGTH );
					if ( IsEncryptedFileByByte( btHeader ) && GetHeaderInfo( btHeader, stHeader ) )
					{
						//	此为加密后文件，需要解密
						lpszBuffer	= pView + DEFILEENCRYPT_HEADER_LENGTH;
						dwBufferLen	= uFileLen - DEFILEENCRYPT_HEADER_LENGTH;

						lpszBufferOut = new UCHAR[ dwBufferLen ];
						if ( lpszBufferOut )
						{
							bRet = TRUE;

							memcpy( lpszBufferOut, lpszBuffer, dwBufferLen );
							EncryptBuffer( (PUCHAR)lpszBufferOut, dwBufferLen, stHeader.dwKey );

							//	将解密后的内容，保存到返回的内存缓存区
							memcpy( lpszDecodedBuffer, lpszBufferOut, min( dwSize, dwBufferLen ) );

							delete [] lpszBufferOut;
							lpszBufferOut = NULL;
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

/**
 *	@ Public
 *	加密内存并保存到文件
 */
BOOL CDeFileEncrypt::SaveEncryptedBufferToFile( IN LPCTSTR lpcszBuffer, IN LPCTSTR lpcszFilePath )
{
	//
	//	lpcszBuffer	- [in] 明文输入
	//	lpcszFilePath	- [in] 要保存到的文件地址
	//	RETURN		- TRUE / FALSE
	//

	BOOL  bRet		= FALSE;
	DWORD dwBufferLen	= 0;
	PUCHAR lpszBufTemp	= NULL;
	STDEENCRYPTEDFILEHEADER stHeader;
	HANDLE hFileEnc		= NULL;
	DWORD dwDummy		= 0;


	if ( NULL == lpcszBuffer || NULL == lpcszFilePath )
	{
		return FALSE;
	}

	//	明文的长度
	dwBufferLen	= _tcslen(lpcszBuffer);

	//	分配临时缓冲区，用于加密明文
	lpszBufTemp = new UCHAR[ dwBufferLen ];
	if ( lpszBufTemp )
	{
		memcpy( lpszBufTemp, lpcszBuffer, dwBufferLen );
		EncryptBuffer( (PUCHAR)lpszBufTemp, dwBufferLen, m_dwKey );

		//	保存
		hFileEnc = CreateFile( lpcszFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFileEnc )
		{
			bRet = TRUE;

			stHeader.dwKey	= m_dwKey;
			
			//	写入头
			SetFilePointer( hFileEnc, 0, 0, FILE_BEGIN );
			WriteFile( hFileEnc, &stHeader, DEFILEENCRYPT_HEADER_LENGTH, &dwDummy, NULL );
			
			//	写数据
			SetFilePointer( hFileEnc, DEFILEENCRYPT_HEADER_LENGTH, 0, FILE_BEGIN);
			WriteFile( hFileEnc, lpszBufTemp, dwBufferLen, &dwDummy, NULL );
			
			//	..
			CloseHandle( hFileEnc );
		}

		delete [] lpszBufTemp;
		lpszBufTemp = NULL;
	}

	return bRet;
}


/**
 *	@ Public
 *	制作加密数据文件
 */
BOOL CDeFileEncrypt::EncryptFile( LPCTSTR lpcszSrcFilePath, LPCTSTR lpcszDstFilePath )
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
	BYTE   btHeader[ DEFILEENCRYPT_HEADER_LENGTH ]	= {0};
	PUCHAR lpszBuffer		= NULL;
	PUCHAR lpszBufferOut		= NULL;
//	ULONG  dwKey			= 89526475;
	DWORD  dwBufferLen		= 0;
	HANDLE hmap			= NULL;
	BYTE * pView			= NULL;
	STDEENCRYPTEDFILEHEADER stHeader;


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
					memcpy( btHeader, pView, DEFILEENCRYPT_HEADER_LENGTH );
					if ( IsEncryptedFileByByte( btHeader ) && GetHeaderInfo( btHeader, stHeader ) )
					{
						//	此为加密后文件，需要解密
						lpszBuffer	= pView + DEFILEENCRYPT_HEADER_LENGTH;
						dwBufferLen	= uFileLen - DEFILEENCRYPT_HEADER_LENGTH;

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
							lpszBufferOut = NULL;
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
							EncryptBuffer( (PUCHAR)lpszBufferOut, dwBufferLen, m_dwKey );

							// ..
							hFileEnc = CreateFile( lpcszDstFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
							if ( INVALID_HANDLE_VALUE != hFileEnc )
							{
								bRet = TRUE;

								stHeader.dwKey	= m_dwKey;

								//	写入头
								SetFilePointer( hFileEnc, 0, 0, FILE_BEGIN );
								WriteFile( hFileEnc, &stHeader, DEFILEENCRYPT_HEADER_LENGTH, &dwDummy, NULL );

								//	写数据
								SetFilePointer( hFileEnc, DEFILEENCRYPT_HEADER_LENGTH, 0, FILE_BEGIN);
								WriteFile( hFileEnc, lpszBufferOut, dwBufferLen, &dwDummy, NULL );

								//	..
								CloseHandle( hFileEnc );
							}

							delete [] lpszBufferOut;
							lpszBufferOut = NULL;
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

INT CDeFileEncrypt::myrand( unsigned int * pSeed )
{
	return ((( *pSeed = (*pSeed) * 214013L + 2531011L) >> 16) & 0x7fff);
}

VOID CDeFileEncrypt::EncryptBuffer( BYTE * pBuf, int nLen, DWORD dwKey )
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
BOOL CDeFileEncrypt::IsEncryptedFileByByte( BYTE * pbHeaderBuffer )
{
	if ( NULL == pbHeaderBuffer )
	{
		return FALSE;
	}
	LPTSTR lpszHeaderFlag = (LPTSTR)( pbHeaderBuffer + sizeof(DWORD)*3 );
	if ( lpszHeaderFlag )
	{
		return strstr( lpszHeaderFlag, DEFILEENCRYPT_HEADER_FLAG ) ? TRUE : FALSE;
	}
	return FALSE;
}

/**
 *	读取头信息
 */
BOOL CDeFileEncrypt::GetHeaderInfo( BYTE * pbHeaderBuffer, STDEENCRYPTEDFILEHEADER & stHeader )
{
	if ( NULL == pbHeaderBuffer )
	{
		return FALSE;
	}
	STDEENCRYPTEDFILEHEADER * pstHeader = (STDEENCRYPTEDFILEHEADER*)pbHeaderBuffer;
	if ( pstHeader )
	{
		stHeader = *pstHeader;
		return TRUE;
	}
	return FALSE;
}