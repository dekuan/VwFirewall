/************************************************************
 *
 *	wdaTrie.h
 *
 *	AUTHOR  : DongYi dolo@vip.sina.com
 *		: 刘其星 liuqixing@gmail.com
 *
 *	FUNCTION: FAST and COMPACT TRIE for 1B1 letter match.
 *		: 双向字典 WORD->ID ID->WORD
 *
 *	NOTE    : 参考了DA.c,修正了其中的BUGS
 *
 *	HISTORY	:
 *		2005-08-16 DongYi dolo@vip.sina.com 发布第一版本
 *		2007-06-29 刘其星 liuqixing@gmail.com 完成 Win32 版本的改造
 *
 ************************************************************/

#include "env.h"
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
//#include <unistd.h>
#include <sys/types.h>
//#include <sys/mman.h>
#include <vector>
#include <map>
#include <string>
#ifndef __WCHAR_DA_TRIE_H_
#define __WCHAR_DA_TRIE_H_

//	编译后的词典文件头
//#define WDATRIE_MAGIC		"DWT.XP.DTRIE"
#define WDATRIE_MAGIC		"DE.XWT.DTRIE"
#define WDATRIE_MAGIC_LEN	12

const int  __g_wchar_size = ( 1 << 16 );

template <class _Tp>
class wdaTrie 
{
public:
	struct wordProp
	{
		_Tp	sProp;
		int offset;
	};

	struct daElm
	{
		int nBase;
		int nCheck;
	};

	struct resolveElm
	{
		int c;
		int nBase;
	};
	
	struct resolveStat
	{
		int		_M_n_elm;
		resolveElm	_M_p_elm[__g_wchar_size];
	};

	struct daTrieHead
	{
		char		_M_magic[ WDATRIE_MAGIC_LEN ];
		bool		_M_case;
		int		_M_size;
		int		_M_n_elm;
		daElm*		_M_p_elm;
		int		_M_n_prop;
		wordProp*	_M_p_prop;
		int		_M_n_text;
		uint16_t	* _M_p_text;
		int		_M_n_offset;
	};

public:

	wdaTrie( int bCase = 1, int nSpeed = 0 )
	{
		_M_n_speed = abs(nSpeed) % 129;
	   	_M_n_left = 2;
		_M_n_map = 0;
		_M_p_map = NULL;
		__Initialize( bCase ? true : false );
	}
	
	~wdaTrie()
	{
		__clean( ); 
	}

private:
	int		_M_n_speed;
	int		_M_n_left;	
	daTrieHead	_M_daTrie;
	int		_M_n_map;
	char *		_M_p_map;

private:
	int  __wstrlen( uint16_t *s )
	{
		if( s == NULL ) return 0;
		uint16_t *p = s; while( *p ) p++;
		return (int)( p - s );
	}

	void __resizeElm( int nIdx )
	{
		if ( nIdx >= _M_daTrie._M_n_elm )
		{
			size_t __nsize = ( (nIdx + 511) & ~511 ) * 3 / 2;
			daElm *p = (daElm *)realloc( _M_daTrie._M_p_elm, __nsize * sizeof(daElm) );
			if ( p == NULL )
			{
				__nsize = ( nIdx + 1023) & ~511;
				p = (daElm *)realloc( _M_daTrie._M_p_elm, __nsize * sizeof(daElm) );
				assert ( p != NULL );
			}
			_M_daTrie._M_p_elm = p;
			for( size_t i = _M_daTrie._M_n_elm ; i < __nsize; i++ )
			{
				_M_daTrie._M_p_elm[i].nBase = 0;
				_M_daTrie._M_p_elm[i].nCheck= 0;
			}
			_M_daTrie._M_n_elm = __nsize;
		}
	}

	void __resizeProp( int nIdx )
	{
		if ( nIdx >= _M_daTrie._M_n_prop )
		{
			size_t __nsize = ( nIdx + 127 ) & ~63;
			_M_daTrie._M_p_prop = (wordProp *)realloc( _M_daTrie._M_p_prop, __nsize * sizeof(wordProp) );
			assert( _M_daTrie._M_p_prop != NULL );
			memset( _M_daTrie._M_p_prop + _M_daTrie._M_n_prop, 0, sizeof( wordProp ) * ( __nsize - _M_daTrie._M_n_prop) );
			_M_daTrie._M_n_prop = __nsize;
		}
	}

