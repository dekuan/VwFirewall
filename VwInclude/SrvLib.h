#ifndef __SRVLIB_HEADER__
#define __SRVLIB_HEADER__


/**
 *	return value
 */
#define SRVLIB_ERROR_SUCC			0
#define SRVLIB_ERROR_UNKNOWN			-1000
#define SRVLIB_ERROR_PARAM			-1001
#define SRVLIB_ERROR_OPEN_SC_MANAGER		-1002
#define SRVLIB_ERROR_CREATE_SERVICE		-1003
#define SRVLIB_ERROR_DELETE_SERVICE		-1004
#define SRVLIB_ERROR_OPEN_SERVICE		-1005
#define SRVLIB_ERROR_START_SERVICE		-1006
#define SRVLIB_ERROR_STOP_SERVICE		-1007
#define SRVLIB_ERROR_QUERY_SERVICE_STATUS	-1008
#define SRVLIB_ERROR_SERVICE_ALREADY_STOPED	-1009



/**
 *	NameSpaceSrvLib
 */
namespace DE
{
	class SrvLib
	{
	public:

		static LONG srvlib_install_service( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvExeFilename, IN LPCTSTR lpcszSrvDesc, IN BOOL bInteractive = TRUE );
		static LONG srvlib_uninstall_service( IN LPCTSTR lpcszSrvName );

		static LONG srvlib_start_service( IN LPCTSTR lpcszSrvName );
		static LONG srvlib_stop_service( IN LPCTSTR lpcszSrvName );
		static LONG srvlib_get_service_status( IN LPCTSTR lpcszSrvName );
		static LONG srvlib_get_service_filepath( IN LPCTSTR lpcszSrvName, OUT LPTSTR lpszSrvExeFilename, IN DWORD dwSize );
		static BOOL srvlib_is_service_exist( IN LPCTSTR lpcszSrvName );

		static LONG srvlib_set_service_description( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvDesc );

	};
}


#endif	// __SRVLIB_HEADER__