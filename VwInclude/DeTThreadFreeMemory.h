// DeTThreadFreeMemory.h: interface for the CTProcCacheBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DETTHREADFREEMEMORY_HEADER__
#define __DETTHREADFREEMEMORY_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////
//	CDeTThreadFreeMemory

/**
 *
 */
template< class _T >
	class CDeTThreadFreeMemory
{
public:
	CDeTThreadFreeMemory()
	{
	}
	~CDeTThreadFreeMemory()
	{
	}

	VOID FreeArrayMemory( _T * pstArray )
	{
		if ( NULL == pstArray )
		{
			return;
		}

		_beginthread
		(
			_thread_DeTThreadFreeMemory_FreeArrayMemory,
			0,
			(VOID*)pstArray
		);
	}
	static VOID _thread_DeTThreadFreeMemory_FreeArrayMemory( PVOID arglist )
	{
		if ( NULL == arglist )
		{
			return;
		}

		__try
		{
			_T * pstArray = (_T*)(arglist);
			if ( pstArray )
			{
				delete [] pstArray;
				pstArray = NULL;
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}
};




#endif	// __DETTHREADFREEMEMORY_HEADER__