	void __resizeText( int nInc )
	{
		if( nInc + _M_daTrie._M_n_offset >= _M_daTrie._M_n_text )
		{
			size_t __nsize = ( ( _M_daTrie._M_n_offset + nInc + 1023 ) & ~1023) * 3 / 2;
			uint16_t * p = (uint16_t *)realloc( _M_daTrie._M_p_text, __nsize * sizeof(uint16_t) );
			if( p == NULL )
			{
				__nsize = ( _M_daTrie._M_n_offset + nInc + 1023 ) & ~1023;
				p = (uint16_t *)realloc( _M_daTrie._M_p_text, __nsize * sizeof(uint16_t) );
				assert( p != NULL );
			}
			_M_daTrie._M_p_text = p;
			_M_daTrie._M_n_text = __nsize;
		}
	}

	int __getBase( int nIdx )
	{
		return ( nIdx >= _M_daTrie._M_n_elm ) ? 0 : _M_daTrie._M_p_elm[nIdx].nBase;
	}

	void __setBase( int nIdx, int v )
	{
		__resizeElm( nIdx );
		_M_daTrie._M_p_elm[nIdx].nBase = v;
	}

	int __getCheck( int nIdx )
	{
		return ( nIdx >= _M_daTrie._M_n_elm ) ? 0 : _M_daTrie._M_p_elm[nIdx].nCheck;
	}

	void __setCheck( int nIdx, int v )
	{
		__resizeElm( nIdx );
		_M_daTrie._M_p_elm[nIdx].nCheck = v;
	}

	_Tp *__getProp( int nIdx )
	{
		int n = __getBase( nIdx );
		return ( n >= _M_daTrie._M_n_prop ) ? NULL : &_M_daTrie._M_p_prop[n].sProp;
	}

	void __setProp( int nIdx, const _Tp &iProp )
	{
		int n = __getBase( nIdx );
		__resizeProp( n ); 
		_M_daTrie._M_p_prop[n].sProp = iProp;
	}

	void __setText( int nIdx, uint16_t *__p, int __len )
	{
		int n = __getBase( nIdx ); 
		__resizeProp( n ); __resizeText ( __len + 1 );
		_M_daTrie._M_p_prop[n].offset = _M_daTrie._M_n_offset;
		memcpy( _M_daTrie._M_p_text + _M_daTrie._M_n_offset, __p, __len * sizeof(uint16_t) );
		_M_daTrie._M_p_text[_M_daTrie._M_n_offset + __len] = 0;
		_M_daTrie._M_n_offset += __len + 1;
	}

	void __pushChild( resolveStat *pStat, int c, int orig_base)
	{
		int n = pStat->_M_n_elm;
		pStat->_M_n_elm++;
		pStat->_M_p_elm[n].c = c;
		pStat->_M_p_elm[n].nBase = orig_base;
	}

	int __collectChild( resolveStat *pStat, int parent, int c )
	{
		int i,idx,nLeft = _M_n_left;
		int base = __getBase( parent );

		pStat->_M_n_elm = 0;
		for ( i = 0; i < __g_wchar_size; i++ )
		{
			idx = parent + base + i;
			if ( i == c )
			{
				__pushChild( pStat, i, ( c == 0 ) ? (_M_daTrie._M_size + 1) : 1 );
			}
			else if ( __getCheck( idx ) == parent )
			{
				__pushChild( pStat, i, __getBase(idx) );
			}
		}
	
		i = 0;
		while( i < pStat->_M_n_elm )
		{
			if ( __getCheck( nLeft + pStat->_M_p_elm[i].c ) != 0 )
			{
				nLeft++; i = 0;
				continue;
			}
			i++;
		}

		int nChr = __g_wchar_size / 2 - _M_n_speed * 256;
		if ( pStat->_M_n_elm >= nChr )
		{
			_M_n_left = nLeft;
		}

		for ( i = 0; i < pStat->_M_n_elm; i++ )
		{
			if ( pStat->_M_p_elm[i].c != c )
			{
				idx = parent + base + pStat->_M_p_elm[i].c;
				__setBase ( idx, 0 ); __setCheck( idx, 0 );
			}
		}

		return nLeft;
	}

