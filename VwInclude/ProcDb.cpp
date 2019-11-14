// CProcDb.cpp: implementation of the CProcDb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcDb.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 替换字符串里面的一个子串
void mystrchg(char* dest, int len, char* rstr)
{
	int rlen = strlen(rstr);
	memmove(dest+rlen, dest+len, strlen(dest+len)+1);
	memcpy(dest, rstr, rlen);
}

// 无符号搜索子串
char* mystristr(char* src, char* dst)
{
	for(;(strlen(src)-strlen(dst) >= 0 && *src);src++)
	{
		if (!strnicmp(src, dst, strlen(dst)))
			return src;
	}
	return NULL;
}

// 替换目标字符串里面的子串
void myreplace(char* src, char* sFindStr, char* sRepWithStr )
{
	for ( char * pos = src; ( pos = mystristr( pos, sFindStr ) ) != NULL; pos = pos + strlen( sRepWithStr ) )
	{
		mystrchg(pos, strlen(sFindStr), sRepWithStr);
	}
}





CProcDb::CProcDb( BOOL bShowMessageBox /* = TRUE */ )
{
	//
	//	是否显示 MessageBox 错误提示
	//
	m_bShowMessageBox = bShowMessageBox;

	m_conn	= NULL;

	m_vtNull.vt = VT_NULL;
	memset( m_szDbPath, 0, sizeof(m_szDbPath) );

	::CoInitialize(NULL);
}

CProcDb::~CProcDb()
{
	if ( NULL != m_conn )
	{
		m_conn->Close();
		m_conn = NULL;
	}

//	m_vtNull.vt = VT_NULL;
//	memset( m_szDbPath, 0, sizeof(m_szDbPath) );

	::CoUninitialize();
}


//	设置是否显示 MessageBox
VOID CProcDb::SetShowMessageBox( BOOL bShowMessageBox )
{
	m_bShowMessageBox = bShowMessageBox;
}


//////////////////////////////////////////////////////////////////////////
// 使得文件可以写
BOOL CProcDb::MakeFileWritable( LPCTSTR lpszFilePath )
{
	BOOL bRet = FALSE;
	if ( lpszFilePath && _tcslen(lpszFilePath) > 0 && PathFileExists(lpszFilePath) )
	{
		DWORD dwFileAttrs = GetFileAttributes( lpszFilePath );
		dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;	// 去除只度属性
		dwFileAttrs &= ~FILE_ATTRIBUTE_SYSTEM;		// 去除系统属性
		dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;		// 去除隐藏属性
		bRet = SetFileAttributes( lpszFilePath, dwFileAttrs );
	}
	return bRet;
}

BOOL CProcDb::IsFileWritable( LPCTSTR lpszFilePath )
{
	BOOL bRet = FALSE;
	if ( lpszFilePath && _tcslen(lpszFilePath) > 0 && PathFileExists(lpszFilePath) )
	{
		DWORD dwFileAttrs	= GetFileAttributes( lpszFilePath );
		//
		bRet = ! ( FILE_ATTRIBUTE_READONLY == ( dwFileAttrs & FILE_ATTRIBUTE_READONLY ) );
	}
	return bRet;
}

BOOL CProcDb::IsDbFileWritable()
{
	if ( 0 == strlen( m_szDbPath ) )
		return FALSE;
	if ( ! PathFileExists( m_szDbPath ) )
		return FALSE;

	return IsFileWritable( m_szDbPath );
}


/**
 *	连接数据库
 */
