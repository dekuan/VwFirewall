// VwACDaTrie.h: interface for the VwACDaTrie class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWACDATRIE_HEADER__
#define __VWACDATRIE_HEADER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "VwTDaTrieAc.h"
#include "VwInfoMonitorConfigFile.h"



/**
 *	class of VwACDaTrie
 */
class CVwACDaTrie :
	public CVwInfoMonitorConfigFile
{
public:
	CVwACDaTrie();
	virtual ~CVwACDaTrie();

	VOID SetSoftRegStatus( BOOL bReged );	
	BOOL BuildDict( LPCTSTR lpcszWordsFile, LPCTSTR lpcszDictFile, vector<STVWINFOMWORD> * pvcWordList = NULL );
	BOOL LoadDict( LPCTSTR lpcszDictFile );

	UINT PrefixMatch( LPCTSTR lpcszText );
	UINT PrefixMatch( LPCTSTR lpcszText, UINT uTextLen );

private:
	UINT GetOnlineTreeIndex();
	UINT GetOfflineTreeIndex();
	BOOL PushNewTreeOnline();

private:

	BOOL m_bSoftReged;			//	m_bSoftReged
	
	CRITICAL_SECTION m_oCrSecTree;
	
	BOOL m_bLoadingDict;			//	是否正在 置换 两颗线上线下的树
	UINT m_uCurrTreeIndex;			//	0 or 1 表示当前正在工作的树

	CVwTDaTrieAc<INT> m_cArrWdaTrie[ 2 ];	//	两颗实际的树
	CVwTDaTrieAc<INT> * m_pcWdaTrie;	//	当前正在工作的树
};



#endif // __VWACDATRIE_HEADER__
