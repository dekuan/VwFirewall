// VwSpiderMatch.h: interface for the CVwSpiderMatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWSPIDERMATCH_HEADER__
#define __VWSPIDERMATCH_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	各大搜索引擎蜘蛛索引值
 */
enum
{
	VWSPIDERMATCH_IDX_GOOGLE = 0,
	VWSPIDERMATCH_IDX_BAIDU,
	VWSPIDERMATCH_IDX_YAHOO,
	VWSPIDERMATCH_IDX_YODAO,
	VWSPIDERMATCH_IDX_SOSO,
	VWSPIDERMATCH_IDX_SOGOU,
	VWSPIDERMATCH_IDX_MSN,
	VWSPIDERMATCH_IDX_ALEXA,
	VWSPIDERMATCH_IDX_SINAWEIBO,
};


typedef struct tagVwSpiderMatchSpiderInfo
{
	tagVwSpiderMatchSpiderInfo( DWORD dwSpIndex_in, LPCTSTR lpcszName, LPCTSTR lpcszAgent )
	{
		memset( this, 0, sizeof(tagVwSpiderMatchSpiderInfo) );

		dwSpIndex = dwSpIndex_in;
		_sntprintf( szName, sizeof(szName)-sizeof(TCHAR), _T("%s"), lpcszName );
		_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), lpcszAgent );
	}

	DWORD dwSpIndex;
	TCHAR szName[ 64 ];
	TCHAR szAgent[ MAX_PATH ];

}STVWSPIDERMATCHSPIDERINFO, *LPSTVWSPIDERMATCHSPIDERINFO;

/**
 *	各大搜索引擎蜘蛛特征符号表
 *	*** 注意顺序必须按照索引值的定义顺序，因为该数组是按照约定的顺序定义的
 */
static STVWSPIDERMATCHSPIDERINFO g_ArrVwSpiderMatchSpiderInfo[] =
{
	//	0 - GOOGLE
	//	"Googlebot" + "Googlebot-Image/1.0" + "Mediapartners-Google"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_GOOGLE, "Google Spider", "Googlebot|Feedfetcher-Google|Mediapartners-Google|" ),

	//	1 - 百度
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_BAIDU, "Baidu Spider", "Baiduspider|Baidu-Transcoder|" ),

	//	2 - YAHOO
	//	"Yahoo! Slurp China;" +  "Yahoo! Slurp;"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_YAHOO, "Yahoo Spider", "Yahoo! Slurp|YahooSeeker|" ),

	//	3 - 有道
	//	"YodaoBot/1.0;"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_YODAO, "Yodao Spider", "YodaoBot|" ),

	//	4 - SOSO
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SOSO, "Soso Spider", "Sosospider|Sosoimagespider|" ),

	//	5 - Sogou
	//	"Sogou Web Sprider|Sogou web spider/4.0|Sogou Orion spider/4.0|Sogou-Test-Spider/4.0|Sogou+head+spider/3.0"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SOGOU, "Sogou Spider", "Sogou Web Sprider|Sogou web spider|Sogou Orion spider|Sogou-Test-Spider|Sogou+head+spider|sohu-search|" ),

	//	6 - Msn Search
	//	"msnbot-media"
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_MSN, "Msn-Search Spider", "msnbot|" ),

	//	7 - Alexa
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_ALEXA, "Alexa-Archiver Spider", "ia_archiver|" ),

	//	8 - SinaWeiboBot
	STVWSPIDERMATCHSPIDERINFO( VWSPIDERMATCH_IDX_SINAWEIBO, "SinaWeibo Bot", "SinaWeiboBot|" )
};

/**
 *	蜘蛛的个数
 */
#define VWSPIDERMATCH_COUNT		( sizeof(g_ArrVwSpiderMatchSpiderInfo) / sizeof(g_ArrVwSpiderMatchSpiderInfo[0]) )
#define VWSPIDERMATCH_MAX_TREEDEEP	64	//	最大树深度


/**
 *	@ public
 *	获取格式化个蜘蛛名称
 */
