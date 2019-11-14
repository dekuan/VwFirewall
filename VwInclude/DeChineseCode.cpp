// DeChineseCode.cpp: implementation of the CDeChineseCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeChineseCode.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeChineseCode::CDeChineseCode()
{
}
CDeChineseCode::~CDeChineseCode()
{
}

BOOL CDeChineseCode::UTF8ToUnicode( wchar_t * pOut, CHAR * pszText )
{
	CHAR * uChar = (CHAR*)pOut;

	if ( NULL == pOut || NULL == pszText )
	{
		return FALSE;
	}

	uChar[ 1 ] = ( ( pszText[ 0 ] & 0x0F ) << 4 ) + ( ( pszText[ 1 ] >> 2 ) & 0x0F );
	uChar[ 0 ] = ( ( pszText[ 1 ] & 0x03 ) << 6 ) + ( pszText[ 2 ] & 0x3F );

	return TRUE;
}

BOOL CDeChineseCode::UnicodeToUTF8( CHAR * pOut, wchar_t * pszText )
{
	if ( NULL == pszText || NULL == pOut )
	{
		return FALSE;
	}

	//	注意 WCHAR 高低字的顺序，低字节在前，高字节在后
	CHAR * pChar = (CHAR*)pszText;

	pOut[ 0 ] = ( 0xE0 | ( ( pChar[ 1 ] & 0xF0 ) >> 4 ) );
	pOut[ 1 ] = ( 0x80 | ( ( pChar[ 1 ] & 0x0F ) << 2 ) ) + ( ( pChar[0] & 0xC0 ) >> 6 );
	pOut[ 2 ] = ( 0x80 | ( pChar[ 0 ] & 0x3F ) );

	return TRUE;
}

