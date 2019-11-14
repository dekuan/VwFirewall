// VwSpiderMatch.cpp: implementation of the CVwSpiderMatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwSpiderMatch.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwSpiderMatch::CVwSpiderMatch()
{
}
CVwSpiderMatch::~CVwSpiderMatch()
{
}


/**
 *	@ public
 *	插入新的蜘蛛配置
 */
BOOL CVwSpiderMatch::InsertSpider( STVWSPIDERMATCHTREE * pstTree, DWORD dwSpiderIndex )
{
	return TRUE;
/*
	if ( NULL == pstTree || FALSE == pstTree->bInitSucc )
	{
		return FALSE;
	}
	if ( ! VwSpiderMatch_IsValidSpiderIndex( dwSpiderIndex ) )
	{
		return FALSE;
	}

	BOOL bRet			= FALSE;
	TCHAR * pszSpider		= NULL;
	STTEXTLINE * pstWordList	= NULL;
	INT nCount;
	INT nWordLen;
	INT i, j;
	INT nMax;
	UINT uDeep;
	WORD wPosVal;

	pszSpider = g_ArrVwSpiderMatchSpiderInfo[ dwSpiderIndex ].szAgent;
	if ( pszSpider && _tcslen(pszSpider) )
	{
		//	劈开所有的词
		nCount = delib_split_string( pszSpider, _T("|"), NULL, FALSE );
		if ( nCount )
		{
			pstWordList = new STTEXTLINE[ nCount ];
			if ( pstWordList )
			{
				memset( pstWordList, 0, sizeof(STTEXTLINE)*nCount );
				delib_split_string( pszSpider, _T("|"), pstWordList, FALSE );
				for ( i = 0; i < nCount; i ++ )
				{
					//	遍历单个词
					StrTrim( pstWordList[i].szLine, " \r\n\t" );
					nWordLen = _tcslen( pstWordList[i].szLine );
					if ( nWordLen )
					{
						_tcslwr( pstWordList[i].szLine );
						nMax = min( pstTree->dwMaxWordLen, nWordLen );

						for ( j = 0; j < nMax; j ++ )
						{
							//	默认每次 +1
							wPosVal	= (UCHAR)pstWordList[i].szLine[ j ];
							uDeep	= j;

							pstTree->stTree[ uDeep ].btNode[ wPosVal ] = 1;
						}

						//	词的个数
						pstTree->dwWordCount ++;
					}
				}
			}
		}
	}
	return bRet;
	*/
}

/**
 *	@ public
 *	是否是有效的树
 */
BOOL CVwSpiderMatch::IsValidTreeAndParam( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen )
{
	if ( pstTree && pstTree->bInitSucc )
	{
		if ( lpcszText && uTextLen )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/**
 *	@ public
 *	前匹配
 */
UINT CVwSpiderMatch::PrefixMatch( STVWSPIDERMATCHTREE * pstTree, LPCTSTR lpcszText, UINT uTextLen )
{
	//
	//	使用前必须用 IsValidTreeAndParam() 先检查一下参数
	//
	
	INT i;
	INT nMaxDeep;
	INT uDeep;
	WORD wPosVal;

	//	扫描的最大深度
	nMaxDeep	= min( pstTree->dwMaxWordLen, uTextLen );

	for ( i = 0; i < nMaxDeep; i ++ )
	{
		wPosVal	= (UCHAR)lpcszText[ i ];
		uDeep	= i;

		if ( 0 == pstTree->stTree[ uDeep ].btNode[ wPosVal ]  )
		{
			break;
		}
	}

	return i;
}