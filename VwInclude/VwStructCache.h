#ifndef __VWSTRUCTCACHE_HEADER__
#define __VWSTRUCTCACHE_HEADER__



/**
 *	struct
 *	CfgIdx cache 结构体
 */
typedef struct tagCacheCfgIdx
{
	tagCacheCfgIdx()
	{
		uServerPort		= 0;
		dwLocalAddrValue	= 0;
		szHost[ 0 ]		= 0;
		pLeft			= NULL;
		pRight			= NULL;
	}
	int operator - ( const tagCacheCfgIdx & r )
	{
		if ( uServerPort == r.uServerPort )
		{
			if ( dwLocalAddrValue == r.dwLocalAddrValue )
			{
				return _tcsicmp( szHost, r.szHost );
			}
			else
			{
				return ( dwLocalAddrValue - r.dwLocalAddrValue );
			}
		}
		else
		{
			return ( uServerPort - r.uServerPort );
		}
	}

	LONG  lnCfgIndex;		//	STHOSTINFO 中的一个成员变量，用于 SET CACHE
	ULONG uServerPort;
	TCHAR szHost[ 128 ];
	DWORD dwLocalAddrValue;

	tagCacheCfgIdx * pLeft;		//	左节点
	tagCacheCfgIdx * pRight;	//	又节点

}STCACHECFGIDX, *PSTCACHECFGIDX;



/**
 *	config data
 */
typedef struct tagCfgData
{
	tagCfgData()
	{
		memset( this, 0, sizeof(tagCfgData) );
		lpszLmtPlayer	= NULL;
		lpszReplaceFile	= NULL;
	}
	LONG	lnConfig;		//	是否开启：1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly, 8-limit thread, 16-limit speed
	LONG	lnLmtThread;		//	限制线程
	LONG	lnLmtSpeed;		//	限制速度
	LONG	lnLmtDayIp;		//	限制被保护资源日访问次数	

	LONG	lnAction;		//	处理动作：1-Send warning messages表示发送警告信息, 2-表示使用文件替换	
	LPSTR	lpszLmtPlayer;		//	该扩展名限制使用哪个播放器播放。MP 表示 Window Media Player；RP 表示 Real Player。
	LPSTR	lpszReplaceFile;	//	该扩展名的警告替换文件路径

}STCFGDATA, *PSTCFGDATA;


/**
 *	用于判断函数 IsProtectResource 等的 Set Cache 存储
 */
typedef struct tagCacheRes
{
	tagCacheRes()
	{
		lnFlag		= (-1);
		pstCfgData	= NULL;
		lnCfgIndex	= (-1);
		dwFullUriCrc32	= 0;

		pLeft		= NULL;
		pRight		= NULL;
	}
	int operator - ( const tagCacheRes & r )
	{
		if ( lnCfgIndex == r.lnCfgIndex )
		{
			return ( dwFullUriCrc32 - r.dwFullUriCrc32 );
		}
		else
		{
			return ( lnCfgIndex - r.lnCfgIndex );
		}
	}

	LONG  lnFlag;			//	非 Set 主键: 标记参数
	LONG  lnReturn;			//	非 Set 主键: 标记参数
	STCFGDATA * pstCfgData;		//	非 Set 主键: 配置参数指针

	LONG  lnCfgIndex;		//	STHOSTINFO 中的一个成员变量，用于 set cache 排序
	DWORD dwFullUriCrc32;		//	用于 set cache 排序
	//DWORD dwRefererHostCrc32;	//	用户例外的判断 2009-05-26 废除 RefererHost 方法，直接用 Referer 判断
	DWORD dwRefererCrc32;		//	用户例外的判断

	tagCacheRes * pLeft;		//	左节点
	tagCacheRes * pRight;		//	又节点

}STCACHERES, *PSTCACHERES;

//	less compare function for ProtectedRes
class __stcmp_less_tagCachePrtRes
{
public:	
	bool operator()( const STCACHERES & l, const STCACHERES & r ) const
	{
		if ( l.lnCfgIndex == r.lnCfgIndex )
		{
			return ( l.dwFullUriCrc32 < r.dwFullUriCrc32 ) ? true : false;
		}
		else
		{
			return ( l.lnCfgIndex < r.lnCfgIndex ) ? true : false;
		}
	}
};

//	less compare function for ExceptionRes
class __stcmp_less_tagCacheEcpRes
{
public:
	bool operator()( const STCACHERES & l, const STCACHERES & r ) const
	{
		if ( l.lnCfgIndex == r.lnCfgIndex )
		{
			if ( l.dwFullUriCrc32 == r.dwFullUriCrc32 )
			{
				return ( l.dwRefererCrc32 < r.dwRefererCrc32 ) ? true : false;
			}
			else
			{
				return ( l.dwFullUriCrc32 < r.dwFullUriCrc32 ) ? true : false;
			}
		}
		else
		{
			return ( l.lnCfgIndex < r.lnCfgIndex ) ? true : false;
		}
	}
};




#endif	// __VWSTRUCTCACHE_HEADER__