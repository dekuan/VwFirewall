// VwFuncServices.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNCSERVICES_HEADER__
#define __VWFUNCSERVICES_HEADER__

#include <vector>
using namespace std;

#include <winsvc.h>



//	停止/启动/重启 IIS 服务
BOOL _vwfunc_enum_services( vector<ENUM_SERVICE_STATUS> & vcAllServices );
BOOL _vwfunc_isexist_w3svc( BOOL bRunning );
BOOL _vwfunc_stop_w3svc();
BOOL _vwfunc_start_w3svc();
BOOL _vwfunc_restart_w3svc();





#endif // __VWFUNCSERVICES_HEADER__
