// DeLicence.cpp: implementation of the CDeLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeLicence.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include <nb30.h>
#pragma comment( lib, "netapi32.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "md5.h"


/**
 *	结构体定义
 */
typedef struct tagAstat
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[ 30 ];
}STASTAT, * PSTASTAT;

//	"c:\\"
static TCHAR g_szDeLicence_CDisk[] =
{
	-100, -59, -93, -93, 0
};

//	"%s|%s|%s"
static TCHAR g_szDeLicence_fmt1[] =
{
	-38, -116, -125, -38, -116, -125, -38, -116, 0
};

//	"%s-%s"
static TCHAR g_szDeLicence_FmtRegCode[] =
{
	-38, -116, -46, -38, -116, 0
};

//	"%s|%s"
static TCHAR g_szDeLicence_FmtActCode[] =
{
	-38, -116, -125, -38, -116, 0
};

//	"%02x-%02x-%02x-%02x-%02x-%02x"
static TCHAR g_szDeLicence_FmtMacAddress[] =
{
	-38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38, -49, -51, -121, -46, -38,
	-49, -51, -121, -46, -38, -49, -51, -121, 0
};

//	"%s-%d-%08x%08x%08x%08x"
static TCHAR g_szDeLicence_FmtCpuId[] =
{
	-38, -116, -46, -38, -101, -46, -38, -49, -57, -121, -38, -49, -57, -121, -38, -49, -57, -121, -38, -49, -57,
	-121, 0
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeLicence::CDeLicence()
{
	memset( & mb_stLcInfo, 0, sizeof(mb_stLcInfo) );

	//	默认是使用硬盘序列号 作为生成 gene 的参数
	m_bCreateGeneWithDiskSN	= TRUE;

	//
	//	解密 fmt
	//
	memset( m_szDecCDisk, 0, sizeof(m_szDecCDisk) );
	memcpy( m_szDecCDisk, g_szDeLicence_CDisk, min( sizeof(g_szDeLicence_CDisk), sizeof(m_szDecCDisk) ) );
	delib_xorenc( m_szDecCDisk );

	memset( m_szDecFmt1, 0, sizeof(m_szDecFmt1) );
	memcpy( m_szDecFmt1, g_szDeLicence_fmt1, min( sizeof(g_szDeLicence_fmt1), sizeof(m_szDecFmt1) ) );
	delib_xorenc( m_szDecFmt1 );

	memset( m_szDecFmtRegCode, 0, sizeof(m_szDecFmtRegCode) );
	memcpy( m_szDecFmtRegCode, g_szDeLicence_FmtRegCode, min( sizeof(g_szDeLicence_FmtRegCode), sizeof(m_szDecFmtRegCode) ) );
	delib_xorenc( m_szDecFmtRegCode );

	memset( m_szDecFmtActCode, 0, sizeof(m_szDecFmtActCode) );
	memcpy( m_szDecFmtActCode, g_szDeLicence_FmtActCode, min( sizeof(g_szDeLicence_FmtActCode), sizeof(m_szDecFmtActCode) ) );
	delib_xorenc( m_szDecFmtActCode );

	memset( m_szDecFmtMacAddress, 0, sizeof(m_szDecFmtMacAddress) );
	memcpy( m_szDecFmtMacAddress, g_szDeLicence_FmtMacAddress, min( sizeof(g_szDeLicence_FmtMacAddress), sizeof(m_szDecFmtMacAddress) ) );
	delib_xorenc( m_szDecFmtMacAddress );

	memset( m_szDecFmtCpuId, 0, sizeof(m_szDecFmtCpuId) );
	memcpy( m_szDecFmtCpuId, g_szDeLicence_FmtCpuId, min( sizeof(g_szDeLicence_FmtCpuId), sizeof(m_szDecFmtCpuId) ) );
	delib_xorenc( m_szDecFmtCpuId );


//	memset( mb_stLcInfo.szKey, 0, sizeof(mb_stLcInfo.szKey) );
//	memset( mb_stLcInfo.szGene, 0, sizeof(mb_stLcInfo.szGene) );

//	memset( mb_szRegCode, 0, sizeof(mb_szRegCode) );
//	memset( mb_stLcInfo.szActCode, 0, sizeof(mb_stLcInfo.szActCode) );
}
CDeLicence::~CDeLicence()
{
}




//////////////////////////////////////////////////////////////////////////
//	protected


/**
 *	@ protected
 *	初始化数据
 */
BOOL CDeLicence::InitData( IN HINSTANCE hCallerInstance, IN LPCTSTR lpcszKey, IN BOOL bCreateGeneWithDiskSN )
{
	memset( mb_stLcInfo.szKey, 0, sizeof(mb_stLcInfo.szKey) );
	if ( lpcszKey )
	{
		_sntprintf( mb_stLcInfo.szKey, sizeof(mb_stLcInfo.szKey)-sizeof(TCHAR), _T("%s"), lpcszKey );
	}

	//	是否使用硬盘序列号 作为生成 GENE 的参数
	m_bCreateGeneWithDiskSN	= bCreateGeneWithDiskSN;

	//	get gene
	GetGene( mb_stLcInfo.szGene, sizeof(mb_stLcInfo.szGene) );
	_tcsupr( mb_stLcInfo.szGene );

	return TRUE;
}

/**
 *	@ protected
 *	是否是正确的许可
 *	验证 ActCode 是否正确
 */
BOOL CDeLicence::IsValidLicence( LPCTSTR lpszActCode )
{
	//
	//	lpszActCode	- [in] 许可证书
	//	RETURN		- TRUE / FALSE
	//

	BOOL  bRet				= FALSE;
	TCHAR szGene[ MAX_PATH ]		= {0};
	TCHAR szActCodeInput[ MAX_PATH ]	= {0};
	TCHAR szActCodeCorrect[ MAX_PATH ]	= {0};

	_sntprintf
	(
		szActCodeInput,
		sizeof(szActCodeInput)-sizeof(TCHAR),
		_T("%s"),
		lpszActCode ? lpszActCode : mb_stLcInfo.szActCode
	);

	if ( _tcslen(mb_stLcInfo.szGene) )
	{
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );

		if ( GetActCode( szGene, szActCodeCorrect, sizeof(szActCodeCorrect) ) )
		{
			_tcslwr( szActCodeCorrect );
			_tcslwr( szActCodeInput );
			bRet = IsEqualString( szActCodeCorrect, szActCodeInput );
		}
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Public


/**
 *	@ public
 *	产生一个本机加密因子
 */
BOOL CDeLicence::GetGene( OUT LPTSTR lpszGene, IN DWORD dwSize )
{
	//
	//	lpszGene	- [out] 返回加密因子，32 位字符 MD5
	//	dwSize		- [in]  返回缓冲区的大小，一般为一个大于 32 的缓冲区
	//	RETURN
	//	算法：
	//		md5( m_szKey + NetworkCard Mac + DiskSerial )
	//

	TCHAR szHDiskSN[ MAX_PATH ]	= {0};
	TCHAR szCPUID[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};

	//	获取 cpuid
	GetCpuId( szCPUID, sizeof(szCPUID) );

	//	获取硬盘序列号
	if ( m_bCreateGeneWithDiskSN )
	{
		GetHDiskSerialNumber( szHDiskSN, sizeof(szHDiskSN) );
	}
	else
	{
		//	由于权限的问题，不能使用硬盘序列号作为参数来生成序列号
		memset( szHDiskSN, 0, sizeof(szHDiskSN) );
	}

	//	加密源文
	//	key + cpuid + HDiskSN
	_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmt1, mb_stLcInfo.szKey, szCPUID, szHDiskSN );
	_tcslwr( szSrc );

	//	..
	return GetStringMd5( szSrc, lpszGene, dwSize, 0 );
}

/**
 *	@ public
 *	获取 regcode
 */
BOOL CDeLicence::GetRegCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszRegCode, IN DWORD dwSize )
{
	//
	//	lpcszGene	- [in]  加密因子，一般为 NULL
	//	lpszRegCode	- [out] 返回许可证书
	//	dwSize		- [in]  返回缓冲区的大小
	//	RETURN
	//	算法：		md5( m_szKey + "-" + Gene )
	//

	BOOL  bRet			= FALSE;
	BOOL  bGetGene			= FALSE;
	TCHAR szGene[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	TCHAR szMd5[ 64 ]		= {0};
	
	if ( lpcszGene )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), lpcszGene );
	}
	else if ( _tcslen(mb_stLcInfo.szGene) )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );
	}
	
	if ( bGetGene )
	{
		//	regcode 算法
		//	key - gene = "%s-%s"
		_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmtRegCode, mb_stLcInfo.szKey, szGene );
		StrTrim( szSrc, _T("\r\n\t ") );
		_tcslwr( szSrc );

		if ( GetStringMd5( szSrc, szMd5, sizeof(szMd5), 0 ) )
		{
			bRet = GetFormatLicenceString( szMd5, lpszRegCode, dwSize );
		}
	}
	
	return bRet;
}


