/*--------------------------------------------------------------------------------
 * CVwIniFile
 *
 * Update:
 *		2003-5-30 Create by liuqixing
 *		2010-5-23 Update by liuqixing
 *--------------------------------------------------------------------------------*/
#ifndef __VWINIFILE_HEADER__
#define __VWINIFILE_HEADER__

//////////////////////////////////////////////////////////////////////////////////////////
//
// 用户接口说明: 在成员函数SetVarStr和SetVarInt函数中,
// iType == 0 如果用户制定的参数在ini文件中不存在,则就写入新的变量.
// iType != 0 如果用户制定的参数在ini文件中不存在,就不写入新的变量,而是直接返回FALSE;
//
//////////////////////////////////////////////////////////////////////////////////////////


//#include "afxtempl.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


typedef struct tagVwIniFileArray
{
	TCHAR szItem[ MAX_PATH ];
	UINT  uLength;

}STVWINIFILEARRAY, *LPSTVWINIFILEARRAY;



class CVwIniFile
{
public:
	CVwIniFile();			//构造函数
	virtual ~CVwIniFile();		//析构函数

private:
	CVwIniFile( const CVwIniFile & );	//构造函数重载
	CVwIniFile & operator = ( const CVwIniFile & );

public:
	//	创建函数
	virtual BOOL Create( LPCTSTR lpcszFileName );


	//	得到变量整数型数值
	virtual INT  GetMyPrivateProfileInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT nDefaultValue );
	virtual BOOL GetVarInt( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, INT & nValue );
	//	重新设置变量整数型数值
	//virtual BOOL SetVarInt( const CString &,const CString & ,const int &,const int iType = 1 );


	//	得到变量字符串型数值
	virtual BOOL GetMyPrivateProfileString( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPCTSTR lpcszDefaultValue, LPTSTR lpszRetValue, DWORD dwSize );
	virtual BOOL GetVarStr( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize );
	//	重新设置变量字符串型数值
	//virtual BOOL SetVarStr( const CString &,const CString &, const CString &,const int iType = 1 );


	//	删除某个 KEY
	//virtual BOOL DelKey( const CString & strSection,const CString & strVarInfo );
	//	删除整个 [Section]
	//virtual BOOL DelSection( const CString & strSection );


	//	对某个 [Section] 中的数据 KEY 按照一定 KEY NAME 重新排序
	//virtual BOOL OrderKey( const CString & strSection, const CString & strByNewKeyName, const int & nStart );


	//	获得某个 [Section] 的变量个数
	//virtual int SearchVarNum( const CString & );

	//	判断某个 [Section] 是否存在
	//virtual BOOL IsSectionExist( const CString & strSection );

	//	获取某个 [Section] 中第 nIndex 个 KEY 的名字，定义第一个 strVarName 的 nIndex=1
	//virtual BOOL GetVarName( const CString & strSection, CString & strReturnValue, const int & nIndex );


private:
	BOOL GetVar( LPCTSTR lpcszSection, LPCTSTR lpcszVarName, LPTSTR lpszReturnValue, DWORD dwSize );
	//BOOL SetVar( const CString &, const CString &, const CString &, const INT iType = 1 );
	INT  SearchLine( LPCSTR lpcszSection, LPCSTR lpcszVarName );
	//INT  SearchLine( const CString & strSection );

private:
//	vector <CString>  FileContainer;
	//CArray <CString,CString> m_caFileContainer;
	vector<STVWINIFILEARRAY> m_caFileContainer;
	vector<STVWINIFILEARRAY>::iterator m_it;
	BOOL  m_bFileExsit;
	BOOL  m_bModified;
	//CStdioFile m_cStdioFile;
	TCHAR m_szInIFileName[ MAX_PATH ];

};

#endif	// __VWINIFILE_HEADER__