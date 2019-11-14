// ProcWinFirewall.cpp: implementation of the CProcWinFirewall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcWinFirewall.h"
#include "Winsvc.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcWinFirewall::CProcWinFirewall()
{
	//CoInitialize( NULL );

	//	...
	m_hrComInit	= E_FAIL;
	m_bComInitSucc	= FALSE;
	m_bFwInitSucc	= FALSE;

	m_pFwProfile	= NULL;


	//
	//	Initialize COM.
	//
	m_hrComInit = CoInitializeEx( 0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

	//
	//	Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	//	initialized with a different mode. Since we don't care what the mode is,
	//	we'll just use the existing mode.
	//
	if ( RPC_E_CHANGED_MODE != m_hrComInit )
	{
		if ( SUCCEEDED( m_hrComInit ) )
		{
			//
			//	COM init successfully
			//
			m_bComInitSucc = TRUE;

			//
			//	Initialize Windows Firewall interface
			//
			if ( SUCCEEDED( Initialize( &m_pFwProfile ) ) )
			{
				m_bFwInitSucc = TRUE;
			}
		}
		else
		{
			//printf( "CoInitializeEx failed: 0x%08lx\n", m_hrComInit );
		}
	}
}

CProcWinFirewall::~CProcWinFirewall()
{
	//CoUninitialize();

	//	Uninitialize COM.
	if ( SUCCEEDED( m_hrComInit ) )
	{
		if ( m_bFwInitSucc && m_pFwProfile )
		{
			//	Release the firewall profile.
			Cleanup();
		}
		CoUninitialize();
	}
}

HRESULT CProcWinFirewall::Initialize( OUT INetFwProfile ** ppfwProfile )
{
	HRESULT hr;
	INetFwMgr * fwMgr	= NULL;
	INetFwPolicy * fwPolicy	= NULL;

	_ASSERT( ppfwProfile != NULL );

	if ( NULL == ppfwProfile )
	{
		return E_FAIL;
	}
	if ( ! m_bComInitSucc )
	{
		return E_FAIL;
	}

	//	...
	*ppfwProfile	= NULL;

	//	Create an instance of the firewall settings manager.
	hr = CoCreateInstance( __uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), (VOID**)&fwMgr );
	if ( SUCCEEDED( hr ) && fwMgr )
	{
		//	Retrieve the local firewall policy.
		hr = fwMgr->get_LocalPolicy( &fwPolicy );
		if ( SUCCEEDED( hr ) && fwPolicy )
		{
			//	Retrieve the firewall profile currently in effect.
			hr = fwPolicy->get_CurrentProfile( ppfwProfile );
			if ( SUCCEEDED( hr ) )
			{
				//	...
			}
			else
			{
				//printf("get_CurrentProfile failed: 0x%08lx\n", hr);
				//goto error;
			}

			//	Release the local firewall policy.
			fwPolicy->Release();
			fwPolicy = NULL;
		}
		else
		{
			//printf("get_LocalPolicy failed: 0x%08lx\n", hr);
			//goto error;
		}

		//	Release the firewall settings manager.
		fwMgr->Release();
		fwMgr = NULL;
	}
	else
	{
		//printf("CoCreateInstance failed: 0x%08lx\n", hr);
		//goto error;
	}

	return hr;
}

BOOL CProcWinFirewall::IsFwInitSucc()
{
	//	是否初始化成功
	return m_bFwInitSucc;
}

VOID CProcWinFirewall::Cleanup()
{
	if ( ! IsFwInitSucc() )
	{
		return;
	}

	//	Release the firewall profile.
	if ( m_pFwProfile )
	{
		m_pFwProfile->Release();
		m_pFwProfile = NULL;
	}
}

BOOL CProcWinFirewall::IsFwSrvExist()
{
	//
	//	返回值		- TRUE/FALSE
	//

	BOOL bRet		= FALSE;
	SC_HANDLE scm;
	SC_HANDLE svc;


	//	..
	bRet = FALSE;

	scm = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if ( scm )
	{
		//	FW_SERVICE_XP_WIN2003
		svc = OpenService( scm, "SharedAccess", SERVICE_ALL_ACCESS );
		if ( svc )
		{
			bRet = TRUE;
			CloseServiceHandle( svc );
		}
		if ( ! bRet )
		{
			//	FW_SERVICE_VISTA
			svc = OpenService( scm, "MpsSvc", SERVICE_ALL_ACCESS );
			if ( svc )
			{
				bRet = TRUE;
				CloseServiceHandle( svc );
			}
		}
		
		CloseServiceHandle( scm );
	}

	return bRet;
}