BOOL CProcDb::ConnDatabase( LPCTSTR lpcszDataFile )
{
	if ( NULL == lpcszDataFile || 0 == _tcslen(lpcszDataFile) )
	{
		return FALSE;
	}
	if ( ! PathFileExists(lpcszDataFile) )
	{
		return FALSE;
	}
	if ( IsConnDatabase() )
	{
		//	数据库已经连接就直接返回
		return TRUE;
	}

	BOOL bRet = FALSE;
	HRESULT hRet		= 0;

	//	save data file path to class member var
	_sntprintf( m_szDbPath, sizeof(m_szDbPath)-sizeof(TCHAR), "%s", lpcszDataFile );

	//	..
	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s", lpcszDataFile );

	try
	{
		//	make file writable
	//	MakeFileWritable( lpcszDataFile );

		//	try to connection access database using ado
		hRet = m_conn.CreateInstance( __uuidof(Connection) );
		if ( SUCCEEDED(hRet) )
		{
			hRet = m_conn->Open( _bstr_t(m_szSql), "", "", adModeUnknown );
			if ( SUCCEEDED(hRet) )
			{
				bRet = TRUE;
				// 上面一句中连接字串中的Provider是针对ACCESS2000环境的，对于ACCESS97,需要改为:Provider=Microsoft.Jet.OLEDB.3.51;  }
				m_rs.CreateInstance( __uuidof(Recordset) );
			}
		}
	}
	catch ( _com_error e )
	{
		if ( m_bShowMessageBox )
		{
			MessageBox( NULL, "Error in ConnDatabase, falied to connect config database.", "EXCEPTION_EXECUTE_HANDLER", MB_ICONWARNING );
		}
	}

	return bRet;
}

BOOL CProcDb::IsConnDatabase()
{
	return ( NULL == m_conn ) ? FALSE : TRUE;
}

VOID CProcDb::CloseDatabase()
{
	if ( m_conn )
	{
		m_conn->Close();
	}

	m_conn	= NULL;
	m_rs	= NULL;
}

BOOL CProcDb::GetSafeString( LPCTSTR lpcszStr, LPTSTR lpszNewStr, DWORD dwSize )
{
	if ( NULL == lpcszStr || NULL == lpszNewStr )
		return FALSE;

	UINT i = 0;

	_sntprintf( lpszNewStr, dwSize-sizeof(TCHAR), "%s", lpcszStr );
	for ( i = 0; i < _tcslen(lpszNewStr); i ++ )
	{
		if ( '\'' == lpszNewStr[i] )
			lpszNewStr[i] = ' ';
	}

	return TRUE;
}

VOID CProcDb::KillBadChar( LPSTR lpszStr )
{
	if ( lpszStr )
	{
		myreplace( lpszStr, "'", "" );
	}
}

VOID CProcDb::DecodeBadChar( LPSTR lpszStr )
{
	if ( lpszStr )
	{
		myreplace( lpszStr, "''", "'" );
	}
}


/**
 *	压缩数据库
 */
BOOL CProcDb::CompactDatabase()
{
	return TRUE;
}

/**
 *	检查某个表是否存在
 */
BOOL CProcDb::IsExistTable( LPCSTR lpcszTableName )
{
	/*
		lpcszTableName	- 表名字
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpcszTableName )
	{
		return FALSE;
	}
	if ( 0 == strlen( lpcszTableName ) )
	{
		return FALSE;
	}
	if ( ! CProcDb::IsConnDatabase() )
	{
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	_RecordsetPtr myrs;
	TCHAR szTabName[ MAX_PATH ];

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM MSysObjects WHERE Flags=0 AND Type=1" );

	try
	{
		if ( S_OK == myrs.CreateInstance( __uuidof(Recordset) ) )
		{
			if ( S_OK == myrs->Open( _variant_t(m_szSql), m_conn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText ) )
			{
				while( ! myrs->adoEOF )
				{
					memset( szTabName, 0, sizeof(szTabName) );
					QueryStringValue( myrs, "Name", szTabName, sizeof(szTabName) );

					if ( _tcslen( szTabName ) && 0 == _tcsicmp( szTabName, lpcszTableName ) )
					{
						bRet = TRUE;
						break;
					}
					//	..
					myrs->MoveNext();
				}

				myrs->Close();
			}
			myrs.Release();
		}
	}
	catch ( _com_error e )
	{
	}
	
	return bRet;
}

/**
 *	检查某个表的某个字段是否存在
 */
BOOL CProcDb::IsExistColumn( LPCSTR lpcszTableName, LPCSTR lpcszColumnName )
{
	/*
		lpcszTableName	- 表名字
		lpcszColumnName	- 字段名称
		RETURN		- TRUE / FALSE
	*/

	if ( NULL == lpcszTableName || NULL == lpcszColumnName )
		return FALSE;
	if ( 0 == strlen( lpcszTableName ) || 0 == strlen( lpcszColumnName ) )
		return FALSE;

	if ( ! CProcDb::IsConnDatabase() )
		return FALSE;

	BOOL bRet			= FALSE;
	_RecordsetPtr myrs;
	VARIANT varindex;
	FieldsPtr oFields;
	FieldPtr oField;
	INT nFieldCount;
	_bstr_t bstrFieldname;
	INT i				= 0;

	_sntprintf( m_szSql, sizeof(m_szSql)-sizeof(TCHAR), "SELECT * FROM %s", lpcszTableName );

	try
	{
		if ( S_OK == myrs.CreateInstance( __uuidof(Recordset) ) )
		{
			if ( S_OK == myrs->Open( _variant_t(m_szSql), m_conn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText ) )
			{
				VariantInit( &varindex );
				varindex.vt	= VT_I2;
				oFields		= myrs->GetFields();
				if ( oFields )
				{
					nFieldCount	= oFields->GetCount();
					for ( i = 0; i < nFieldCount; i++ )
					{
						varindex.iVal	= i;
						oField		= oFields->GetItem( varindex );
						bstrFieldname	= oField->GetName();
						if ( (LPCTSTR)bstrFieldname && 0 == _tcsicmp( (LPCTSTR)bstrFieldname, lpcszColumnName ) )
						{
							bRet = TRUE;
							break;
						}
					}
				}

				myrs->Close();
			}
			myrs.Release();
		}
	}
	catch ( _com_error e )
	{
	}

	return bRet;
}

LONG CProcDb::QueryIntValue( _RecordsetPtr rs, LPCSTR lpcszKey )
{
	LONG lnRet = -1;
	TCHAR szTemp[ MAX_PATH ]	= {0};

	if ( ! rs->adoEOF && lpcszKey && strlen(lpcszKey) > 0 )
	{
		try
		{
			m_varValue = rs->GetCollect( lpcszKey );
			if ( m_varValue != m_vtNull )
			{
				lnRet = long( m_varValue );
			}
		}
		catch ( _com_error e )
		{
			if ( m_bShowMessageBox )
			{
				_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "Error in QueryStringValue (%s)", lpcszKey );
				MessageBox( NULL, szTemp, "Error in QueryIntValue", MB_ICONWARNING );
			}
		}
	}
	return lnRet;
}

