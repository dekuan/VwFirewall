#include "stdafx.h"
#include "DeDriverManager.h"



CDeDriverManager::CDeDriverManager()
{
	m_hDriver	= NULL;
	m_bRemovable	= TRUE;

	memset( m_szDriverName, 0, sizeof(m_szDriverName) );
	memset( m_szDriverPath, 0, sizeof(m_szDriverPath) );
	memset( m_szDriverDosName, 0, sizeof(m_szDriverDosName) );

	m_bInitialized	= FALSE;
	m_bLoaded	= FALSE;
	m_bStarted	= FALSE;
}

CDeDriverManager::~CDeDriverManager()
{
	if ( m_hDriver )
	{
		CloseHandle( m_hDriver ); 
		m_hDriver = NULL; 
	}
	UnloadDriver();
}

VOID CDeDriverManager::SetRemovable( BOOL bValue )
{
	m_bRemovable = bValue;
}

BOOL CDeDriverManager::IsInitialized()
{
	return m_bInitialized;
}

BOOL CDeDriverManager::IsLoaded()
{
	return m_bLoaded;
}

BOOL CDeDriverManager::IsStarted()
{
	return m_bStarted;
}

LONG CDeDriverManager::InitDriver( LPCTSTR lpcszDriverName, LPCTSTR lpcszDriverPath )
{
	if ( NULL == lpcszDriverName || NULL == lpcszDriverPath )
	{
		return DEDRIVERMANAGER_ERROR_PARAM;
	}

	if ( m_bInitialized )
	{
		//
		//	如果已经初始化过，那么要先 Unload
		//
		if ( DEDRIVERMANAGER_SUCCESS != UnloadDriver() )
		{
			return DEDRIVERMANAGER_ERROR_ALREADY_INITIALIZED;
		}
	}

	//
	//	拷贝驱动信息
	//
	_sntprintf( m_szDriverPath, sizeof(m_szDriverPath)-sizeof(TCHAR), _T("%s"), lpcszDriverPath );
	_sntprintf( m_szDriverName, sizeof(m_szDriverName)-sizeof(TCHAR), _T("%s"), lpcszDriverName );
	_sntprintf( m_szDriverDosName, sizeof(m_szDriverDosName)-sizeof(TCHAR), _T("\\\\.\\%s"), lpcszDriverName );

	//
	//	initialize successfully
	//
	m_bInitialized = TRUE;

	return DEDRIVERMANAGER_SUCCESS;
}

LONG CDeDriverManager::LoadDriver( LPCTSTR lpcszDriverName, LPCTSTR lpcszDriverPath, BOOL bStart )
{
	LONG lnRet	= DEDRIVERMANAGER_ERROR_UNKNOWN;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	BOOL bOpenService	= FALSE;	

	//
	//	InitDriver
	//
	if ( DEDRIVERMANAGER_SUCCESS == 
		InitDriver( lpcszDriverName, lpcszDriverPath ) )
	{
		if ( m_bLoaded )
		{
			lnRet = DEDRIVERMANAGER_SUCCESS;
		}
		else
		{
			hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
			if ( hSCManager )
			{
				hSCService = OpenService( hSCManager, m_szDriverName, SERVICE_ALL_ACCESS );
				if ( hSCService )
				{
					bOpenService = TRUE;
					
					CloseServiceHandle( hSCService );
					hSCService = NULL;
				}
				else
				{
					hSCService = CreateService
						(
							hSCManager,		//	hSCManager database
							m_szDriverName,		//	nombre del servicio
							m_szDriverName,		//	nombre a mostrar
							SERVICE_ALL_ACCESS,	//	acceso total
							SERVICE_KERNEL_DRIVER,	//	driver del kernel
							SERVICE_DEMAND_START,	//	comienzo bajo demanda
							SERVICE_ERROR_NORMAL,	//	control de errores normal
							m_szDriverPath,		//	path del driver
							NULL,			//	no pertenece a un grupo
							NULL,			//	sin tag
							NULL,			//	sin dependencias
							NULL,			//	cuenta local del sistema
							NULL			//	sin password
						);
					if ( hSCService )
					{
						bOpenService = TRUE;
						
						CloseServiceHandle( hSCService );
						hSCService = NULL;
					}
				}
				
				CloseServiceHandle( hSCManager );
				hSCManager = NULL;
				
				if ( bOpenService )
				{
					m_bLoaded = TRUE;
					
					if ( bStart )
					{
						lnRet = StartDriver();
					}
				}
			}
			else
			{
				lnRet = DEDRIVERMANAGER_ERROR_SCM;
			}
		}
	}

	return lnRet;
}

LONG CDeDriverManager::UnloadDriver( BOOL bForceClearData /* = FALSE */ )
{
	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;

	if ( m_bStarted )
	{
		lnRet = StopDriver();
		if ( DEDRIVERMANAGER_SUCCESS == lnRet )
		{
			if ( m_bRemovable )
			{
				hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
				if ( hSCManager )
				{
					hSCService = OpenService( hSCManager, m_szDriverName, SERVICE_ALL_ACCESS );
					if ( hSCService )
					{
						if ( DeleteService(hSCService) )
						{
							lnRet = DEDRIVERMANAGER_SUCCESS;
						}
						else
						{
							lnRet = DEDRIVERMANAGER_ERROR_REMOVING;
						}

						CloseServiceHandle( hSCService );
						hSCService = NULL;
					}
					else
					{
						lnRet = DEDRIVERMANAGER_ERROR_SERVICE;
					}

					CloseServiceHandle( hSCManager );
					hSCManager = NULL;
				}
				else
				{
					lnRet = DEDRIVERMANAGER_ERROR_SCM;
				}
			}
		}

		if ( DEDRIVERMANAGER_SUCCESS == lnRet )
		{
			m_bLoaded = FALSE;
		}
	}

	if ( m_bInitialized )
	{
		if ( DEDRIVERMANAGER_SUCCESS == lnRet || bForceClearData )
		{
			m_bInitialized = FALSE;
		}
	}

	return lnRet;
}

