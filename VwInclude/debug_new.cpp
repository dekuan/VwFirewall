/************************************************************************/
/* comment:  此文件与debug_new.h配合使用,用于在调试期发现内存泄漏       */
/*           仅在VC++编译器中适用(包括Intel C++,因为它使用了相同的库)   */
/* 作者:     周星星 http://blog.vckbase.com/bruceteen/                  */
/* 版权申明: 无,可任意 使用,修改 和 发布                                */
/************************************************************************/

#include "StdAfx.h"
//#include "debug_new.h"

#ifdef _DEBUG

#include <iostream>
#include <windows.h>
#include <crtdbg.h>

using namespace std;


class _CriSec
{
    CRITICAL_SECTION criSection;
public:
    _CriSec()    { InitializeCriticalSection( &criSection ); }
    ~_CriSec()   { DeleteCriticalSection( &criSection );     }
    void Enter() { EnterCriticalSection( &criSection );      }
    void Leave() { LeaveCriticalSection( &criSection );      }
} _cs;

void _RegDebugNew( void )
{
    _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF );
}
void * __cdecl operator new( size_t nSize, const char* lpszFileName, int nLine )
{
    // comment 1: MFC中提供的debug new虽然加了锁,但我在实际测试的时候发现多线程并发
    //            调用的时候还是抛出了系统错误,所以我在这里加了一个线程互斥量.
    // comment 2: debug new和debug delete之间需不需要互斥我并不知道,保险起见,我同样
    //            加了线程互斥量.
    // comment 3: 按照C++标准规定,在operator new失败后应当调用set_new_handler设置的
    //            函数,但是MSDN中却说"头文件new中的set_new_handler是stub的,而应该使
    //            用头文件new.h中的_set_new_handler",这简直是滑天下之大稽.
    //            以下是VC++6.0中的set_new_handler定义:
    //                new_handler __cdecl set_new_handler( new_handler new_p )
    //                {
    //                    assert( new_p == 0 ); // cannot use stub to register a new handler
    //                    _set_new_handler( 0 );
    //                    return 0;
    //                }
    //            所以我也无计可施,只能舍弃set_new_handler的作用.

    _cs.Enter();
    void* p = _malloc_dbg( nSize, _NORMAL_BLOCK, lpszFileName, nLine );
    _cs.Leave();
    return p;
}
void __cdecl operator delete( void* p, const char* /*lpszFileName*/, int /*nLine*/ )
{
    _cs.Enter();
    _free_dbg( p, _CLIENT_BLOCK );
    _cs.Leave();
}

#endif