VOID CProcDb::QueryStringValue( _RecordsetPtr rs, LPCSTR lpcszKey, LPTSTR lpszValue, DWORD dwSize )
{
	TCHAR szTemp[ MAX_PATH ]	= {0};

	if ( ! rs->adoEOF && lpcszKey && strlen(lpcszKey) > 0 && lpszValue )
	{
		try
		{
			m_varValue = rs->GetCollect( lpcszKey );
			if ( m_varValue != m_vtNull )
			{
				_sntprintf( lpszValue, dwSize-sizeof(TCHAR), "%s", ((char*)(_bstr_t)(m_varValue)) );
			}
		}
		catch ( _com_error e )
		{
			if ( m_bShowMessageBox )
			{
				_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "Error in QueryStringValue (%s)", lpcszKey );
				MessageBox( NULL, szTemp, "Error in QueryStringValue", MB_ICONWARNING );
			}
		}
	}
}

/**
 *	执行 SQL 语句
 */
BOOL CProcDb::ExectueSql( LPCSTR lpcszSql, BOOL bAlertError /*TRUE*/ )
{
	if ( ! IsConnDatabase() )
	{
		return FALSE;
	}
	if ( ! IsDbFileWritable() )
	{
		return FALSE;
	}


	TCHAR szTemp[ 1024 ];

	if ( lpcszSql && strlen(lpcszSql) > 0 )
	{
		try
		{
			if ( m_conn )
			{
				m_conn->Execute( (_bstr_t)(lpcszSql), &m_RecordsAffected, adCmdText );
			}

			return TRUE;

			/*
			if ( m_conn )
			{
				_CommandPtr pCommand;
				pCommand.CreateInstance( __uuidof(Command) );
				pCommand->ActiveConnection	= m_conn;
				pCommand->CommandText		= lpcszSql;
				pCommand->Execute( NULL, NULL, adCmdText );
			}
			return TRUE;
			*/
		}
		catch ( _com_error e )
		{
			if ( bAlertError )
			{
				if ( m_bShowMessageBox )
				{
					_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "Error in ExectueSql (%s)", lpcszSql );
					MessageBox( NULL, szTemp, "Error in ExectueSql", MB_ICONWARNING );
				}
			}
			return FALSE;
		}
	}
	return FALSE;
}










































