/**
 *	@ public
 *	获取 actcode
 */
BOOL CDeLicence::GetActCode( IN LPCTSTR lpcszGene, OUT LPTSTR lpszActCode, IN DWORD dwSize )
{
	//
	//	lpcszGene	- [in]  加密因子，一般为 NULL
	//	lpszActCode	- [out] 返回许可证书
	//	dwSize		- [in]  返回缓冲区的大小
	//	RETURN
	//	算法：		md5( m_szKey + "|" + Gene )
	//

	BOOL  bRet			= FALSE;
	BOOL  bGetGene			= FALSE;
	TCHAR szGene[ MAX_PATH ]	= {0};
	TCHAR szSrc[ MAX_PATH ]		= {0};
	TCHAR szMd5[ 64 ]		= {0};

	if ( lpcszGene )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), lpcszGene );
	}
	else if ( _tcslen(mb_stLcInfo.szGene) )
	{
		bGetGene = TRUE;
		_sntprintf( szGene, sizeof(szGene)-sizeof(TCHAR), _T("%s"), mb_stLcInfo.szGene );
	}

	if ( bGetGene )
	{
		//	actcode 算法
		//	key | gene = "%s|%s"
		_sntprintf( szSrc, sizeof(szSrc)-sizeof(TCHAR), m_szDecFmtActCode, mb_stLcInfo.szKey, szGene );
		StrTrim( szSrc, _T("\r\n\t ") );
		_tcslwr( szSrc );

		if ( GetStringMd5( szSrc, szMd5, sizeof(szMd5), 0 ) )
		{
			bRet = GetFormatLicenceString( szMd5, lpszActCode, dwSize );
		}
	}

	return bRet;
}

