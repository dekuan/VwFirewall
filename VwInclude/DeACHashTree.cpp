// DeACHashTree.cpp: implementation of the CDeACHashTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeACHashTree.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeACHashTree::CDeACHashTree()
{
	InitializeCriticalSection( & m_oCrSecTree );

	m_bSoftReged		= FALSE;	//	软件注册状态
	m_bTreeSwitching	= FALSE;

	m_uCurrTreeIndex	= 0;
	m_uCurrTreeDeep		= 0;

	m_pArrTree[ 0 ]		= NULL;
	m_pArrTree[ 1 ]		= NULL;
	m_pstTreeOn		= m_pArrTree[ 0 ];	//	线上服务的树
}

CDeACHashTree::~CDeACHashTree()
{
	DeleteCriticalSection( & m_oCrSecTree );
}

/**
 *	@ public
 *	设置软件注册状态
 */
VOID CDeACHashTree::SetSoftRegStatus( BOOL bReged )
{
	m_bSoftReged = bReged;
}

BOOL CDeACHashTree::BuildTree( LPCTSTR lpcszFilename )
{
	if ( NULL == lpcszFilename || ( ! PathFileExists( lpcszFilename ) ) )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	FILE * fp			= NULL;
	vector<STVWINFOMWORD> vcWordList;
	vector<STVWINFOMWORD>::iterator it;
	STVWINFOMWORD stWord;
	STVWINFOMWORD * pstWord;

	UINT uMaxWordLoadCount	= 0;
	UINT uWordMaxSize	= 0;
	
	UINT i;
	STDEACHASHTREENODE * pstNewTree;

	UINT uDeep;
	UINT uAddStep;
	UCHAR ucCharL, ucCharH;
	WORD wPosVal;

	//	从文件中装载词
	vcWordList.clear();

	if ( m_bSoftReged )
	{
		uMaxWordLoadCount = 0;
	}
	else
	{
		uMaxWordLoadCount = ( 987 - 980 - 1 );	//	6
	}
	uWordMaxSize		= CVWINFOMONITORCONFIGFILE_MAX_WORDLEN;	//DEACHASHTREE_MAX_TREE_DEEP;
	CVwInfoMonitorConfigFile::LoadWordsFromFile( lpcszFilename, uMaxWordLoadCount, vcWordList, & uWordMaxSize );

	try
	{
		//	开始创建索引
		if ( vcWordList.size() && uWordMaxSize )
		{
			pstNewTree = new STDEACHASHTREENODE[ uWordMaxSize + 1 ];
			if ( pstNewTree )
			{
				memset( pstNewTree, 0, ( sizeof(STDEACHASHTREENODE) ) * ( uWordMaxSize + 1 ) );

				for ( it = vcWordList.begin(); it != vcWordList.end(); it ++ )
				{
					pstWord = it;

					i	= 0;
					uDeep	= 0;

					while( i < pstWord->uLen )
					{
						//	默认每次 +1
						uAddStep = 1;
						ucCharH = (UCHAR)pstWord->szWord[ i ];
						if ( ucCharH > 0x80 && i < ( pstWord->uLen - 1 ) )
						{
							//	本次取了两位，所以要 +2
							uAddStep = 2;
							ucCharL  = (UCHAR)pstWord->szWord[ ( i + 1 ) ];
							wPosVal	 = MAKEWORD( ucCharL, ucCharH );
						}
						else
						{
							wPosVal	 = ucCharH;
						}

						pstNewTree[ uDeep ].btNode[ wPosVal ]	= 1;

						i += uAddStep;
						uDeep ++;
					}
				}

				//
				//	将新 build 的树推上线
				//
				PushNewTreeOnline( pstNewTree, uWordMaxSize );
			}
		}
	}
	catch (...)
	{
	}

	return bRet;
}

UINT CDeACHashTree::GetOnlineTreeIndex()
{
	return m_uCurrTreeIndex;
}

UINT CDeACHashTree::GetOfflineTreeIndex()
{
	return ( ( m_uCurrTreeIndex + 1 ) % 2 );
}

BOOL CDeACHashTree::PushNewTreeOnline( STDEACHASHTREENODE * pstNewTree, UINT uTreeDeep )
{
	if ( NULL == pstNewTree || 0 == uTreeDeep )
	{
		return FALSE;
	}

	//	进入红灯区
	//EnterCriticalSection( & m_oCrSecTree );
	m_bTreeSwitching = TRUE;


	//	获得离线的树的编号
	UINT uOfflineTreeIndex = GetOfflineTreeIndex();

	//	先删除现有离线 tree 的数据
	if ( m_pArrTree[ uOfflineTreeIndex ] )
	{
		delete [] m_pArrTree[ uOfflineTreeIndex ];
		m_pArrTree[ uOfflineTreeIndex ] = NULL;
	}

	//
	//	将新 Tree 的数据结构指针指向离线数组
	//
	m_pArrTree[ uOfflineTreeIndex ]	= pstNewTree;

	//
	//	换班，将线下的 Tree 推向线上
	//
	m_pstTreeOn		= m_pArrTree[ uOfflineTreeIndex ];
	m_uCurrTreeIndex	= uOfflineTreeIndex;
	m_uCurrTreeDeep		= uTreeDeep;


	//	离开红灯区
	//LeaveCriticalSection( & m_oCrSecTree );
	m_bTreeSwitching = FALSE;

	return TRUE;
}

UINT CDeACHashTree::PrefixMatch( LPCTSTR lpcszText, UINT uTextLen )
{
	//
	//	返回匹配到的长度
	//

	if ( NULL == lpcszText || 0 == uTextLen )
	{
		return 0;
	}
	if ( m_bTreeSwitching )
	{
		//	如果线上线下两颗树正在交接工作，则直接返回
		return 0;
	}
	if ( NULL == m_pstTreeOn )
	{
		return 0;
	}

	UINT i;
	UINT nMax;

	UINT uDeep;
	UINT uAddStep;
	UCHAR ucCharL, ucCharH;
	WORD  wPosVal;


	//	进入红灯区
	//EnterCriticalSection( & m_oCrSecTree );


	//	扫描的最大深度
	nMax	= min( ( m_uCurrTreeDeep + 1 ), uTextLen );
	i	= 0;
	uDeep	= 0;

	while( i < nMax )
	{
		//	默认每次 +1
		uAddStep = 1;
		ucCharH  = (UCHAR)lpcszText[ i ];
		if ( ucCharH > 0x80 && i < ( nMax - 1 ) )
		{
			//	本次取了两位，所以要 +2
			uAddStep = 2;
			ucCharL  = (UCHAR)lpcszText[ ( i + 1 ) ];
			wPosVal	 = MAKEWORD( ucCharL, ucCharH );
		}
		else
		{
			wPosVal	 = ucCharH;
		}

		if ( 0 == m_pstTreeOn[ uDeep ].btNode[ wPosVal ]  )
		{
			break;
		}

		//	移动指针位置
		i += uAddStep;

		//	增加扫描深度
		uDeep ++;
	}


	//	离开红灯区
	//LeaveCriticalSection( & m_oCrSecTree );


	return i;
}




//////////////////////////////////////////////////////////////////////////
//	Private

