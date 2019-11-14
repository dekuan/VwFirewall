// VwLanguage.h: interface for the CVwLanguage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWLANGUAGE_HEADER__
#define __VWLANGUAGE_HEADER__

#pragma warning(disable: 4786)

#include <vector>
using namespace std;


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define CVWLANGUAGE_DEFAULT_LANG	_T("English-US")


//
//	Learn more about <Language Identifiers and Locales>
//	http://msdn.microsoft.com/en-us/library/ms903928.aspx
//

//
//	Language Identifiers and Locales
//
typedef struct tagVwLanguageLocaleID
{
	DWORD dwLocaleID;		//	Locale identifier
	TCHAR * lpszEnglishName;	//	English name

}STVWLANGUAGELOCALEID, *LPSTVWLANGUAGELOCALEID;

static STVWLANGUAGELOCALEID g_ArrLanguageLocaleID[] = 
{
	{ 0x0416, _T("Portuguese") },
	{ 0x0419, _T("Russian") },
	{ 0x0804, _T("Chinese-Simplified") },
	{ 0x0404, _T("Chinese-Traditional") },
	{ 0xFFFF, _T("Chinese-Uyghurche") },
	{ 0x0804, _T("Chinese-Kazakh") },
	{ 0x0411, _T("Japanese") },
	{ 0x080a, _T("Spanish") },
	{ 0x040a, _T("Spanish-Traditional") },
	{ 0x0413, _T("Dutch") },
	{ 0x0407, _T("German") },
	{ 0x041f, _T("Turkish") },
	{ 0x040c, _T("French") },
	{ 0x0410, _T("Italian") },
	{ 0x0418, _T("Romanian") },
	{ 0x0406, _T("Danish") },
	{ 0x040d, _T("Hebrew") },
	{ 0x041d, _T("Swedish") },
	{ 0x0421, _T("Indonesian") },
	{ 0x0439, _T("Hindi-India") },
	{ 0x0415, _T("Polish") },
	{ 0x040b, _T("Finnish") },
	{ 0x0412, _T("Korean") },
	{ 0x0401, _T("Arabic") },
	{ 0x042a, _T("Vietnamese") },
	{ 0x0403, _T("Catalan") },
	{ 0x042c, _T("Azerbaijani") },
	{ 0x0422, _T("Ukrainian") },
	{ 0x040e, _T("Hungarian") },
	{ 0x041e, _T("Thai") },
	{ 0x0402, _T("Bulgarian") },
	{ 0x0405, _T("Czech") },
	{ 0x0c1a, _T("Serbian-Cyrillic") },
	{ 0x081a, _T("Serbian-Latin") },
	{ 0x042f, _T("Macedonian") },
	{ 0x0408, _T("Greek") },
	{ 0x041a, _T("Croatian") },
	{ 0x0414, _T("Norwegian") },
	{ 0x0437, _T("Galician") },
	{ 0x041c, _T("Albanian") },
	{ 0x042d, _T("Basque") },
	{ 0x0427, _T("Lithuanian") },
	{ 0x041b, _T("Slovak") },
	{ 0xFFFF, _T("Geogrian") },
	{ 0x0425, _T("Estonian") },
	{ 0xFFFF, _T("Kurdish") },
	{ 0x141a, _T("Bosnian") },
	{ 0x3409, _T("Philippines") },
	{ 0x0424, _T("Slovenian") }
};
static INT g_nLanguageLocaleIDCount = sizeof(g_ArrLanguageLocaleID)/sizeof(g_ArrLanguageLocaleID[0]);


/**
 *	struct
 */
typedef struct tagVwLanguageList
{
	tagVwLanguageList()
	{
		memset( this, 0, sizeof(tagVwLanguageList) );
	}
	
	TCHAR szLangLocName[ 128 ];	//	语言名称
	TCHAR szLangLocCode[ 64 ];	//	语言code
	TCHAR szLangEngName[ 64 ];	//	语言的英文名称，也是文件名的组成部分
	DWORD dwLangLocID;		//	Local ID，例如简体中文是 0x0804
	TCHAR szFilepath[ MAX_PATH ];	//	语言文件全路径
	TCHAR szFilename[ 64 ];		//	语言文件名
	INT   nLangMenuID;		//	菜单上的 MenuID

}STVWLANGUAGELIST, *LPSTVWLANGUAGELIST;

/**
 *	class of CVwLanguage
 */
class CVwLanguage
{
public:
	CVwLanguage();
	virtual ~CVwLanguage();

	//
	//	language list
	//
	BOOL  LoadLangList( LPCTSTR lpctszDir );
	DWORD GetLangListCount();

	//
	//	language map
	//
	VOID  SetCurrentLang( STVWLANGUAGELIST * pstLang );
	VOID  SetCurrentLang( LPCTSTR lptszLangEngName );
	BOOL  GetLangText( LPCTSTR lpctszID, LPTSTR lptszText, DWORD dwSize );
	BOOL  SetLangForDlgItem( LPCTSTR lpctszID, HWND hWnd );

private:
	BOOL ScanFiles( LPCTSTR lpctszDir );
	VOID GetLocalInfo();

public:
	STVWLANGUAGELIST m_stCurrentLang;
	TCHAR * m_pszLocLangEnglishName;

	vector<STVWLANGUAGELIST> m_vcLangList;
	vector<STVWLANGUAGELIST>::iterator m_itLangList;

};



#endif // __VWLANGUAGE_HEADER__