/**
 *	获取 MAC 地址
 */
BOOL CDeLicence::GetMacAddress( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize )
{
	//
	//	lpcszNetBiosName	- [in]
	//	lpszMacAddress		- [out] 
	//	dwSize			- [in]
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszNetBiosName || 0 == strlen( lpcszNetBiosName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszMacAddress || 0 == dwSize )
	{
		return FALSE;
	}
	
	BOOL bRet		= FALSE;
	TCHAR szBiosName[ 64 ]	= {0};
	STASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	//	copy bios name
	_sntprintf( szBiosName, sizeof(szBiosName)-sizeof(TCHAR), "%s", lpcszNetBiosName );
	
	memset( & ncb, 0, sizeof(ncb) );
	ncb.ncb_command		= NCBRESET;
	ncb.ncb_lana_num	= 0;

	uRetCode = Netbios( &ncb );
	if ( 0 == uRetCode )
	{
		memset( & ncb, 0, sizeof(ncb) );
		ncb.ncb_command		= NCBASTAT;
		ncb.ncb_lana_num	= 0;
		
		//	大写
		_tcsupr( szBiosName );
		
		FillMemory( ncb.ncb_callname, NCBNAMSZ - 1, 0x20 );
		_sntprintf( (LPTSTR)ncb.ncb_callname, sizeof(ncb.ncb_callname)-sizeof(TCHAR), "%s", szBiosName );
		
		ncb.ncb_callname[ strlen( szBiosName ) ] = 0x20;
		ncb.ncb_callname[ NCBNAMSZ ] = 0x0;
		
		ncb.ncb_buffer = (unsigned char *) &Adapter;
		ncb.ncb_length = sizeof(Adapter);
		
		uRetCode = Netbios( &ncb );
		if ( 0 == uRetCode )
		{
			bRet = TRUE;
			_sntprintf
			(
				lpszMacAddress,
				dwSize-sizeof(TCHAR),
				m_szDecFmtMacAddress,		//	"%02x-%02x-%02x-%02x-%02x-%02x"
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
		}
	}
	
	return bRet;
}

/**
 *	获取硬盘序列号
 */
BOOL CDeLicence::GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];

	bRet = ::GetVolumeInformation
	(
		m_szDecCDisk,	//	"c:\\"
		szNameBuf,
		sizeof(szNameBuf), 
		& dwSerialNumber, 
		& dwMaxLen,
		& dwFileFlag,
		szSysNameBuf,
		sizeof(szSysNameBuf)
	);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), _T("%x"), dwSerialNumber );
	}
	
	return bRet;
}