	void __moveChildren( resolveStat *pStat, int parent, int left )
	{
		int i,move;
		int orig_base = __getBase( parent );

		//move = old_left - new_left = ( parent + base(parent) ) - left;
		move = parent + orig_base - left;
		//new_base = orig_base - move = orig_base - ( parent + orig_base - left )
		__setBase( parent, left - parent );

		for ( i = 0; i < pStat->_M_n_elm; i++ )
		{
			int idx = left + pStat->_M_p_elm[i].c;
			__setCheck( idx, parent );
			if( pStat->_M_p_elm[i].c == 0 )
				__setBase ( idx, pStat->_M_p_elm[i].nBase );
			else
				__setBase ( idx, pStat->_M_p_elm[i].nBase + move);
		}

		for ( i = 0; i < pStat->_M_n_elm; i++ )
		{
			int k;
			int old_left, old_child;

			//old_child = t = s + base(s) + c
			old_child = parent + orig_base + pStat->_M_p_elm[i].c; 
			//s = t = old_child ; left = old_child + base(old_child) = old_child + pStat->_M_p_elm[i].nBase
			old_left = old_child + pStat->_M_p_elm[i].nBase;
		
			for ( k = 0; k < __g_wchar_size; k++ )
			{
				int orig_check = __getCheck( old_left + k);
				if ( orig_check == old_child && __getCheck( orig_check ) == 0 )
				{
					__setCheck( old_left + k, orig_check - move );
				}
			}
		}
	}

	int __resolveConflict( int parent, uint16_t c )
	{
		int nLeft;
		resolveStat curChild;

		nLeft = __collectChild( &curChild,  parent, c);
		__moveChildren( &curChild, parent, nLeft);
		return nLeft + c;
	}

	int __addChr( int parent, uint16_t c )
	{
		int next = parent + __getBase(parent) + c;
		if( __getCheck( next ) > 0 ) {
			return __resolveConflict( parent , c); 
		}
		__setCheck( next, parent );
		if( c == 0 ) 
			__setBase ( next, _M_daTrie._M_size + 1 );
		else
			__setBase ( next, (parent == 1) ? (__g_wchar_size - next) : 1 );
		return next;
	}

	void __Initialize( bool bCase )
	{
		memset( &_M_daTrie, 0, sizeof( daTrieHead ) );
		memcpy( _M_daTrie._M_magic, WDATRIE_MAGIC, WDATRIE_MAGIC_LEN );
		_M_daTrie._M_case = bCase;
		__setBase ( 1, 1 ); __setCheck( 1, -1);
	}

	void __clean()
	{
		if ( _M_p_map )
		{
			//munmap( _M_p_map, _M_n_map );
			_M_p_map = NULL;
			_M_n_map = 0;
		}
		else
		{
			if ( _M_daTrie._M_p_elm ) { free( _M_daTrie._M_p_elm ); }
			if ( _M_daTrie._M_p_prop) { free( _M_daTrie._M_p_prop); }
			if ( _M_daTrie._M_p_text) { free( _M_daTrie._M_p_text); }
		}
		memset( &_M_daTrie, 0, sizeof( daTrieHead ) );
	}

private:

