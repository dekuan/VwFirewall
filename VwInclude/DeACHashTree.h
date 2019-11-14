/*
 ************************************************************
 *
 *	DeACHashTree.h
 *
 *	CLASS NAME:
 *		Desafe Ascii Code Hash Tree
 *		提供快速前匹配查找的算法
 *		* 仅仅支持 Ascii 编码
 *		* 最适合小型词库，并且每个词都很短的情况
 *
 *	AUTHOR:
 *		刘其星 liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-09-01 刘其星 liuqixing@gmail.com 完成
 *
 ************************************************************
 */

#ifndef __DEACHASHTREE_HEADER__
#define __DEACHASHTREE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "VwInfoMonitorConfigFile.h"

#include <vector>
using namespace std;


/**
 *	define
 */
#define DEACHASHTREE_GETCHARSEX( cChar )	( cChar >= 0 ? 1 : 0 )
#define DEACHASHTREE_MAKEINT( low, high )	( ( (CHAR)(low) ) + ( ( (CHAR)(high) ) << 8 ) )

//	最大词长度(按照字节计算，一个中文 2 个字节)
//#define DEACHASHTREE_MAX_TREE_DEEP		CVWINFOMONITORCONFIGFILE_MAX_WORDLEN



/**
 *	struct
 */
typedef struct tagDeACHashTreeNode
{
	tagDeACHashTreeNode()
	{
		memset( this, 0, sizeof(tagDeACHashTreeNode) );
	}

	BYTE btNode[ 0x10000 ];	//	满足 0 ~ 0xFFFF 个位置

}STDEACHASHTREENODE, *PSTDEACHASHTREENODE;

/*
typedef struct tagDeACHashTreeWord
{
	tagDeACHashTreeWord()
	{
		memset( this, 0, sizeof(tagDeACHashTreeWord) );
	}

	UINT uLen;
	CHAR szWord[ MAX_PATH ];

}STDEACHASHTREEWORD, *PSTDEACHASHTREEWORD;
*/




/**
 *	class of CDeACHashTree
 */
class CDeACHashTree :
	public CVwInfoMonitorConfigFile
{
public:
	CDeACHashTree();
	virtual ~CDeACHashTree();

public:
	VOID SetSoftRegStatus( BOOL bReged );
	BOOL BuildTree( LPCTSTR lpcszFilename );
	UINT GetOnlineTreeIndex();
	UINT GetOfflineTreeIndex();
	BOOL PushNewTreeOnline( STDEACHASHTREENODE * pstNewTree, UINT uTreeDeep );

	UINT PrefixMatch( LPCTSTR lpcszText, UINT uTextLen );


private:
	BOOL m_bSoftReged;			//	m_bSoftReged

	CRITICAL_SECTION m_oCrSecTree;

	BOOL m_bTreeSwitching;			//	是否正在 置换 两颗线上线下的树
	UINT m_uCurrTreeIndex;			//	0 or 1 表示当前正在工作的树
	UINT m_uCurrTreeDeep;			//	当前树的深度
	STDEACHASHTREENODE * m_pArrTree[ 2 ];	//	两颗实际的树
	STDEACHASHTREENODE * m_pstTreeOn;	//	指向正在工作的树
};




#endif // __DEACHASHTREE_HEADER__