/**
 *	获取 cpuid
 */
BOOL CDeLicence::GetCpuId( LPTSTR lpszCPUID, DWORD dwSize )
{
	//
	//	lpszCPUID	- [out] cpuid 返回值
	//	dwSize		- [in]  返回值缓冲区大小
	//	RETURN		- TRUE / FALSE
	//
	//	备注：
	//		_asm cpuid 指令可以被一下 CPU 识别：
	//			- Intel 486 以上的 CPU
	//			- Cyrix M1 以上的 CPU
	//			- AMD Am486 以上的 CPU
	//

	if ( NULL == lpszCPUID || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL  bException	= FALSE;
	TCHAR szOEMInfo[ 13 ]	= {0};
	INT nFamily;
	INT nEAXValue, nEBXValue, nECXValue, nEDXValue;
	INT nALValue;
	ULONG ulIdPart1	= 0;
	ULONG ulIdPart2	= 0;
	ULONG ulIdPart3	= 0;
	ULONG ulIdPart4	= 0;

	__try
	{
		//	get OEM info
		//
		//	(1)判断CPU厂商，先让 EAX = 0，再调用 cpuid
		//
		_asm
		{
			mov	eax,	0
			cpuid
			mov	DWORD PTR szOEMInfo[0],	ebx
			mov	DWORD PTR szOEMInfo[4],	edx
			mov	DWORD PTR szOEMInfo[8],	ecx
			mov	BYTE PTR szOEMInfo[12],	0
		}

		//
		//	get family number
		//
		//	(2)CPU 到底是几 86，是否支持 MMX
		//	先让 EAX = 1，再调用 cpuid
		//	EAX 的 8 到 11 位就表明是几 86
		//	3 - 386
		//	4 - i486
		//	5 - Pentium
		//	6 - Pentium Pro Pentium II
		//	2 - Dual Processors
		//	EDX 的第 0 位: 有无 FPU
		//	EDX 的第 23 位: CPU 是否支持 IA MMX，很重要啊！如果你想用那 57 条新增的指令，先 
		//	检查这一位吧，否则就等着看Windows的“该程序执行了非法指令，将被关闭”吧 。
		//
		//	(3)专门检测是否 P6 架构
		//	先让 EAX = 1，再调用 CPUID
		//	如果 AL = 1，就是 Pentium Pro 或 Pentium II
		//
		//	(4)专门检测 AMD 的 CPU 信息
		//	先让 EAX = 80000001H，再调用 CPUID
		//	如果 EAX = 51H，是 AMD K5
		//	如果 EAX = 66H，是 K6
		//	K7 是什么标志，只有等大家拿到 K7 的芯再说了。
		//	EDX 第 0 位: 是否有 FPU(多余的！谁用过没 FPU 的 K5,K6?)
		//	EDX 第 23 位，CPU 是否支持 MMX
		//
		_asm
		{
			mov	eax,	1
			cpuid
			mov	nEAXValue,	eax
			mov	nEBXValue,	ebx
			mov	nECXValue,	ecx
			mov	nEDXValue,	edx
		}
		nFamily	= ( 0xf00 & nEAXValue ) >> 8;

		_asm
		{
			mov	eax,	2
			cpuid
			mov	nALValue,	eax
		}
		
		//
		//	get cpu id
		//
		//	CPU 的序列号用一个 96bit 的串表示，格式是连续的 6 个 WORD 值：XXXX-XXXX-XXXX-XXX-XXXX-XXXX。
		//	WORD是16个bit长的数据，可以用unsigned short模拟：
		//	typedef unsigned short WORD;
		//	获得序列号需要两个步骤，首先用 eax = 1 做参数，返回的 eax 中存储序列号的高两个 WORD。
		//	用 eax = 3 做参数，返回 ecx 和 edx 按从低位到高位的顺序存储前 4 个 WORD 即可得到 cpuid
		//
		_asm
		{
			mov	eax,	01h
			xor	edx,	edx
			cpuid
			mov	ulIdPart1,	edx
			mov	ulIdPart2,	eax
		}	
		_asm
		{
			mov	eax,	03h
			xor	ecx,	ecx
			xor	edx,	edx
			cpuid
			mov	ulIdPart3,	edx
			mov	ulIdPart4,	ecx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bException = TRUE;
	}

	_sntprintf
	(
		lpszCPUID, dwSize-sizeof(TCHAR),
		m_szDecFmtCpuId,	//	"%s-%d-%08x%08x%08x%08x"
		szOEMInfo, nFamily,
		ulIdPart1, ulIdPart2, ulIdPart3, ulIdPart4
	);
	_tcslwr( lpszCPUID );

	return bException ? FALSE : TRUE;
}


/**
 *	获取一个字符串的 MD5 值(32位字符串)
 */
BOOL CDeLicence::GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen /* = 0 */ )
{
	//
	//	lpszString	- [in]  待处理字符串
	//	lpszMd5		- [out] 返回 MD5 值字符串缓冲区
	//	dwSize		- [in]  返回 MD5 值字符串长度
	//	dwSpecStringLen	- [in]  指定处理 lpszString 从头开始的多少个字节
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpszString || NULL == lpszMd5 )
		return FALSE;
	if ( dwSize < 32 )
		return FALSE;
	
	MD5_CTX	m_md5;
	unsigned char szEncrypt[16];
	unsigned char c;
	INT i			= 0;

	memset( szEncrypt, 0, sizeof(szEncrypt) );
	if ( dwSpecStringLen > 0 )
	{
		m_md5.MD5Update( (unsigned char *)lpszString, dwSpecStringLen );
	}
	else
	{
		m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	}
	m_md5.MD5Final( szEncrypt );
	
	//	...
	for ( i = 0; i < 16; i++ )
	{
		c = szEncrypt[i] / 16;
		lpszMd5[i*2]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
		c = szEncrypt[i] % 16;
		lpszMd5[i*2+1]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
	}

	//	..
	lpszMd5[ min( 32, dwSize-sizeof(TCHAR) ) ] = 0;
	
	return TRUE;
}

