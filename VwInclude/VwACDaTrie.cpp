// VwACDaTrie.cpp: implementation of the VwACDaTrie class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwACDaTrie.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwACDaTrie::CVwACDaTrie()
{
	InitializeCriticalSection( & m_oCrSecTree );
	
	m_bSoftReged		= FALSE;	//	软件注册状态
	m_bLoadingDict		= FALSE;
	
	m_uCurrTreeIndex	= 0;		//	两棵树的索引值

	m_pcWdaTrie		= NULL;		//	线上服务的树
}
CVwACDaTrie::~CVwACDaTrie()
{
	DeleteCriticalSection( & m_oCrSecTree );
}


/**
 *	@ public
 *	设置软件注册状态
 */
VOID CVwACDaTrie::SetSoftRegStatus( BOOL bReged )
{
	m_bSoftReged = bReged;
}


/**
 *	@ public
 *	创建词典文件
 */
BOOL CVwACDaTrie::BuildDict( LPCTSTR lpcszWordsFile, LPCTSTR lpcszDictFile, vector<STVWINFOMWORD> * pvcWordList /*=NULL*/ )
{
	if ( NULL == lpcszWordsFile && NULL == pvcWordList )
	{
		return FALSE;
	}
	if ( lpcszWordsFile )
	{
		if ( 0 == _tcslen( lpcszWordsFile ) || FALSE == PathFileExists( lpcszWordsFile ) )
		{
			return FALSE;
		}
	}

	if ( NULL == lpcszDictFile || 0 == _tcslen( lpcszDictFile ) )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	CVwTDaTrieAc<INT> cWdaTrie;
	vector<STVWINFOMWORD> vcWordList;
	vector<STVWINFOMWORD>::iterator it;
	vector<STVWINFOMWORD> * pvcSaveWordList;
	STVWINFOMWORD stWord;
	STVWINFOMWORD * pstWord;
	FILE * fp;

	UINT uMaxWordLoadCount;
	UINT uWordMaxSize;


	//	...
	bRet = FALSE;


	try
	{
		pvcSaveWordList		= NULL;
		uMaxWordLoadCount	= 0;
		uWordMaxSize		= 0;

		if ( pvcWordList )
		{
			//
			//	优先
			//	从函数参数传入了 vector 数组
			//
			pvcSaveWordList = pvcWordList;

			for ( it = pvcWordList->begin(); it != pvcWordList->end(); it ++ )
			{
				pstWord = it;
				StrTrim( pstWord->szWord, " \t\r\n" );
				pstWord->uLen = _tcslen( pstWord->szWord );
				if ( pstWord->uLen > uWordMaxSize )
				{
					uWordMaxSize = pstWord->uLen;
				}
			}
		}
		else
		{
			//
			//	从词文件中装载
			//

			//	免费版本最多只让设置 6 个过滤词
			//	0 or 6
			uMaxWordLoadCount	= ( m_bSoftReged ? 0 : ( 987 - 980 - 1 ) );
			uWordMaxSize		= CVWINFOMONITORCONFIGFILE_MAX_WORDLEN;	//	DEACHASHTREE_MAX_TREE_DEEP;
			vcWordList.clear();

			//
			//	从文件中装载词
			//
			if ( CVwInfoMonitorConfigFile::LoadWordsFromFile( lpcszWordsFile, uMaxWordLoadCount, vcWordList, & uWordMaxSize ) )
			{
				pvcSaveWordList = &vcWordList;
			}
		}

		if ( pvcSaveWordList )
		{
			//
			//	开始创建索引
			//
			if ( pvcSaveWordList->size() && uWordMaxSize )
			{
				for ( it = pvcSaveWordList->begin(); it != pvcSaveWordList->end(); it ++ )
				{
					pstWord = it;
					StrTrim( pstWord->szWord, " \t\r\n" );
					pstWord->uLen = _tcslen( pstWord->szWord );
					if ( pstWord->uLen )
					{
						//
						//	将词插入内存索引中
						//
						cWdaTrie.insert( (__charwt*)pstWord->szWord );
					}
				}

				//
				//	保存到索引文件
				//
				bRet = cWdaTrie.saveDict( lpcszDictFile );
			}

			if ( ! bRet && 0 == pvcSaveWordList->size() )
			{
				//	没有词，创建一个空文件
				fp = fopen( lpcszDictFile, "w" );
				if ( fp )
				{
					fclose( fp );
					fp = NULL;
				}
			}
		}
	}
	catch (...)
	{
	}

	return bRet;
}


