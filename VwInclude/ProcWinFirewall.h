// ProcWinFirewall.h: interface for the CProcWinFirewall class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCWINFIREWALL_HEADER__
#define __PROCWINFIREWALL_HEADER__


#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "netfw.h"
#include "objbase.h"
#include "oleauto.h"

#pragma comment( lib, "ole32.lib" )




/**
 *	class of CProcWinFirewall
 */
class CProcWinFirewall
{
public:
	CProcWinFirewall();
	virtual ~CProcWinFirewall();

public:
	BOOL	IsFwInitSucc();
	BOOL    IsFwSrvExist();

	HRESULT Initialize( OUT INetFwProfile ** ppfwProfile );
	VOID    Cleanup();

	HRESULT IsOn( OUT BOOL * pbFwOn );
	HRESULT TurnOn();
	HRESULT TurnOff();

	HRESULT AppIsEnabled( IN CONST WCHAR * pwszFwProcessImageFileName, OUT BOOL * pbFwAppEnabled );
	HRESULT AppAdd( IN CONST WCHAR * pwszFwProcessImageFileName, IN CONST WCHAR * pwszFwName );
	HRESULT AppRemove( IN CONST WCHAR * pwszFwProcessImageFileName );

	HRESULT PortIsEnabled( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol, OUT BOOL * fwPortEnabled );
	HRESULT PortAdd( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol, IN CONST WCHAR * pwszName );
	HRESULT PortRemove( IN LONG lnPortNumber, IN NET_FW_IP_PROTOCOL ipProtocol );

	VOID    test();

private:
	HRESULT m_hrComInit;
	BOOL	m_bComInitSucc;
	BOOL	m_bFwInitSucc;
	INetFwProfile * m_pFwProfile;
};




#endif // __PROCWINFIREWALL_HEADER__



