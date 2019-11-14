//////////////////////////////////////////////////////////////////////
//
// Written by Michael Dunn (mdunn at inreach dot com).  Copyright and all
// that stuff.  Use however you like but give me credit where it's due.  I'll
// know if you don't. bwa ha ha ha ha!
// 
// Release history:
//   December 24, 1999: Version 1.0.  First release.
//
//////////////////////////////////////////////////////////////////////

// BindStatusCallback.h: interface for the CBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BINDSTATUSCALLBACK_HEADER__
#define __BINDSTATUSCALLBACK_HEADER__


//	disable warnings about unreferenced params
#pragma warning(disable:4100)


#include "Urlmon.h"
#pragma comment( lib, "Urlmon.lib" )


#ifndef UM_HTTP_PREGRESS
	#define UM_HTTP_PREGRESS	WM_USER + 0x6000
#endif

#ifndef DOWNLOAD_TIME_FORCEFAIL
	//	进度无变化，超时处理，强制超时时间
	#define DOWNLOAD_TIME_FORCEFAIL	60*1000
#endif


/**
 *
 */
class CCallback : public IBindStatusCallback  
{
public:
	CCallback();


	BOOL* m_pbAbort;
	HWND  m_hWnd;			// 进度窗口
	
	DWORD m_dwStart;		// 开始时间
	DWORD m_dwProgressChange;
	ULONG m_LastProgress;		// 上次进入 OnProgress 的时候的进度
	DWORD m_dwTimeout;		// 超时时间间隔

	// IBindStatusCallback methods.  Note that the only method called by IE
	// is OnProgress(), so the others just return E_NOTIMPL.
	
	STDMETHOD(OnStartBinding)(
		/* [in] */ DWORD dwReserved,
		/* [in] */ IBinding __RPC_FAR *pib)
        { 
		return E_NOTIMPL;
	}
	
	STDMETHOD(GetPriority)(
		/* [out] */ LONG __RPC_FAR *pnPriority)
        { 
		return E_NOTIMPL;
	}
	
	STDMETHOD(OnLowResource)(
		/* [in] */ DWORD reserved)
        { 
		return E_NOTIMPL;
	}
	
	STDMETHOD(OnProgress)(
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR wszStatusText);
		
	STDMETHOD(OnStopBinding)(
		/* [in] */ HRESULT hresult,
		/* [unique][in] */ LPCWSTR szError)
        { 
		return E_NOTIMPL;
	}
	
	STDMETHOD(GetBindInfo)(
		/* [out] */ DWORD __RPC_FAR *grfBINDF,
		/* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo);
		
	STDMETHOD(OnDataAvailable)(
		/* [in] */ DWORD grfBSCF,
		/* [in] */ DWORD dwSize,
		/* [in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [in] */ STGMEDIUM __RPC_FAR *pstgmed)
        { 
		return E_NOTIMPL;
	}
	
	STDMETHOD(OnObjectAvailable)(
		/* [in] */ REFIID riid,
		/* [iid_is][in] */ IUnknown __RPC_FAR *punk)
        { 
		return E_NOTIMPL;
	}
	
	// IUnknown methods.  Note that IE never calls any of these methods, since
	// the caller owns the IBindStatusCallback interface, so the methods all
	// return zero/E_NOTIMPL.
	
	STDMETHOD_(ULONG,AddRef)()
        { return 0; }
	
	STDMETHOD_(ULONG,Release)()
        { return 0; }
	
	STDMETHOD(QueryInterface)(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
        { return E_NOTIMPL; }
};


#pragma warning(default:4100)



#endif // __BINDSTATUSCALLBACK_HEADER__