/**
 *	@ public
 *	转载已经编译好的词典文件
 */
BOOL CVwACDaTrie::LoadDict( LPCTSTR lpcszDictFile )
{
	if ( NULL == lpcszDictFile || FALSE == PathFileExists( lpcszDictFile ) )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	CVwTDaTrieAc<INT> * poWdaTrieOff;

	//
	//	进入红灯区
	//
	//EnterCriticalSection( & m_oCrSecTree );
	m_bLoadingDict = TRUE;

	//
	//	在离线树上装载
	//
	poWdaTrieOff = & m_cArrWdaTrie[ GetOfflineTreeIndex() ];
	if ( poWdaTrieOff )
	{
		//
		//	装载新词典
		//	# loadDict 函数会自动清除以前的内存
		//
		if ( delib_get_file_size( lpcszDictFile ) > 0 )
		{
			if ( poWdaTrieOff->loadDict( lpcszDictFile ) )
			{
				bRet	= TRUE;

				//
				//	将刚刚装载的新树推上线
				//	# 新树创建于离线树上
				//
				PushNewTreeOnline();
			}
		}
		else
		{
			//	这是个空树，直接清理下词库了事
			poWdaTrieOff->clearIndex();
			poWdaTrieOff->cleanMemory();

			//
			//	将刚刚装载的新树推上线
			//	# 新树创建于离线树上
			//
			PushNewTreeOnline();
		}
	}

	//
	//	离开红灯区
	//
	//LeaveCriticalSection( & m_oCrSecTree );
	m_bLoadingDict = FALSE;


	return bRet;
}

/**
 *	@ public
 *	前匹配
 */
UINT CVwACDaTrie::PrefixMatch( LPCTSTR lpcszText )
{
	//
	//	lpcszText	- 输入字符串
	//	RETURN		- 返回前匹配到的字符串的长度
	//
	
	if ( NULL == lpcszText )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}
	if ( NULL == m_pcWdaTrie )
	{
		return FALSE;
	}
	
	//	未指定匹配长度，直到 0 结束
	return (UINT)( m_pcWdaTrie->prefixMatch( (__charwt*)lpcszText ) );
}
UINT CVwACDaTrie::PrefixMatch( LPCTSTR lpcszText, UINT uTextLen )
{
	//
	//	lpcszText	- 输入字符串
	//	uTextLen	- 输入字符串长度
	//	RETURN		- 返回前匹配到的字符串的长度
	//

	if ( NULL == lpcszText )
	{
		return FALSE;
	}
	if ( m_bLoadingDict )
	{
		return FALSE;
	}
	if ( NULL == m_pcWdaTrie )
	{
		return FALSE;
	}

	return (UINT)( m_pcWdaTrie->prefixMatch( (__charwt*)lpcszText ), uTextLen );
}



//////////////////////////////////////////////////////////////////////////
//	Private




UINT CVwACDaTrie::GetOnlineTreeIndex()
{
	return m_uCurrTreeIndex;
}

UINT CVwACDaTrie::GetOfflineTreeIndex()
{
	return ( ( m_uCurrTreeIndex + 1 ) % 2 );
}

BOOL CVwACDaTrie::PushNewTreeOnline()
{
	//
	//	获取当前树的索引值
	//	离线树 赋给 当前树
	//
	m_uCurrTreeIndex = GetOfflineTreeIndex();

	//
	//	将 离线树的指针 直接指向 当前树指针
	//
	m_pcWdaTrie = & m_cArrWdaTrie[ m_uCurrTreeIndex ];
	
	return TRUE;
}