/**
 *	获取指定名称的 Cookie Value
 */
BOOL CDeLicence::GetCookieValue( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszCookieString	- [in]  整个 COOKIE 的字符串
	//	lpszCookieName		- [in]  COOKIE 名称
	//	lpszValue		- [out] 返回值
	//	dwSize			- [in]  返回值缓冲区大小
	//	RETURN			- TRUE / FALSE
	//
	
	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpszValue )
	{
		return FALSE;
	}
	if ( NULL == lpszCookieString[0] || NULL == lpszCookieName[0] )
	{
		return FALSE;
	}
	
	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszMove	= NULL;
	LONG	uValueLen	= 0;
	
	lpszMove = _tcsstr( lpszCookieString, lpszCookieName );
	if ( lpszMove )
	{
		lpszHead = lpszMove + _tcslen(lpszCookieName);
		if ( lpszHead )
		{
			//
			//	拷贝内容到返回值缓冲区
			//
			memcpy( lpszValue, lpszHead, dwSize-sizeof(TCHAR) );
			
			//
			//	处理尾部
			//
			lpszMove = strpbrk( lpszValue, _T(" ;\r\n") );
			if ( lpszMove )
			{
				*lpszMove = 0;
			}
			
			bRet = TRUE;
		}
	}
	return bRet;
}


/**
 *	字符串比较
 */
BOOL CDeLicence::IsEqualString( LPCTSTR lpcszStr1, LPCTSTR lpcszStr2 )
{
	BOOL bRet	= FALSE;
	INT  i, nLen1, nLen2;

	if ( lpcszStr1 && lpcszStr2 )
	{
		nLen1	= _tcslen(lpcszStr1);
		nLen2	= _tcslen(lpcszStr2);
		if ( nLen1 == nLen2 )
		{
			bRet = TRUE;
			for ( i = 0; i < nLen1; i ++ )
			{
				if ( lpcszStr1[ i ] != lpcszStr2[ i ] )
				{
					bRet = FALSE;
					break;
				}
			}
		}
	}

	return bRet;
}











//////////////////////////////////////////////////////////////////////////
//	Private




/**
 *	获取格式化后的注册码信息
 */
BOOL CDeLicence::GetFormatLicenceString( LPCTSTR lpcszInput, LPTSTR lpszLicenceString, DWORD dwSize )
{
	if ( NULL == lpcszInput || 32 != _tcslen(lpcszInput) )
	{
		return FALSE;
	}
	if ( NULL == lpszLicenceString || dwSize <= 35 )
	{
		return FALSE;
	}

	//	...
	_sntprintf
	(
		lpszLicenceString, dwSize-sizeof(TCHAR),
		_T("%.8s-%.8s-%.8s-%.8s"),
		lpcszInput+0,    lpcszInput+8,
		lpcszInput+16,   lpcszInput+24
	);
	return TRUE;
}


