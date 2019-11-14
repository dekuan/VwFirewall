#ifndef __DEFILEENCRYPT_HEADER__
#define __DEFILEENCRYPT_HEADER__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



/**
 *	const value
 */
#define BASE_XOR_VALUE		(0x04020626)
#define BASE_PLUS_VALUE		(0x00970702)
#define DEF_ENCRYPT_KEY		(0x03070201)

#define DEFILEENCRYPT_HEADER_LENGTH		64
#define DEFILEENCRYPT_HEADER_FLAG		"de_encrypted_file"

#define CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )

/**
 *	加密文件头结构，保证必须是 64 个字节
 */
typedef struct tagDeEncryptedFileHeader
{
	tagDeEncryptedFileHeader()
	{
		memset( this, 0, sizeof(tagDeEncryptedFileHeader) );

		dwVersion	= 1;
		dwBuild		= 1001;
		dwKey		= DEF_ENCRYPT_KEY;
		strcpy( szHeaderFlag, DEFILEENCRYPT_HEADER_FLAG );
	}

	DWORD  dwVersion;
	DWORD  dwBuild;
	DWORD  dwKey;
	TCHAR  szHeaderFlag[ 32 ];
	TCHAR  szOther[ 20 ];

}STDEENCRYPTEDFILEHEADER, *PSTDEENCRYPTEDFILEHEADER;




/**
 *	class of CDeFileEncrypt
 */
class CDeFileEncrypt
{
public:
	CDeFileEncrypt();
	virtual ~CDeFileEncrypt();

	//	是否是加密后的文件
	BOOL IsEncryptedFile( LPCTSTR lpcszFilePath );

	//	从指定文件中得到解密后的内容
	BOOL GetDecodedBufferFromFile( IN LPCTSTR lpcszFilePath, OUT LPTSTR lpszDecodedBuffer, IN DWORD dwSize );

	//	保存加密后的 buffer 到文件
	BOOL SaveEncryptedBufferToFile( IN LPCTSTR lpcszBuffer, IN LPCTSTR lpcszFilePath );

	//	加密/解密 文件
	BOOL EncryptFile( LPCTSTR lpcszSrcFilePath, LPCTSTR lpcszDstFilePath );

	//	加密/解密 buffer
	VOID EncryptBuffer( BYTE * pBuf, int nLen, DWORD dwKey );

private:

	BOOL IsEncryptedFileByByte( BYTE * pbHeaderBuffer );
	BOOL GetHeaderInfo( BYTE * pbHeaderBuffer, STDEENCRYPTEDFILEHEADER & stHeader );

	INT  myrand( unsigned int * pSeed );
	ULONG GetCRC( PUCHAR lpData, ULONG uLength );

private:
	DWORD	m_dwKey;
};




#endif	// __DEFILEENCRYPT_HEADER__