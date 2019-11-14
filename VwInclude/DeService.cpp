#include "StdAfx.h"
//#include "resource.h"
#include "DeService.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDeService::CDeService()
{
}
CDeService::~CDeService()
{
}

//
//	启动服务
//
LONG CDeService::Install( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvExeFilename, IN LPCTSTR lpcszSrvDesc, IN BOOL bInteractive /* =TRUE */ )
{
	//
	//	lpcszSrvName		- [in] 服务名
	//	lpcszSrvExeFilename	- [in] 服务可执行文件名
	//	lpcszSrvDesc		- [in] 服务描述
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszSrvName || NULL == lpcszSrvExeFilename || NULL == lpcszSrvDesc )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) || 0 ==_tcslen(lpcszSrvExeFilename) )
	{
		return DESERVICE_ERROR_PARAM;
	}
	
	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	DWORD dwServiceType;

	if ( bInteractive )
	{
		//	interactive to win32 logon desktop
		dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	}
	else
	{
		dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	}

	schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE );
	if ( NULL != schSCManager )
	{
		schService = CreateService
		(
			schSCManager,			//	SCManager database
			lpcszSrvName,			//	name of service
			lpcszSrvName,			//	service name to display
			SERVICE_ALL_ACCESS,		//	desired access
			dwServiceType,			//	service type, org = "SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS"
			SERVICE_AUTO_START,		//	start type
			SERVICE_ERROR_NORMAL,		//	error control type
			lpcszSrvExeFilename,		//	service's binary
			NULL,				//	no load ordering group
			NULL,				//	no tag identifier
			NULL,				//	no dependencies
			NULL,				//	LocalSystem account
			NULL				//	no password
		);
		if ( NULL != schService )
		{
			lnRet = DESERVICE_ERROR_SUCC;

			//	Set Service Description
			SetDescription( lpcszSrvName, lpcszSrvDesc );

			CloseServiceHandle( schService );
		}
		else
		{
			lnRet = DESERVICE_ERROR_CREATE_SERVICE;
		}

		CloseServiceHandle( schSCManager );
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	启动服务
//
LONG CDeService::Uninstall( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] 服务名
	//	lpszError	- [out] 错误输出
	//	RETURN		- TRUE / FALSE
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if ( NULL != schSCManager )
	{
		SC_HANDLE schService = OpenService( schSCManager, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != schService )
		{
			if ( DeleteService( schService ) )
			{
				//	成功
				lnRet = DESERVICE_ERROR_SUCC;
			}
			else
			{
				//	卸载服务失败
				lnRet = DESERVICE_ERROR_DELETE_SERVICE;
			}

			CloseServiceHandle( schService ); 
		}
		else
		{
			//	服务不存在
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( schSCManager );
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	启动服务
//
LONG CDeService::Start( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] 服务名
	//	lpszError	- [out] 错误输出
	//	RETURN		- DESERVICE_ERROR_SUCC or ...
	//
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	
	scm = OpenSCManager( NULL,NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_START );
		if ( NULL != svc )
		{
			if ( StartService( svc, 0, NULL ) )
			{
				lnRet = DESERVICE_ERROR_SUCC;
			}
			else
			{
				lnRet = DESERVICE_ERROR_START_SERVICE;
			}
			CloseServiceHandle( svc );
		}
		else
		{
			//	服务不存在
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle(scm);
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	启动服务
//
LONG CDeService::Stop( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] 服务名
	//	RETURN		- DESERVICE_ERROR_SUCC or ...
	//
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS ServiceStatus;
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			if ( QueryServiceStatus( svc, &ServiceStatus ) )
			{
				if ( SERVICE_RUNNING == ServiceStatus.dwCurrentState )
				{
					if ( ControlService( svc, SERVICE_CONTROL_STOP, &ServiceStatus ) )
					{
						//	停止成功
						lnRet = DESERVICE_ERROR_SUCC;
					}
					else
					{
						//	停止失败
						lnRet = DESERVICE_ERROR_STOP_SERVICE;
					}
				}
				else
				{
					//	服务已经停止了
					lnRet = DESERVICE_ERROR_SERVICE_ALREADY_STOPED;
				}
			}
			else
			{
				//	查询服务状态失败
				lnRet = DESERVICE_ERROR_QUERY_SERVICE_STATUS;
			}
			CloseServiceHandle( svc );
		}
		else
		{
			//	服务不存在
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	获取服务状态
//
LONG CDeService::GetStatus( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] 服务名称
	//	RETURN		- SERVICE_STOPPED | SERVICE_START_PENDING | SERVICE_STOP_PENDING,
	//			  SERVICE_RUNNING | SERVICE_CONTINUE_PENDING | SERVICE_PAUSE_PENDING | SERVICE_PAUSED
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;
	SERVICE_STATUS ServiceStatus;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			if ( QueryServiceStatus( svc, &ServiceStatus ) )
			{
				lnRet = ServiceStatus.dwCurrentState;
			}
			else
			{
				lnRet = DESERVICE_ERROR_QUERY_SERVICE_STATUS;
			}

			CloseServiceHandle( svc );
		}
		else
		{
			//	服务不存在
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	获取服务程序的完整路径
//
LONG CDeService::GetFilepath( IN LPCTSTR lpcszSrvName, OUT LPTSTR lpszSrvExeFilename, IN DWORD dwSize )
{
	//
	//	lpcszSrvName		- [in] 服务名称
	//	lpszSrvExeFilename	- [out] 服务器程序完整路径，可能包括参数
	//	dwSize			- [in] size of lpszSrvExeFilename
	//	RETURN			- DESERVICE_ERROR_SUCC or ...
	//
	
	if ( NULL == lpcszSrvName )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	LONG lnRet = DESERVICE_ERROR_UNKNOWN;
	SC_HANDLE scm;
	SC_HANDLE svc;

	BOOL bQuerySucc				= FALSE;
	LPQUERY_SERVICE_CONFIG lpServiceConfig	= NULL;
	DWORD dwBytesNeeded			= 0;


	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			//
			//	Query Service Config
			//
			lpServiceConfig	= (LPQUERY_SERVICE_CONFIG)LocalAlloc( LPTR, 1 );
			if ( lpServiceConfig )
			{
				if ( 0 == QueryServiceConfig( svc, lpServiceConfig, 1, & dwBytesNeeded ) )
				{
					LocalFree( lpServiceConfig );
					lpServiceConfig = NULL;

					lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc( LPTR, dwBytesNeeded );
					if ( lpServiceConfig )
					{
						if ( QueryServiceConfig( svc, lpServiceConfig, dwBytesNeeded, & dwBytesNeeded ) )
						{
							lnRet = DESERVICE_ERROR_SUCC;

							//	set return value
							_sntprintf( lpszSrvExeFilename, dwSize-sizeof(TCHAR), _T("%s"), lpServiceConfig->lpBinaryPathName );
						}
						
						LocalFree( lpServiceConfig );
						lpServiceConfig = NULL;
					}
				}
			}
			
			CloseServiceHandle( svc );
		}
		else
		{
			//	服务不存在
			lnRet = DESERVICE_ERROR_OPEN_SERVICE;
		}
		CloseServiceHandle( scm );
	}
	else
	{
		//	无法打开服务控制台
		lnRet = DESERVICE_ERROR_OPEN_SC_MANAGER;
	}
	
	return lnRet;
}

//
//	检查服务是否存在
//
BOOL CDeService::IsExist( IN LPCTSTR lpcszSrvName )
{
	//
	//	lpcszSrvName	- [in] 服务名称
	//	RETURN		- 1 / 0
	//

	if ( NULL == lpcszSrvName )
	{
		return FALSE;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	SC_HANDLE scm;
	SC_HANDLE svc;
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( NULL != scm )
	{
		svc = OpenService( scm, lpcszSrvName, SERVICE_ALL_ACCESS );
		if ( NULL != svc )
		{
			bRet = TRUE;
			CloseServiceHandle( svc );
		}

		CloseServiceHandle( scm );
	}

	return bRet;
}

//
//	设置服务描述
//
LONG CDeService::SetDescription( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvDesc )
{
	//
	//	lpcszSrvName	- [in] 服务名
	//	lpcszSrvDesc	- [in] 服务描述
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszSrvName || NULL == lpcszSrvDesc )
	{
		return DESERVICE_ERROR_PARAM;
	}
	if ( 0 == _tcslen(lpcszSrvName) )
	{
		return DESERVICE_ERROR_PARAM;
	}

	TCHAR szKeyPath[ MAX_PATH ]		= {0};
	TCHAR szKeyValue[ MAX_PATH ]		= {0};
	DWORD dwSize				= 0;
	
	//	添加服务的描述信息
	_sntprintf
	(
		szKeyPath, sizeof(szKeyPath)-sizeof(TCHAR), 
		_T("SYSTEM\\ControlSet001\\Services\\%s"), _T(lpcszSrvName)
	);
	_sntprintf( szKeyValue, sizeof(szKeyValue)-sizeof(TCHAR), _T("%s"), lpcszSrvDesc );
	dwSize = sizeof(szKeyValue);
	SHSetValue( HKEY_LOCAL_MACHINE, _T(szKeyPath), _T("Description"), REG_SZ, szKeyValue, dwSize );

	return DESERVICE_ERROR_SUCC;
}