	int __dynac_load( const char * sIndexName )
	{
		//int i;
		HANDLE hFile		= NULL;
		DWORD dwDummy		= 0;		
		daTrieHead __daHead;

		hFile = CreateFile( sIndexName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFile )
		{
			//	HEADER
			dwDummy = 0;
			ReadFile( hFile, &__daHead, sizeof( daTrieHead ), &dwDummy, NULL );
			//i = fread( &__daHead, sizeof(char), sizeof( daTrieHead ), fp );
			if ( dwDummy != sizeof( daTrieHead ) )
			{
				CloseHandle( hFile );
				return 0;
			}
			if ( memcmp( __daHead._M_magic, WDATRIE_MAGIC, WDATRIE_MAGIC_LEN ) )
			{
				CloseHandle( hFile );
				return 0;
			}
			__clean();
			__daHead._M_p_elm = (daElm *)malloc( sizeof( daElm ) * __daHead._M_n_elm );
			assert( __daHead._M_p_elm != NULL );
			__daHead._M_p_prop = (wordProp *)malloc( sizeof( wordProp ) * __daHead._M_n_prop );
			assert( __daHead._M_p_prop != NULL );
			__daHead._M_p_text = (uint16_t *)malloc( sizeof( uint16_t ) * __daHead._M_n_text );
			assert( __daHead._M_p_text != NULL );
			
			//	daElm
			dwDummy = 0;
			ReadFile( hFile, __daHead._M_p_elm, sizeof( daElm ) * __daHead._M_n_elm, &dwDummy, NULL );
			//i = fread( __daHead._M_p_elm, sizeof(char), sizeof( daElm ) * __daHead._M_n_elm, fp );
			if ( dwDummy != (int)( sizeof( daElm ) * __daHead._M_n_elm ) )
			{
				CloseHandle( hFile );
				return 0;
			}

			//	wordProp
			dwDummy = 0;
			ReadFile( hFile, __daHead._M_p_prop, sizeof( wordProp ) * __daHead._M_n_prop, &dwDummy, NULL );
			//i = fread( __daHead._M_p_prop, sizeof(char), sizeof( wordProp ) * __daHead._M_n_prop, fp );
			if ( dwDummy != (int)(sizeof( wordProp ) * __daHead._M_n_prop ) )
			{
				CloseHandle( hFile );
				return 0;
			}
			
			//	text
			ReadFile( hFile, __daHead._M_p_text, sizeof( uint16_t ) * __daHead._M_n_text, &dwDummy, NULL );
			//i = fread( __daHead._M_p_text, sizeof(char), sizeof( uint16_t ) * __daHead._M_n_text, fp );
			if ( dwDummy != (int)(sizeof( uint16_t ) * __daHead._M_n_text ) )
			{
				CloseHandle( hFile );
				return 0;
			}

			memcpy( &_M_daTrie, &__daHead, sizeof( daTrieHead ) );

			CloseHandle( hFile );
			return 1;
		}
		else
		{
			return 0;
		}
	}
/*
	int __static_load( char * sIndexName )
	{
		int i,nFD;
		daTrieHead __daHead;

		nFD = open( sIndexName, O_RDONLY, 0666);
		if( nFD < 0 ) return 0;

		//HEADER
		i = read( nFD, &__daHead, sizeof( daTrieHead ) );
		if( i != sizeof( daTrieHead ) )
		{
			close(nFD);
			return 0;
		}
		if( memcmp( __daHead._M_magic, WDATRIE_MAGIC, WDATRIE_MAGIC_LEN ) )
		{
			close( nFD ); return 0;
		}

		__clean( );

		_M_n_map = lseek( nFD, 0L, SEEK_END );
		lseek ( nFD, 0L, SEEK_SET);

		_M_p_map = (char *)::mmap( NULL, _M_n_map, PROT_READ, MAP_FILE|MAP_SHARED, nFD, 0);
		if ( _M_p_map == MAP_FAILED )
		{
			close( nFD );
			return 0;
		}
		int iStep = 64 * 1024;
		char c[iStep];
		char *p = (char *)_M_p_map;
		for ( int i = 0; i < ( _M_n_map / iStep ); i++ )
		{
			memcpy( c, p + i * iStep, iStep );
		}
		__daHead._M_p_elm  = (daElm *)    ( _M_p_map + sizeof( daTrieHead ) );
		__daHead._M_p_prop = (wordProp *) ( _M_p_map + sizeof( daTrieHead ) + sizeof( daElm ) * __daHead._M_n_elm );
		__daHead._M_p_text = (uint16_t *) ( _M_p_map + sizeof( daTrieHead ) + sizeof( daElm ) * __daHead._M_n_elm + sizeof( wordProp ) * __daHead._M_n_prop );

		memcpy( &_M_daTrie, &__daHead, sizeof( daTrieHead ) );
		close( nFD );
		return 1;
	}
*/
public:

	void clean( ) { __clean(); }

	void clear( )
	{
		_M_daTrie._M_size = 0;
		_M_daTrie._M_n_offset = 0;
		memset( _M_daTrie._M_p_elm , 0, sizeof( daElm )  * _M_daTrie._M_n_elm );
		memset( _M_daTrie._M_p_prop, 0, sizeof( wordProp) * _M_daTrie._M_n_prop );
		__setBase ( 1, 1 ); __setCheck( 1, -1);
	}

	int size( ) { return _M_daTrie._M_size; }	

	bool prefixSrch( uint16_t *sText, _Tp **ppProp = NULL, int *pId = NULL )
	{
		int left, next, parent = 1;
		uint16_t *p;

		left = parent + __getBase( parent );	
		for ( p = sText; *p; p++ ) {
			if( _M_daTrie._M_case )
				next = left + toupper( *p );
			else
				next = left + *p;
			if ( __getCheck( next ) != parent ) return false;
			parent = next;
			left = parent + __getBase( parent );
		}

		if( __getCheck( left ) != parent ) return false;

		if( pId   != NULL ) *pId = __getBase( left );
		if( ppProp!= NULL ) *ppProp = &_M_daTrie._M_p_prop[__getBase( left )].sProp;
		return true;
	}