HRESULT CProcWinFirewall::IsOn( OUT BOOL * pbFwOn )
{
	HRESULT hr;
	VARIANT_BOOL fwEnabled;

	_ASSERT( pbFwOn != NULL );

	if ( NULL == pbFwOn )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	...
	*pbFwOn = FALSE;

	//	Get the current state of the firewall.
	hr = m_pFwProfile->get_FirewallEnabled( &fwEnabled );
	if ( SUCCEEDED( hr ) )
	{
		//	Check to see if the firewall is on.
		if ( VARIANT_FALSE != fwEnabled )
		{
			*pbFwOn = TRUE;
			//printf("The firewall is on.\n");
		}
		else
		{
			//printf("The firewall is off.\n");
		}
	}
	else
	{
		//printf("get_FirewallEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}


HRESULT CProcWinFirewall::TurnOn()
{
	HRESULT hr;
	BOOL fwOn;

	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	Check to see if the firewall is off.
	hr = IsOn( &fwOn );
	if ( SUCCEEDED( hr ) )
	{
		//	If it is, turn it on.
		if ( ! fwOn )
		{
			//	Turn the firewall on.
			hr = m_pFwProfile->put_FirewallEnabled( VARIANT_TRUE );
			if ( SUCCEEDED( hr ) )
			{
				//printf("The firewall is now on.\n");
			}	
			else
			{
				//printf("put_FirewallEnabled failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already turn on
			hr = S_OK;
		}	
	}
	else
	{
		//printf("IsOn failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}


HRESULT CProcWinFirewall::TurnOff()
{
	HRESULT hr = S_OK;
	BOOL fwOn;

	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	Check to see if the firewall is on.
	hr = IsOn( &fwOn );
	if ( SUCCEEDED( hr ) )
	{
		//	If it is, turn it off.
		if ( fwOn )
		{
			//	Turn the firewall off.
			hr = m_pFwProfile->put_FirewallEnabled( VARIANT_FALSE );
			if ( SUCCEEDED( hr ) )
			{
				//printf("The firewall is now off.\n");
			}	
			else
			{
				//printf("put_FirewallEnabled failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already turn off
			hr = S_OK;
		}	
	}
	else
	{
		//printf("IsOn failed: 0x%08lx\n", hr);
		//goto error;
	}

	return hr;
}


HRESULT CProcWinFirewall::AppIsEnabled( IN CONST WCHAR * pwszFwProcessImageFileName, OUT BOOL * pbFwAppEnabled )
{
	HRESULT hr = S_OK;
	BSTR fwBstrProcessImageFileName = NULL;
	VARIANT_BOOL fwEnabled;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT( pwszFwProcessImageFileName != NULL );
	_ASSERT( pbFwAppEnabled != NULL );

	if ( NULL == pwszFwProcessImageFileName || NULL == pbFwAppEnabled )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	...
	*pbFwAppEnabled = FALSE;

	//	Retrieve the authorized application collection.
	hr = m_pFwProfile->get_AuthorizedApplications( &fwApps );
	if ( SUCCEEDED( hr ) )
	{
		//	Allocate a BSTR for the process image file name.
		fwBstrProcessImageFileName = SysAllocString( pwszFwProcessImageFileName );
		if ( SysStringLen( fwBstrProcessImageFileName ) > 0 )
		{
			//	Attempt to retrieve the authorized application.
			hr = fwApps->Item( fwBstrProcessImageFileName, &fwApp );
			if ( SUCCEEDED( hr ) )
			{
				//	Find out if the authorized application is enabled.
				hr = fwApp->get_Enabled( &fwEnabled );
				if ( SUCCEEDED( hr ) )
				{
					if ( VARIANT_FALSE != fwEnabled )
					{
						//	The authorized application is enabled.
						*pbFwAppEnabled = TRUE;
						//printf( "Authorized application %lS is enabled in the firewall.\n", pwszFwProcessImageFileName );
					}
					else
					{
						//printf( "Authorized application %lS is disabled in the firewall.\n", pwszFwProcessImageFileName );
					}
				}
				else
				{
					//printf("get_Enabled failed: 0x%08lx\n", hr);
					//goto error;
				}

				//	...
				fwApp->Release();
				fwApp = NULL;
			}
			else
			{
				//	The authorized application was not in the collection.
				hr = S_OK;
				//printf( "Authorized application %lS is disabled in the firewall.\n", pwszFwProcessImageFileName );
			}

	
			//	Free the BSTR.
			SysFreeString( fwBstrProcessImageFileName );
		}
		else
		{
			hr = E_OUTOFMEMORY;
			//printf("SysAllocString failed: 0x%08lx\n", hr);
			//goto error;
		}

		//	...
		fwApps->Release();
		fwApps = NULL;
	}
	else
	{
		//printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}


HRESULT CProcWinFirewall::AppAdd( IN CONST WCHAR * pwszFwProcessImageFileName, IN CONST WCHAR * pwszFwName )
{
	HRESULT hr = S_OK;
	BOOL bFwAppEnabled;
	BSTR fwBstrName = NULL;
	BSTR fwBstrProcessImageFileName = NULL;
	INetFwAuthorizedApplication * fwApp = NULL;
	INetFwAuthorizedApplications * fwApps = NULL;

	_ASSERT( pwszFwProcessImageFileName != NULL );
	_ASSERT( pwszFwName != NULL );

	if ( NULL == pwszFwProcessImageFileName || NULL == pwszFwName )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	First check to see if the application is already authorized.
	hr = AppIsEnabled( pwszFwProcessImageFileName, &bFwAppEnabled );
	if ( SUCCEEDED( hr ) )
	{
		//	Only add the application if it isn't already authorized.
		if ( ! bFwAppEnabled )
		{
			//	Retrieve the authorized application collection.
			hr = m_pFwProfile->get_AuthorizedApplications( &fwApps );
			if ( SUCCEEDED( hr ) )
			{
				//	Create an instance of an authorized application.
				hr = CoCreateInstance(
					__uuidof(NetFwAuthorizedApplication), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwAuthorizedApplication), (void**)&fwApp );
				if ( SUCCEEDED( hr ) )
				{
					//	Allocate a BSTR for the process image file name.
					fwBstrProcessImageFileName = SysAllocString( pwszFwProcessImageFileName );
					if ( SysStringLen( fwBstrProcessImageFileName ) > 0 )
					{
						//	Set the process image file name.
						hr = fwApp->put_ProcessImageFileName( fwBstrProcessImageFileName );
						if ( SUCCEEDED( hr ) )
						{
							//	Allocate a BSTR for the application friendly name.
							fwBstrName = SysAllocString( pwszFwName );
							if ( SysStringLen( fwBstrName ) > 0 )
							{
								// Set the application friendly name.
								hr = fwApp->put_Name( fwBstrName );
								if ( SUCCEEDED( hr ) )
								{
									// Add the application to the collection.
									hr = fwApps->Add( fwApp );
									if ( SUCCEEDED( hr ) )
									{
										//printf( "Authorized application %lS is now enabled in the firewall.\n", pwszFwProcessImageFileName );
									}
									else
									{
										//printf("Add failed: 0x%08lx\n", hr);
										//goto error;
									}
								}
								else
								{
									//printf("put_Name failed: 0x%08lx\n", hr);
									//goto error;
								}

								//	Free the BSTRs.
								SysFreeString( fwBstrName );
								fwBstrName = NULL;
							}
							else
							{
								hr = E_OUTOFMEMORY;
								//printf("SysAllocString failed: 0x%08lx\n", hr);
								//goto error;
							}
						}
						else
						{
							//printf("put_ProcessImageFileName failed: 0x%08lx\n", hr);
							//goto error;
						}

						//	Free the BSTRs.
						SysFreeString( fwBstrProcessImageFileName );
						fwBstrProcessImageFileName = NULL;
					}
					else
					{
						hr = E_OUTOFMEMORY;
						//printf("SysAllocString failed: 0x%08lx\n", hr);
						//goto error;
					}

					//	...
					fwApp->Release();
					fwApp = NULL;
				}
				else
				{
					//printf("CoCreateInstance failed: 0x%08lx\n", hr);
					//goto error;
				}

				//	...
				fwApps->Release();
				fwApps = NULL;
			}
			else
			{
				//printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already authorized
			hr = S_OK;
		}
	}
	else
	{
		//printf("AppIsEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}

HRESULT CProcWinFirewall::AppRemove( IN CONST WCHAR * pwszFwProcessImageFileName )
{
	HRESULT hr = S_OK;
	BOOL bFwAppEnabled;
	BSTR fwBstrName = NULL;
	BSTR fwBstrProcessImageFileName = NULL;
	INetFwAuthorizedApplication * fwApp = NULL;
	INetFwAuthorizedApplications * fwApps = NULL;

	_ASSERT( pwszFwProcessImageFileName != NULL );

	if ( NULL == pwszFwProcessImageFileName )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	First check to see if the application is already authorized.
	hr = AppIsEnabled( pwszFwProcessImageFileName, &bFwAppEnabled );
	if ( SUCCEEDED( hr ) )
	{
		//	Only add the application if it is already authorized.
		if ( bFwAppEnabled )
		{
			//	Retrieve the authorized application collection.
			hr = m_pFwProfile->get_AuthorizedApplications( &fwApps );
			if ( SUCCEEDED( hr ) )
			{
				//	Allocate a BSTR for the process image file name.
				fwBstrProcessImageFileName = SysAllocString( pwszFwProcessImageFileName );
				if ( SysStringLen( fwBstrProcessImageFileName ) > 0 )
				{
					hr = fwApps->Remove( fwBstrProcessImageFileName );
					if ( SUCCEEDED( hr ) )
					{
						//	...
					}	

					//	Free the BSTRs.
					SysFreeString( fwBstrProcessImageFileName );
					fwBstrProcessImageFileName = NULL;
				}
				else
				{
					hr = E_OUTOFMEMORY;
					//printf("SysAllocString failed: 0x%08lx\n", hr);
					//goto error;
				}

				//	...
				fwApps->Release();
				fwApps = NULL;
			}
			else
			{
				//printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already authorized
			hr = S_OK;
		}
	}
	else
	{
		//printf("AppIsEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}


HRESULT CProcWinFirewall::PortIsEnabled( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol, OUT BOOL * fwPortEnabled )
{
	HRESULT hr = S_OK;
	VARIANT_BOOL fwEnabled;
	INetFwOpenPort* fwOpenPort = NULL;
	INetFwOpenPorts* fwOpenPorts = NULL;

	_ASSERT( fwPortEnabled != NULL );
	
	if ( NULL == fwPortEnabled )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	..
	*fwPortEnabled = FALSE;

	//	Retrieve the globally open ports collection.
	hr = m_pFwProfile->get_GloballyOpenPorts( &fwOpenPorts );
	if ( SUCCEEDED( hr ) )
	{
		//	Attempt to retrieve the globally open port.
		hr = fwOpenPorts->Item( lnPortNumber, ipProtocol, &fwOpenPort );
		if ( SUCCEEDED( hr ) )
		{
			//	Find out if the globally open port is enabled.
			hr = fwOpenPort->get_Enabled( &fwEnabled );
			if ( SUCCEEDED( hr ) )
			{
				if ( VARIANT_FALSE != fwEnabled )
				{
					//	The globally open port is enabled.
					*fwPortEnabled = TRUE;
					//printf("Port %ld is open in the firewall.\n", lnPortNumber);
				}
				else
				{
					//printf("Port %ld is not open in the firewall.\n", lnPortNumber);
				}
			}	
			else
			{
				//printf("get_Enabled failed: 0x%08lx\n", hr);
				//goto error;
			}

			//	...
			fwOpenPort->Release();
			fwOpenPort = NULL;
		}
		else
		{
			// The globally open port was not in the collection.
			hr = S_OK;
			//printf("Port %ld is not open in the firewall.\n", lnPortNumber);
		}

		//	...
		fwOpenPorts->Release();
		fwOpenPorts = NULL;
	}
	else
	{
		//printf("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
		//goto error;
	}
	
	return hr;
}


HRESULT CProcWinFirewall::PortAdd( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol, IN CONST WCHAR * pwszName )
{
	HRESULT hr = S_OK;
	BOOL fwPortEnabled;
	BSTR fwBstrName = NULL;
	INetFwOpenPort* fwOpenPort = NULL;
	INetFwOpenPorts* fwOpenPorts = NULL;

	_ASSERT( pwszName != NULL );

	if ( NULL == pwszName )
	{
		return E_FAIL;
	}
	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	First check to see if the port is already added.
	hr = PortIsEnabled( lnPortNumber, ipProtocol, &fwPortEnabled );
	if ( SUCCEEDED( hr ) )
	{
		//	Only add the port if it isn't already added.
		if ( ! fwPortEnabled )
		{
			//	Retrieve the collection of globally open ports.
			hr = m_pFwProfile->get_GloballyOpenPorts( &fwOpenPorts );
			if ( SUCCEEDED( hr ) )
			{
				//	Create an instance of an open port.
				hr = CoCreateInstance( __uuidof(NetFwOpenPort), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwOpenPort), (VOID**)&fwOpenPort );
				if ( SUCCEEDED( hr ) )
				{
					//	Set the port number.
					hr = fwOpenPort->put_Port( lnPortNumber );
					if ( SUCCEEDED( hr ) )
					{
						//	Set the IP protocol.
						hr = fwOpenPort->put_Protocol( ipProtocol );
						if ( SUCCEEDED( hr ) )
						{
							//	Allocate a BSTR for the friendly name of the port.
							fwBstrName = SysAllocString( pwszName );
							if ( SysStringLen( fwBstrName ) > 0 )
							{
								// Set the friendly name of the port.
								hr = fwOpenPort->put_Name( fwBstrName );
								if ( SUCCEEDED( hr ) )
								{
									// Opens the port and adds it to the collection.
									hr = fwOpenPorts->Add( fwOpenPort );
									if ( SUCCEEDED( hr ) )
									{
										//printf("Port %ld is now open in the firewall.\n", lnPortNumber);
									}	
									else
									{
										//printf("Add failed: 0x%08lx\n", hr);
										//goto error;
									}
								}
								else
								{
									//printf("put_Name failed: 0x%08lx\n", hr);
									//goto error;
								}

								//	Free the BSTR.
								SysFreeString( fwBstrName );
								fwBstrName = NULL;
							}
							else
							{
								hr = E_OUTOFMEMORY;
								//printf("SysAllocString failed: 0x%08lx\n", hr);
								//goto error;
							}
						}
						else
						{
							//printf("put_Protocol failed: 0x%08lx\n", hr);
							//goto error;
						}
					}
					else
					{
						//printf("put_Port failed: 0x%08lx\n", hr);
						//goto error;
					}

					//	...
					fwOpenPort->Release();
					fwOpenPort = NULL;
				}
				else
				{
					//printf("CoCreateInstance failed: 0x%08lx\n", hr);
					//goto error;
				}

				//	...
				fwOpenPorts->Release();
				fwOpenPorts = NULL;
			}
			else
			{
				//printf("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already added. 
			hr = S_OK;
		}
	}
	else
	{
		//printf("PortIsEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}

	return hr;
}

HRESULT CProcWinFirewall::PortRemove( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol )
{
	HRESULT hr = S_OK;
	BOOL fwPortEnabled;
	INetFwOpenPorts* fwOpenPorts = NULL;

	if ( ! IsFwInitSucc() )
	{
		return E_FAIL;
	}

	//	First check to see if the port is already added.
	hr = PortIsEnabled( lnPortNumber, ipProtocol, &fwPortEnabled );
	if ( SUCCEEDED( hr ) )
	{
		//	Only add the port if it is already added.
		if ( fwPortEnabled )
		{
			//	Retrieve the collection of globally open ports.
			hr = m_pFwProfile->get_GloballyOpenPorts( &fwOpenPorts );
			if ( SUCCEEDED( hr ) )
			{
				hr = fwOpenPorts->Remove( lnPortNumber, ipProtocol );
				if ( SUCCEEDED( hr ) )
				{
				}
				else
				{
					//printf("CoCreateInstance failed: 0x%08lx\n", hr);
					//goto error;
				}

				//	...
				fwOpenPorts->Release();
				fwOpenPorts = NULL;
			}
			else
			{
				//printf("get_GloballyOpenPorts failed: 0x%08lx\n", hr);
				//goto error;
			}
		}
		else
		{
			//	already added. 
			hr = S_OK;
		}
	}
	else
	{
		//printf("PortIsEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}

	return hr;
}




VOID CProcWinFirewall::test()
{
	HRESULT hr = S_OK;
	INetFwProfile* fwProfile = NULL;

	
	// Turn off the firewall.
	hr = TurnOff();
	if (FAILED(hr))
	{
		printf("TurnOff failed: 0x%08lx\n", hr);
		goto error;
	}
	
	// Turn on the firewall.
	hr = TurnOn();
	if (FAILED(hr))
	{
		printf("TurnOn failed: 0x%08lx\n", hr);
		goto error;
	}
	
	// Add Windows Messenger to the authorized application collection.
	hr = AppAdd(
		L"%ProgramFiles%\\Messenger\\msmsgs.exe",
		L"Windows Messenger"
		);
	if (FAILED(hr))
	{
		printf("AppAdd failed: 0x%08lx\n", hr);
		goto error;
	}
	
	// Add TCP::80 to list of globally open ports.
	hr = PortAdd( 80, NET_FW_IP_PROTOCOL_TCP, L"WWW");
	if (FAILED(hr))
	{
		printf("PortAdd failed: 0x%08lx\n", hr);
		goto error;
	}
	
error:
	hr = S_OK;
}
