#ifndef __VWIAREGEX_HEADER__
#define __VWIAREGEX_HEADER__

/**
 *	结构体定义
 */
typedef struct tagHBLink
{
	tagHBLink()
	{
		memset( this, 0, sizeof(tagHBLink) );
	}
	TCHAR szName[ MAX_PATH ];
	TCHAR szUrl[ MAX_PATH ];
	BOOL  bNewItem;
	
}STHBLINK, *PSTHBLINK;

typedef struct tagVwIARegexLink
{
	tagVwIARegexLink()
	{
		memset( this, 0, sizeof(tagVwIARegexLink) );
	}

	TCHAR szItemId[ 32 ];
	TCHAR szName[ MAX_PATH ];
	TCHAR szUrl[ MAX_PATH ];
	BOOL  bNewItem;

}STVWIAREGEXLINK, *PSTVWIAREGEXLINK;



//////////////////////////////////////////////////////////////////////////
//	...
//
INT __stdcall vwia_regex_parse_tmall_items( LPCTSTR lpcszContent, STVWIAREGEXLINK * pstLinkList );





#endif	// __VWIAREGEX_HEADER__