BOOL CDeChineseCode::UnicodeToGB2312( CHAR * pOut, wchar_t uData )
{
	if ( NULL == pOut )
	{
		return FALSE;
	}

	if ( WideCharToMultiByte( CP_ACP,NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDeChineseCode::Gb2312ToUnicode( wchar_t * pOut, CHAR * pszGbBuffer )
{
	if ( NULL == pOut || NULL == pszGbBuffer )
	{
		return FALSE;
	}

	if ( ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pszGbBuffer, 2, pOut, 1 ) > 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDeChineseCode::GB2312ToUTF8( string & pOut, CHAR * pszText, INT nInputLen )
{
	BOOL bRet;
	CHAR szBuf[ 4 ];
	CHAR * pstTemp;
	INT  nTempLen;
	wchar_t wcBuffer;
	USHORT uTemp;
	INT i	= 0;
	INT j	= 0;

	if ( NULL == pszText || 0 == nInputLen )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		nTempLen	= nInputLen * 3;
		pstTemp		= new CHAR[ nTempLen ];
		if ( pstTemp )
		{
			memset( szBuf, 0, 4 );
			memset( pstTemp, 0, nTempLen );

			while ( i < nInputLen )
			{
				//	如果是英文直接复制就可以
				if ( *( pszText + i ) >= 0 )
				{
					pstTemp[ j++ ] = pszText[ i++ ];
				}
				else
				{
					wcBuffer = NULL;
					Gb2312ToUnicode( &wcBuffer, pszText+i );	
					UnicodeToUTF8( szBuf, &wcBuffer );

					uTemp	= 0;
					uTemp	= pstTemp[ j ]		= szBuf[ 0 ];
					uTemp	= pstTemp[ j + 1 ]	= szBuf[ 1 ];
					uTemp	= pstTemp[ j + 2 ]	= szBuf[ 2 ]; 

					j += 3;
					i += 2;
				}
			}

			pstTemp[ j ] = '\0';

			//	返回结果
			pOut	= pstTemp; 
			delete [] pstTemp;
			pstTemp	= NULL;

			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return bRet;
}

BOOL CDeChineseCode::UTF8ToGB2312( CHAR * pszOutBuf, CHAR * pszText, INT nInputLen )
{
	BOOL bRet;
	CHAR sztemp[4];
	WCHAR wcTemp;
	INT i;
	INT j;

	if ( NULL == pszOutBuf || NULL == pszText || 0 == nInputLen )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		memset( sztemp, 0, sizeof(sztemp) );
		i	= 0;
		j	= 0;

		while( i < nInputLen )
		{
			if ( pszText[ i ] > 0 )
			{
				pszOutBuf[ j++ ] = pszText[ i++ ]; 
			}
			else 
			{
				UTF8ToUnicode( &wcTemp, pszText + i );
				UnicodeToGB2312( sztemp, wcTemp );

				pszOutBuf[ j ]	= sztemp[ 0 ];
				pszOutBuf[ j + 1 ]	= sztemp[ 1 ];

				i += 3; 
				j += 2; 
			}
		}

		pszOutBuf[ j ]	= '\0';
		bRet		= TRUE;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return bRet;
}

/**
 *	判断字符串是否是 UTF-8 字符串
 *	刘军民的方法
 */
BOOL CDeChineseCode::IsUtf8_byLiuJunMing( LPCTSTR lpcszString, UINT uLength )
{
	UINT i;
	UINT m;
	UINT uCount;
	UCHAR ucChar;

	if ( NULL == lpcszString || 0 == uLength )
	{
		return FALSE;
	}

	__try
	{
		for ( i = 0; i < uLength; )
		{
			ucChar = lpcszString[ i ];
			if ( ucChar >= 0 && ucChar < 0x7F )
			{
				i ++;
				continue;
			}
			if ( ucChar < 0xC0 || ucChar > 0xFD )
			{
				return FALSE;
			}
			uCount = ucChar > 0xFC ? 5 : ucChar > 0xF8 ? 4 : ucChar > 0xF0 ? 3 : ucChar > 0xE0 ? 2 : 1;
			if ( i + uCount > uLength )
			{
				return FALSE;
			}

			i ++;

			for ( m = 0; m < uCount; m ++ )
			{
				if ( lpcszString[ i ] < 0x80 || lpcszString[ i ] > 0xBF )
				{
					return FALSE;
				}
				i ++;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return TRUE;
}

/**
 *	判断字符串是否是 UTF-8 字符串
 */
BOOL CDeChineseCode::IsUTF8Text( LPCTSTR lpcszBuffer, LONG lnLength )
{
	BOOL bRet;
	UCHAR * ucStart;
	UCHAR * ucEnd;

	if ( NULL == lpcszBuffer || 0 == lnLength )
	{
		return FALSE;
	}

	bRet	= TRUE;
	ucStart	= (UCHAR*)lpcszBuffer;
	ucEnd	= (UCHAR*)lpcszBuffer + lnLength;

	while ( ucStart < ucEnd )
	{
		if ( *ucStart < 0x80 )
		{
			//	(10000000): 值小于0x80的为ASCII字符
			ucStart++;
		}
		else if ( *ucStart < (0xC0) )
		{
			//	(11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
			bRet = FALSE;
			break;
		}
		else if ( *ucStart < (0xE0) ) 
		{
			//	(11100000): 此范围内为2字节UTF-8字符
			if ( ucStart >= ucEnd - 1 )
			{
				break;
			}
			if ( ( ucStart[ 1 ] & (0xC0)) != 0x80 )
			{
				bRet = FALSE;
				break;
			}
			ucStart += 2;
		}
		else if ( *ucStart < (0xF0) )
		{
			//	(11110000): 此范围内为3字节UTF-8字符
			if (ucStart >= ucEnd - 2)
			{
				break;
			}
			if ( ( ucStart[ 1 ] & (0xC0) ) != 0x80 || ( ucStart[ 2 ] & (0xC0) ) != 0x80 )
			{
				bRet = FALSE;
				break;
			}
			ucStart += 3;
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}










BOOL CDeChineseCode::IsUTF8( const char * pzInfo )
{
	MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, pzInfo, -1, NULL, 0 );
	if ( ERROR_NO_UNICODE_TRANSLATION == GetLastError() )
	{
		return FALSE;
	}
	//	判断是否是gb2312,只要把CP_UTF8用936代替即可.
	return TRUE;
}

/***************************************************
 * Function:       IsGB2312
 * Description:    判断是否gb2312编码字符
 * Input:          pzInfo:待判断字符
 * Output:         
 * Return:         
 * Others:         
 ***************************************************/
BOOL CDeChineseCode::IsGB2312( const char * pzInfo )
{
	MultiByteToWideChar( 936, MB_ERR_INVALID_CHARS, pzInfo, -1, NULL, 0 );
	if ( ERROR_NO_UNICODE_TRANSLATION == GetLastError() )
	{
		return FALSE;
	}
	//	判断是否是 CP_UTF8, 只要把 936 用 CP_UTF8 代替即可.
	return TRUE;
}

/***************************************************
* Function:       IsGB
* Description:    是否gb2312
* Input:          pText：待判断字符
* Output:         
* Return:         
* Others:         
***************************************************/
int CDeChineseCode::IsGB( char *pText )
{
	unsigned char *sqChar = (unsigned char *)pText;
	if (sqChar[0] >= 0xa1)
	{
		if (sqChar[0] == 0xa3)
		{
			return 1;//全角字符
		}
		else
		{
			return 2;//汉字
		}
	}
	else
	{
		return 0;//英文、数字、英文标点
	}
}

/***************************************************
* Function:       IsChinese
* Description:    是否有中文
* Input:          pzInfo：待判断字符
* Output:         
* Return:         
* Others:         
***************************************************/
bool CDeChineseCode::IsChinese( const char *pzInfo )
{
	int i;
	char *pText = (char*)pzInfo;
	while (*pText != '\0')
	{
		i = IsGB(pText);
		switch(i)
		{
		case 0:
			pText++;
			break;
		case 1:
			pText++;
			pText++;
			break;
		case 2:
			pText++;
			pText++;
			return TRUE;
			break;
		}
	}
	return false;
}






//////////////////////////////////////////////////////////////////////////
//	private



/*
inline BYTE CDeChineseCode::toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}
inline BYTE CDeChineseCode::toByte(const BYTE &x)
{
	return x > 57? x - 55: x - 48;
}
CString CDeChineseCode::URLDecode(CString sIn)
{
	CString sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen);
	
	if(pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;
		// do encoding
		while (*pInTmp)
		{
			if('%'==*pInTmp)
			{
				pInTmp++;
				*pOutTmp++ = (toByte(*pInTmp)%16<<4) + toByte(*(pInTmp+1))%16;//高4位+低4位
				pInTmp++;
			}
			else if('+'==*pInTmp)
				*pOutTmp++ = ' ';
			else
				*pOutTmp++ = *pInTmp;
			pInTmp++;
		}
		*pOutTmp = '\0';
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	
	return sOut;
}

CString CDeChineseCode::URLEncode(CString sIn)
{
	CString sOut;
	const int nLen = sIn.GetLength() + 1;
	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen*3);
	
	if(pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;
		// do encoding
		while (*pInTmp)
		{
			if(isalnum(*pInTmp) || '-'==*pInTmp || '_'==*pInTmp || '.'==*pInTmp)
				*pOutTmp++ = *pInTmp;
			else if(isspace(*pInTmp))
				*pOutTmp++ = '+';
			else
			{
				*pOutTmp++ = '%';
				*pOutTmp++ = toHex(*pInTmp>>4);//高4位
				*pOutTmp++ = toHex(*pInTmp%16);//低4位
			}
			pInTmp++;
		}
		*pOutTmp = '\0';
		sOut.ReleaseBuffer();
	}
	sIn.ReleaseBuffer();
	
	return sOut;
}
*/