static BOOL VwSpiderMatch_IsValidSpiderIndex( DWORD dwSpIndex )
{
	return ( dwSpIndex >= 0 && dwSpIndex < VWSPIDERMATCH_COUNT );
}
static LPTSTR VwSpiderMatch_GetSpiderNameByIndex( DWORD dwSpIndex )
{
	if ( ! VwSpiderMatch_IsValidSpiderIndex( dwSpIndex ) )
	{
		return NULL;
	}

	return g_ArrVwSpiderMatchSpiderInfo[ dwSpIndex ].szName;
}
static DWORD VwSpiderMatch_GetSpiderIndexByName( LPCTSTR lpszSpiderName )
{
	if ( NULL == lpszSpiderName )
	{
		return 0;
	}

	DWORD dwRet	= 0;
	INT i;

	for ( i = 0; i < VWSPIDERMATCH_COUNT; i ++ )
	{
		if ( 0 == _tcsicmp( lpszSpiderName, g_ArrVwSpiderMatchSpiderInfo[ i ].szName ) )
		{
			dwRet = g_ArrVwSpiderMatchSpiderInfo[ i ].dwSpIndex;
			break;
		}
	}

	return dwRet;
}


/**
 *	数据结构体
 */
typedef struct tagVwSpiderMatchTreeNode
{
	//	树节点
	tagVwSpiderMatchTreeNode()
	{
		memset( this, 0, sizeof(tagVwSpiderMatchTreeNode) );
	}
	
	BYTE btNode[ 0x100 ];
	
}STVWSPIDERMATCHTREENODE, *LPSTVWSPIDERMATCHTREENODE;

typedef struct tagVwSpiderMatchTree
{
	//	整个树的配置
	tagVwSpiderMatchTree()
	{
		memset( this, 0, sizeof(tagVwSpiderMatchTree) );

		InitTree();
	}

	VOID InitTree()
	{
		INT   i;
		DWORD dwWordLenTemp		= 0;
		TCHAR * pszSpider		= NULL;
		TCHAR * pszHead;
		TCHAR * pszMov;

		//
		//	初始化参数值
		//
		this->bInitSucc		= FALSE;
		this->dwWordCount	= 0;
		this->dwMinWordLen	= 100000000;	//	最短匹配词的长度
		this->dwMaxWordLen	= 0;		//	最长匹配词的长度
		memset( this->stTree, 0, sizeof(this->stTree) );
		
		//
		//	遍历所有蜘蛛的所有词，计算上面这些值
		//
		for ( i = 0; i < VWSPIDERMATCH_COUNT; i ++ )
		{
			pszSpider = g_ArrVwSpiderMatchSpiderInfo[ i ].szAgent;
			if ( pszSpider && _tcslen(pszSpider) )
			{
				//	"Googlebot|Feedfetcher-Google|Mediapartners-Google|"
				pszHead	= pszSpider;
				while( pszHead )
				{
					pszMov = _tcsstr( pszHead, "|" );
					if ( pszMov )
					{
						dwWordLenTemp = pszMov - pszHead;
						this->dwMaxWordLen = max( this->dwMaxWordLen, dwWordLenTemp );
						this->dwMinWordLen = min( this->dwMinWordLen, dwWordLenTemp );

						pszHead = CharNext( pszMov );
					}
					else
					{
						break;
					}
				}
			}
		}

		if ( this->dwMinWordLen && this->dwMaxWordLen && this->dwMinWordLen <= this->dwMaxWordLen )
		{
			this->dwMaxWordLen = min( this->dwMaxWordLen, VWSPIDERMATCH_MAX_TREEDEEP );
			this->bInitSucc = TRUE;
		}
		else
		{
			this->dwMinWordLen = 0;
		}
	}

	//
	//	成员变量
	//
	BOOL  bInitSucc;		//	是否初始化成功
	DWORD dwWordCount;		//	插入多少个关键词
	DWORD dwMinWordLen;		//	最短匹配词的长度
	DWORD dwMaxWordLen;		//	最长匹配词的长度
	STVWSPIDERMATCHTREENODE stTree[ VWSPIDERMATCH_MAX_TREEDEEP ];

}STVWSPIDERMATCHTREE, *LPSTVWSPIDERMATCHTREE;




/**
 *	class of CVwSpiderMatch
 */
class CVwSpiderMatch
{
public:
	CVwSpiderMatch();
	virtual ~CVwSpiderMatch();

	BOOL InsertSpider( STVWSPIDERMATCHTREE * pstTree, DWORD dwSpiderIndex );
	BOOL IsValidTreeAndParam( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen );
	UINT PrefixMatch( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen );

};


#endif	// __VWSPIDERMATCH_HEADER__
