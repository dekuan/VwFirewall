/************************************************************************/
/* comment:  此文件与debug_new.cpp配合使用,用于在调试期发现内存泄漏     */
/*           仅在VC++编译器中适用(包括Intel C++,因为它使用了相同的库)   */
/* 作者:     周星星 http://blog.vckbase.com/bruceteen/                  */
/* 版权申明: 无,可任意 使用,修改 和 发布                                */
/************************************************************************/

/* sample

#include <iostream>
#include "debug_new.h" // +
using namespace std;

int main( void )
{
    REG_DEBUG_NEW; // +

    char* p = new char[2];

    cout << "--End--" << endl;
    return 0;
}

在VC++ IDE中按F5调试运行将会在Output窗口的Debug页看到类似如下的提示:
Dumping objects ->
d:\test.cpp(10) : {45} normal block at 0x003410C8, 2 bytes long.
Data: <  > CD CD 
Object dump complete.

如果不出现如上提示请Rebuild All一次.

*/

#ifndef _DEBUG_NEW_H_
#define _DEBUG_NEW_H_

#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h> 
#include<crtdbg.h>

#ifdef _DEBUG

	#undef new
	extern void _RegDebugNew( void );
	extern void* __cdecl operator new( size_t, const char*, int );
	extern void __cdecl operator delete( void*, const char*, int);
	#define new new(__FILE__, __LINE__)

	#define REG_DEBUG_NEW _RegDebugNew();

#else

	#define REG_DEBUG_NEW

#endif // _DEBUG

#endif // _DEBUG_NEW_H_
