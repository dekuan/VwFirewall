#ifndef __VWFILEENCRYPT_HEADER__
#define __VWFILEENCRYPT_HEADER__


#include "VwConst.h"


/**
 *	const value
 */
#define BASE_XOR_VALUE		(0x04020626)
#define BASE_PLUS_VALUE		(0x00970702)
#define DEF_ENCRYPT_KEY		(0x03070201)

#define VWFILEENCRYPT_HEADER_LENGTH		64
#define VWFILEENCRYPT_HEADER_FLAG		"vw_encrypted_file_header"

#define CONST_VWFILEENCRYPT_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )

/**
 *	加密文件头结构，保证必须是 64 个字节
 */
typedef struct tagVwEncryptedFileHeader
{
	tagVwEncryptedFileHeader()
	{
		memset( this, 0, sizeof(tagVwEncryptedFileHeader) );

		dwVersion	= 1;
		dwBuild		= 1001;
		dwKey		= DEF_ENCRYPT_KEY;
		strcpy( szHeaderFlag, VWFILEENCRYPT_HEADER_FLAG );
	}

	DWORD  dwVersion;
	DWORD  dwBuild;
	DWORD  dwKey;
	TCHAR  szHeaderFlag[ 32 ];
	TCHAR  szOther[ 20 ];

}STVWENCRYPTEDFILEHEADER, *PSTVWENCRYPTEDFILEHEADER;




/**
 *	class of CVwFileEncrypt
 */
class CVwFileEncrypt
{
public:
	CVwFileEncrypt();
	virtual ~CVwFileEncrypt();

	BOOL IsEncryptedFile( LPCTSTR lpcszFilePath );
	BOOL EncryptFile( LPCTSTR lpcszSrcFilePath, LPCTSTR lpcszDstFilePath );
	VOID EncryptBuffer( BYTE * pBuf, int nLen, DWORD dwKey );

private:

	BOOL IsEncryptedFileByByte( BYTE * pbHeaderBuffer );
	BOOL GetHeaderInfo( BYTE * pbHeaderBuffer, STVWENCRYPTEDFILEHEADER & stHeader );

	INT  myrand( unsigned int * pSeed );
	ULONG GetCRC( PUCHAR lpData, ULONG uLength );

};




#endif	// __VWFILEENCRYPT_HEADER__