	int prefixMatch( uint16_t *sText, _Tp ** ppProp = NULL, int *pId = NULL)
	{
		int left,next,parent = 1;
		int acceptPOINT = 0;
		uint16_t *p, *pOK;

		left = parent + __getBase( parent );	
		for ( pOK = p = sText; *p; p++ )
		{
			if ( _M_daTrie._M_case )
			{
				next = left + toupper( *p );
			}
			else
			{
				next = left + *p;
			}

			if ( __getCheck( next ) != parent )
			{
				if ( acceptPOINT )
				{
					if( pId    != NULL ) *pId = __getBase( acceptPOINT ) ;
					if( ppProp != NULL ) *ppProp = &_M_daTrie._M_p_prop[__getBase( acceptPOINT )].sProp;
					return (int)( pOK - sText );
				}
				return 0;
			}
			parent = next;
			left = parent + __getBase( parent );
			if ( __getCheck( left ) == parent )
			{
				pOK = p + 1;
				acceptPOINT = left;
			}
		}

		if ( acceptPOINT )
		{
			if ( pId    != NULL )
			{
				*pId = __getBase( acceptPOINT );
			}
			if ( ppProp != NULL )
			{
				*ppProp = &_M_daTrie._M_p_prop[__getBase( acceptPOINT )].sProp;
			}
			return (int)( pOK - sText );
		}
		return 0;
	}
	
	//
	//	前匹配
	//
	int prefixMatch( uint16_t *sText, int nLen, _Tp ** ppProp = NULL, int *pId = NULL )
	{
		INT left, next, parent = 1;
		INT acceptPOINT = 0;
		uint16_t * p, * q, * pOK;

		left	= parent + __getBase( parent );
		for ( pOK = p = sText, q = sText + nLen; p < q; p++ )
		{
			if ( _M_daTrie._M_case )
			{
				next = left + toupper( *p );
			}
			else
			{
				next = left + *p;
			}
			if ( __getCheck( next ) != parent )
			{
				if ( acceptPOINT )
				{
					if ( pId != NULL )
					{
						*pId = __getBase( acceptPOINT );
					}
					if ( ppProp != NULL )
					{
						*ppProp = &_M_daTrie._M_p_prop[ __getBase( acceptPOINT ) ].sProp;
					}
					return (int)( pOK - sText );
				}
				return 0;
			}
			parent	= next;
			left	= parent + __getBase( parent );
			if ( __getCheck( left ) == parent )
			{
				pOK = p + 1; acceptPOINT = left;
			}
		}

		if ( acceptPOINT )
		{
			if ( pId != NULL )
			{
				*pId = __getBase( acceptPOINT );
			}
			if ( ppProp != NULL )
			{
				*ppProp = &_M_daTrie._M_p_prop[ __getBase( acceptPOINT ) ].sProp;
			}
			return (int)( pOK - sText );
		}
		return 0;
	}

	//RETURN VAL: 
	// -1:	ERROR
	//0-N:	WORDID
	int insert( uint16_t *sWord, const _Tp &iProp = 0 )
	{
		int i, next, parent = 1;
		bool bInsert = false;
		uint16_t c;

		int nLen = __wstrlen( sWord );
		if ( nLen == 0 )
		{
			return -1;
		}

		for ( i = 0; i <= nLen; i++)
		{
			if ( _M_daTrie._M_case )
			{
				c = toupper( sWord[i] );
			}
			else
			{
				c = sWord[i];
			}

			next = parent + __getBase( parent ) + c;

			if ( __getCheck( next ) != parent )
			{
				bInsert = true;
				next = __addChr( parent, c );
			}
			parent = next;
		}

		if ( bInsert )
		{
			_M_daTrie._M_size ++;
			__setText( parent, sWord, nLen );
		}
		__setProp( parent, iProp );

		return __getBase( parent );
	}

	bool erase( uint16_t *sWord )
	{
		int i,next,parent = 1;

		int nLen = __wstrlen( sWord );
		if( nLen == 0 ) return false;

		for ( i = 0; i <= nLen; i++) {
			if( _M_daTrie._M_case )
				next = parent + __getBase( parent ) + toupper( sWord[i] );
			else
				next = parent + __getBase( parent ) + sWord[i];
			if ( __getCheck( next ) != parent ) return false;
			parent = next;
		}
		__setBase( parent, 0 ); __setCheck( parent, 0 );
		return true;
	}