LONG CDeDriverManager::StartDriver()
{
	if ( m_bStarted )
	{
		return DEDRIVERMANAGER_SUCCESS;
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;

	hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if ( hSCManager )
	{
		hSCService = OpenService( hSCManager, m_szDriverName, SERVICE_ALL_ACCESS );
		if ( hSCService )
		{
			if ( StartService( hSCService, 0, NULL ) )
			{
				lnRet = DEDRIVERMANAGER_SUCCESS;
			}
			else
			{
				if ( ERROR_SERVICE_ALREADY_RUNNING == GetLastError() ) 
				{
					m_bRemovable = FALSE;
					lnRet = DEDRIVERMANAGER_SUCCESS;
				}
				else
				{
					lnRet = DEDRIVERMANAGER_ERROR_STARTING;
				}
			}

			CloseServiceHandle( hSCService );
			hSCService = NULL;
		}
		else
		{
			lnRet = DEDRIVERMANAGER_ERROR_SERVICE;
		}

		CloseServiceHandle( hSCManager );
		hSCManager = NULL;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_SCM; 
	}

	//
	//	...
	//
	if ( DEDRIVERMANAGER_SUCCESS == lnRet )
	{
		m_bStarted = TRUE;

		lnRet = OpenDriverDevice();
	}

	return lnRet;
}

LONG CDeDriverManager::StopDriver()
{
	if ( ! m_bStarted )
	{
		return DEDRIVERMANAGER_SUCCESS;
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
	SERVICE_STATUS  status;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCManager )
	{
		hSCService = OpenService( hSCManager, m_szDriverName, SERVICE_ALL_ACCESS );
		if ( hSCService )
		{
			//
			//	关闭 m_hDriver
			//
			CloseHandle( m_hDriver );
			m_hDriver = NULL;

			if ( ControlService( hSCService, SERVICE_CONTROL_STOP, &status ) )
			{
				lnRet = DEDRIVERMANAGER_SUCCESS;
			}
			else
			{
				lnRet = DEDRIVERMANAGER_ERROR_STOPPING;
			}

			CloseServiceHandle(hSCService);
			hSCService = NULL;			
		}
		else
		{
			lnRet = DEDRIVERMANAGER_ERROR_SERVICE;
		}

		CloseServiceHandle( hSCManager );
		hSCManager = NULL;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_SCM;
	}

	if ( DEDRIVERMANAGER_SUCCESS == lnRet )
	{
		m_bStarted = FALSE;
	}

	return lnRet;
}

LONG CDeDriverManager::OpenDriverDevice()
{
	if ( m_hDriver ) 
	{
		CloseHandle( m_hDriver );
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;

	m_hDriver = CreateFile
		(
			m_szDriverDosName,
			GENERIC_READ|GENERIC_WRITE,
			0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL
		);
	if ( INVALID_HANDLE_VALUE != m_hDriver )
	{
		lnRet = DEDRIVERMANAGER_SUCCESS;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_INVALID_HANDLE;
	}

	return lnRet;
}

HANDLE CDeDriverManager::GetDriverHandle()
{
	return m_hDriver;
}

LONG CDeDriverManager::WriteIo( DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize )
{
	if ( NULL == m_hDriver )
	{
		return DEDRIVERMANAGER_ERROR_INVALID_HANDLE;
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	DWORD bytesReturned;

	if ( DeviceIoControl( m_hDriver, dwIoControlCode, lpInBuffer, dwInBufferSize, NULL, 0, &bytesReturned, NULL ) )
	{
		lnRet = DEDRIVERMANAGER_SUCCESS;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_IO;
	}

	return lnRet;
}

LONG CDeDriverManager::ReadIo( DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD dwOutBufferSize )
{
	if ( NULL == m_hDriver )
	{
		return DEDRIVERMANAGER_ERROR_INVALID_HANDLE;
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	DWORD bytesReturned;

	if ( DeviceIoControl( m_hDriver, dwIoControlCode, NULL, 0, lpOutBuffer, dwOutBufferSize, &bytesReturned, NULL ) )
	{
		lnRet = bytesReturned;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_IO;
	}

	return lnRet;
}

LONG CDeDriverManager::RawIo( DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize )
{
	if ( NULL == m_hDriver )
	{
		return DEDRIVERMANAGER_ERROR_INVALID_HANDLE;
	}

	LONG lnRet = DEDRIVERMANAGER_ERROR_UNKNOWN;
	DWORD bytesReturned;

	if ( DeviceIoControl( m_hDriver, dwIoControlCode, lpInBuffer, dwInBufferSize, lpOutBuffer, dwOutBufferSize, &bytesReturned, NULL ) )
	{
		lnRet = bytesReturned;
	}
	else
	{
		lnRet = DEDRIVERMANAGER_ERROR_IO;
	}

	return lnRet;
}