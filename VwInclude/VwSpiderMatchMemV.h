// VwSpiderMatchMemV.h: interface for the CVwSpiderMatchMemV class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWSPIDERMATCHMEMV_HEADER__
#define __VWSPIDERMATCHMEMV_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	各大搜索引擎蜘蛛索引值
 */
#define VWSPIDERMATCHMEMV_IDX_GOOGLE		0
#define VWSPIDERMATCHMEMV_IDX_BAIDU		1
#define VWSPIDERMATCHMEMV_IDX_YAHOO		2
#define VWSPIDERMATCHMEMV_IDX_YODAO		3
#define VWSPIDERMATCHMEMV_IDX_SOSO		4
#define VWSPIDERMATCHMEMV_IDX_SOGOU		5
#define VWSPIDERMATCHMEMV_IDX_MSN		6
#define VWSPIDERMATCHMEMV_IDX_ALEXA		7


typedef struct tagVwSpiderMatchMemVSpiderInfo
{
	DWORD dwSpIndex;
	TCHAR * pszName;
	DWORD dwAgentCount;
	TCHAR * pszAgentChrs[ 8 ];

}STVWSPIDERMATCHMEMVSPIDERINFO, *LPSTVWSPIDERMATCHMEMVSPIDERINFO;

/**
 *	各大搜索引擎蜘蛛特征符号表
 *	*** 注意顺序必须按照索引值的定义顺序，因为该数组是按照约定的顺序定义的
 */
static STVWSPIDERMATCHMEMVSPIDERINFO g_ArrVwSpiderMatchMemVSpiderInfo[] =
{
	//	0 - GOOGLE
	//	"Googlebot" + "Googlebot-Image/1.0" + "Mediapartners-Google"
	{ VWSPIDERMATCHMEMV_IDX_GOOGLE, "Google Spider", 3, { "Googlebot", "Feedfetcher-Google", "Mediapartners-Google" } },

	//	1 - 百度
	{ VWSPIDERMATCHMEMV_IDX_BAIDU, "Baidu Spider", 8, { "Baiduspider", "Baidu-Transcoder", "Baiduspider-mobile", "Baiduspider-image", "Baiduspider-video", "Baiduspider-news", "Baiduspider-favo", "Baiduspider-cpro" } },

	//	2 - YAHOO
	//	"Yahoo! Slurp China;" +  "Yahoo! Slurp;"
	{ VWSPIDERMATCHMEMV_IDX_YAHOO, "Yahoo Spider", 2, { "Yahoo! Slurp", "YahooSeeker" } },

	//	3 - 有道
	//	"YodaoBot/1.0;"
	{ VWSPIDERMATCHMEMV_IDX_YODAO, "Yodao Spider", 1, { "YodaoBot" } },

	//	4 - SOSO
	{ VWSPIDERMATCHMEMV_IDX_SOSO, "Soso Spider", 2, { "Sosospider", "Sosoimagespider" } },

	//	5 - Sogou
	//	"Sogou Web Sprider|Sogou web spider/4.0|Sogou Orion spider/4.0|Sogou-Test-Spider/4.0|Sogou+head+spider/3.0"
	{ VWSPIDERMATCHMEMV_IDX_SOGOU, "Sogou Spider",	6, { "Sogou Web Sprider", "Sogou web spider", "Sogou Orion spider", "Sogou-Test-Spider", "Sogou+head+spider", "sohu-search" } },

	//	6 - Msn Search
	//	"msnbot-media"
	{ VWSPIDERMATCHMEMV_IDX_MSN, "Msn-Search Spider", 1, { "msnbot" } },

	//	7 - Alexa
	{ VWSPIDERMATCHMEMV_IDX_ALEXA, "Alexa-Archiver Spider", 1, { "ia_archiver" } }
};

/**
 *	蜘蛛的个数
 */
#define VWSPIDERMATCHMEMV_COUNT		( sizeof(g_ArrVwSpiderMatchMemVSpiderInfo) / sizeof(g_ArrVwSpiderMatchMemVSpiderInfo[0]) )


/**
 *	@ public
 *	获取格式化个蜘蛛名称
 */
static BOOL VwSpiderMatchMemV_IsValidSpiderIndex( DWORD dwSpIndex )
{
	return ( dwSpIndex >= 0 && dwSpIndex < VWSPIDERMATCHMEMV_COUNT );
}
static LPSTVWSPIDERMATCHMEMVSPIDERINFO VwSpiderMatchMemV_GetSpiderInfoByIndex( DWORD dwSpIndex )
{
	if ( ! VwSpiderMatchMemV_IsValidSpiderIndex( dwSpIndex ) )
	{
		return NULL;
	}

	return ( & g_ArrVwSpiderMatchMemVSpiderInfo[ dwSpIndex ] );
}
static LPTSTR VwSpiderMatchMemV_GetSpiderNameByIndex( DWORD dwSpIndex )
{
	if ( ! VwSpiderMatchMemV_IsValidSpiderIndex( dwSpIndex ) )
	{
		return NULL;
	}

	return g_ArrVwSpiderMatchMemVSpiderInfo[ dwSpIndex ].pszName;
}
static DWORD VwSpiderMatchMemV_GetSpiderIndexByName( LPCTSTR lpszSpiderName )
{
	if ( NULL == lpszSpiderName )
	{
		return 0;
	}

	DWORD dwRet	= 0;
	INT i;

	for ( i = 0; i < VWSPIDERMATCHMEMV_COUNT; i ++ )
	{
		if ( 0 == _tcsicmp( lpszSpiderName, g_ArrVwSpiderMatchMemVSpiderInfo[ i ].pszName ) )
		{
			dwRet = g_ArrVwSpiderMatchMemVSpiderInfo[ i ].dwSpIndex;
			break;
		}
	}

	return dwRet;
}




/**
 *	struct of tagVwSpiderMatchMemVData
 */
typedef struct tagVwSpiderMatchMemVData
{
	DWORD dwSpIndex;
	LONG  lnConfig;

}STVWSPIDERMATCHMEMVDATA, *LPSTVWSPIDERMATCHMEMVDATA;



/**
 *	calss of CVwSpiderMatchMemV
 */
class CVwSpiderMatchMemV  
{
public:
	CVwSpiderMatchMemV();
	virtual ~CVwSpiderMatchMemV();

};




#endif // __VWSPIDERMATCHMEMV_HEADER__