	bool setProp( uint16_t *sWord, const _Tp &iProp )
	{
		_Tp *pProp;
		bool bSuc = prefixSrch( sWord, &pProp );
		if( bSuc ) pProp->sProp = iProp;
		return bSuc;
	}

	bool setProp( int nId, const _Tp &iProp)
	{
		if( nId < 0 || nId >= _M_daTrie._M_size ) return false;
		_M_daTrie._M_p_prop[nId].sProp = iProp;
		return true;
	}

	bool getProp( uint16_t *sWord, _Tp &iProp )
	{
		_Tp *pProp;
		bool bSuc = prefixSrch( sWord, &pProp );
		if( bSuc ) iProp = pProp->sProp;
		return bSuc;
	}

	bool getProp( int nId, _Tp &iProp )
	{
		if( nId < 0 || nId >= _M_daTrie._M_size ) return false;
		iProp = _M_daTrie._M_p_prop[nId].sProp;
		return true;
	}

	uint16_t *find( int nId )
	{
		if( nId < 0 || nId >= _M_daTrie._M_size ) return NULL;
		return _M_daTrie._M_p_text + _M_daTrie._M_p_prop[nId].offset;
	}

	int find( uint16_t *sWord )
	{
		int nID = -1;
		prefixSrch( sWord, NULL, &nID );
		return nID;
	}

	//	for win32
	//	保存词典到文件
	int saveDict( char * sIndexName )
	{
		int i;
		int len;
		HANDLE hFile	= NULL;
		DWORD dwDummy	= 0;
		daTrieHead __daHead;

		if ( _M_daTrie._M_size == 0 )
		{
			return 0;
		}

		hFile = CreateFile( sIndexName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFile)
		{
			//	Write HEADER INFO
			memcpy( & __daHead, & _M_daTrie, sizeof( daTrieHead ) );

			//	Write daElm
			for ( i = _M_daTrie._M_n_elm - 1; i >= 0; i-- )
			{
				if ( __getCheck( i ) > 0 )
				{
					break;
				}
			}
			__daHead._M_n_elm	= i + 1;
			__daHead._M_n_prop	= ( __daHead._M_size > __daHead._M_n_prop ) ? __daHead._M_n_prop : __daHead._M_size + 1;
			__daHead._M_n_text	= __daHead._M_n_offset + 1;
			
			//	HEADER
			dwDummy	= 0;
			WriteFile( hFile, &__daHead, sizeof( daTrieHead ), &dwDummy, NULL );
			//i = fwrite( &__daHead, sizeof(char), sizeof( daTrieHead ), fp );
			assert( dwDummy == sizeof( daTrieHead ) );

			//	daElm
			len = sizeof(char);
			len = sizeof( daElm );
			len = sizeof( daElm ) * __daHead._M_n_elm;
			dwDummy	= 0;
			WriteFile( hFile, __daHead._M_p_elm, len, &dwDummy, NULL );
			//i = fwrite( __daHead._M_p_elm , sizeof(char), len, fp );
			assert( (size_t)dwDummy == len );

			//	wordProp
			len = sizeof( wordProp ) * __daHead._M_n_prop;
			dwDummy	= 0;
			WriteFile( hFile, __daHead._M_p_prop, len, &dwDummy, NULL );
			//i = fwrite( __daHead._M_p_prop, sizeof(char), len, fp );
			assert( (size_t)dwDummy == len );

			//	_KEYBUF
			len = sizeof( uint16_t ) * __daHead._M_n_text;
			dwDummy	= 0;
			WriteFile( hFile, __daHead._M_p_text, len, &dwDummy, NULL );
			//i = fwrite( __daHead._M_p_text, sizeof(char), len, fp );
			assert( (size_t)dwDummy == len );

			CloseHandle( hFile );

			return 1;
		}
		else
		{
			return 0;
		}
	}

	//
	//	装载编译后的词典
	//
	int loadDict( const char * sIndexName, bool bRdOnly = false )
	{
		/*
		if ( bRdOnly )
		{
			return __static_load( sIndexName );
		}
		else
		{
			//	一般是这里
			return __dynac_load( sIndexName );
		}*/
		return __dynac_load( sIndexName );
	}

};

#endif





















