/************************************************************
 *
 *	VwTDaTrieAc.h
 *
 *	AUTHOR  : liuqixing@gmail.com
 *
 *	FUNCTION: FAST and COMPACT TRIE for ASCII 1B1 letter match.
 *		: 双向字典 WORD->ID ID->WORD
 *
 *	HISTORY	: 2009-03-12 刘其星 liuqixing@gmail.com 创建
 *
 ************************************************************/

#ifndef __VWTEMPLATEDATRIEAC_HEADER__
#define __VWTEMPLATEDATRIEAC_HEADER__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



/**
 *	变量类型定义
 */
typedef unsigned char		__charwt;	//	char word type
//typedef unsigned short	__charwt;


/**
 *	编译后的词典文件头
 */
#define CVWTDATRIEAC_MAGIC		"VW.IFM.DTRIE"
#define CVWTDATRIEAC_MAGIC_LEN		12
#define CVWTDATRIEAC_ARRAY_SIZE		( 1 << 16 )



/**
 *	template of CDeTTrieAc
 */
template < class _Tp > class CVwTDaTrieAc
{
public:
	typedef struct tagWordProp
	{
		_Tp sProp;
		INT offset;

	}STWORDPROP, *LPSTWORDPROP;

	typedef struct tagDaElm
	{
		INT nBase;
		INT nCheck;

	}STDAELM, *LPSTDAELM;

	typedef struct tagResolveElm
	{
		INT c;
		INT nBase;

	}STRESOLVEELM, *LPSTRESOLVEELM;

	typedef struct tagResolveStat
	{
		INT		nCount;
		STRESOLVEELM	stArrElm[ CVWTDATRIEAC_ARRAY_SIZE ];

	}STRESOLVESTAT, *LPSTRESOLVESTAT;

	typedef struct tagDaTrieHead
	{
		CHAR		szMagic[ CVWTDATRIEAC_MAGIC_LEN ];
		bool		bCase;
		INT		nSize;
		INT		nCount;
		STDAELM *	pstElm;
		INT		nProp;
		STWORDPROP *	pstProp;
		INT		nText;
		__charwt *	puszText;
		INT		nOffset;

	}STDATRIEHEAD, *LPSTDATRIEHEAD;

private:
	INT		m_nSpeed;
	INT		m_nLeft;	
	STDATRIEHEAD	m_stTrieHead;
	INT		m_nMap;
	CHAR *		m_pszMap;

public:

	CVwTDaTrieAc( INT bCase = 1, INT nSpeed = 0 )
	{
		m_nSpeed	= abs( nSpeed ) % 129;
	   	m_nLeft		= 2;
		m_nMap		= 0;
		m_pszMap	= NULL;

		__Initialize( bCase ? true : false );
	}
	~CVwTDaTrieAc()
	{
		__clean_memory( ); 
	}

	VOID cleanMemory()
	{
		__clean_memory();
	}

	VOID clearIndex()
	{
		m_stTrieHead.nSize	= 0;
		m_stTrieHead.nOffset	= 0;
		memset( m_stTrieHead.pstElm , 0, sizeof(STDAELM) * m_stTrieHead.nCount );
		memset( m_stTrieHead.pstProp, 0, sizeof(STWORDPROP) * m_stTrieHead.nProp );

		__setBase( 1, 1 );
		__setCheck( 1, -1);
	}

	//	返回词的个数
	INT size()
	{
		return m_stTrieHead.nSize;
	}

	bool prefixSrch( __charwt * lpuszText, _Tp ** ppProp = NULL, INT * pId = NULL )
	{
		if ( NULL == lpuszText )
		{
			return false;
		}
		if ( 0 == m_stTrieHead.nSize )
		{
			return false;
		}

		INT left, next, parent = 1;
		__charwt * p;

		left = parent + __getBase( parent );	
		for ( p = lpuszText; *p; p++ )
		{
			if ( m_stTrieHead.bCase )
			{
				next = left + toupper( *p );
			}
			else
			{
				next = left + *p;
			}
			if ( __getCheck( next ) != parent )
			{
				return false;
			}
			parent = next;
			left = parent + __getBase( parent );
		}

		if ( __getCheck( left ) != parent )
		{
			return false;
		}

		if ( pId != NULL )
		{
			*pId = __getBase( left );
		}
		if ( ppProp != NULL )
		{
			*ppProp = &m_stTrieHead.pstProp[__getBase( left )].sProp;
		}
		return true;
	}

	//
	//	前匹配 到 0 结束
	//
	INT prefixMatch( __charwt * lpuszText, _Tp ** ppProp = NULL, INT *pId = NULL)
	{
		if ( NULL == lpuszText )
		{
			return 0;
		}
		if ( 0 == m_stTrieHead.nSize )
		{
			return 0;
		}

		INT left,next,parent = 1;
		INT acceptPOINT = 0;
		__charwt *p, *pOK;

		__try
		{
			left = parent + __getBase( parent );	
			for ( pOK = p = lpuszText; *p; p++ )
			{
				if ( m_stTrieHead.bCase )
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
							*ppProp = &m_stTrieHead.pstProp[__getBase( acceptPOINT )].sProp;
						}
						return (INT)( pOK - lpuszText );
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
				if ( pId != NULL )
				{
					*pId = __getBase( acceptPOINT );
				}
				if ( ppProp != NULL )
				{
					*ppProp = &m_stTrieHead.pstProp[__getBase( acceptPOINT )].sProp;
				}
				return (INT)( pOK - lpuszText );
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}

		return 0;
	}
	
	//
	//	前匹配 指定匹配长度
	//
	INT prefixMatch( __charwt * lpuszText, INT nLen, _Tp ** ppProp = NULL, INT * pId = NULL )
	{
		//
		//	lpuszText	- 待匹配字符串
		//	nLen		- 匹配长度，也许可以用来匹配二进制，遇到 0 不结束，而是匹配到指定长度
		//	ppProp		-
		//	pId		-
		//	RETURN		-
		//

		if ( NULL == lpuszText || 0 == nLen )
		{
			return 0;
		}
		if ( 0 == m_stTrieHead.nSize )
		{
			return 0;
		}

		INT left, next, parent = 1;
		INT acceptPOINT = 0;
		__charwt * p, * q, * pOK;

		left	= parent + __getBase( parent );
		for ( pOK = p = lpuszText, q = lpuszText + nLen; p < q; p ++ )
		{
			if ( m_stTrieHead.bCase )
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
						*ppProp = &m_stTrieHead.pstProp[ __getBase( acceptPOINT ) ].sProp;
					}
					return (INT)( pOK - lpuszText );
				}
				return 0;
			}
			parent	= next;
			left	= parent + __getBase( parent );
			if ( __getCheck( left ) == parent )
			{
				pOK = p + 1;
				acceptPOINT = left;
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
				*ppProp = &m_stTrieHead.pstProp[ __getBase( acceptPOINT ) ].sProp;
			}
			return (INT)( pOK - lpuszText );
		}
		return 0;
	}

	INT insert( __charwt * lpuszWord, const _Tp &iProp = 0 )
	{
		//
		//	RETURN VAL: 
		//	-1:	ERROR
		//	0-N:	WORDID
		//
		if ( NULL == lpuszWord )
		{
			return -1;
		}

		INT i, next, parent = 1;
		bool bInsert = false;
		__charwt c;

		INT nLen = __word_len( lpuszWord );
		if ( 0 == nLen )
		{
			return -1;
		}

		for ( i = 0; i <= nLen; i++ )
		{
			if ( m_stTrieHead.bCase )
			{
				c = toupper( lpuszWord[ i ] );
			}
			else
			{
				c = lpuszWord[i];
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
			m_stTrieHead.nSize ++;
			__setText( parent, lpuszWord, nLen );
		}
		__setProp( parent, iProp );

		return __getBase( parent );
	}

	bool erase( __charwt * lpuszWord )
	{
		if ( NULL == lpuszWord )
		{
			return false;
		}
		if ( 0 == m_stTrieHead.nSize )
		{
			return false;
		}

		INT i, next, parent = 1;

		INT nLen = __word_len( lpuszWord );
		if ( nLen == 0 )
		{
			return false;
		}

		for ( i = 0; i <= nLen; i++ )
		{
			if ( m_stTrieHead.bCase )
			{
				next = parent + __getBase( parent ) + toupper( lpuszWord[i] );
			}
			else
			{
				next = parent + __getBase( parent ) + lpuszWord[i];
			}
			if ( __getCheck( next ) != parent )
			{
				return false;
			}
			parent = next;
		}
		__setBase( parent, 0 ); __setCheck( parent, 0 );
		return true;
	}

	bool setProp( __charwt * lpuszWord, const _Tp &iProp )
	{
		if ( NULL == lpuszWord )
		{
			return false;
		}

		_Tp * pProp;
		bool bSuc = prefixSrch( lpuszWord, &pProp );
		if( bSuc )
		{
			pProp->sProp = iProp;
		}
		return bSuc;
	}

	bool setProp( INT nId, const _Tp &iProp)
	{
		if( nId < 0 || nId >= m_stTrieHead.nSize ) return false;
		m_stTrieHead.pstProp[nId].sProp = iProp;
		return true;
	}

	bool getProp( __charwt * lpuszWord, _Tp &iProp )
	{
		if ( NULL == lpuszWord )
		{
			return false;
		}

		_Tp * pProp;
		bool bSuc = prefixSrch( lpuszWord, &pProp );
		if ( bSuc )
		{
			iProp = pProp->sProp;
		}
		return bSuc;
	}

	bool getProp( INT nId, _Tp &iProp )
	{
		if( nId < 0 || nId >= m_stTrieHead.nSize ) return false;
		iProp = m_stTrieHead.pstProp[nId].sProp;
		return true;
	}

	__charwt * find( INT nId )
	{
		if( nId < 0 || nId >= m_stTrieHead.nSize ) return NULL;
		return m_stTrieHead.puszText + m_stTrieHead.pstProp[nId].offset;
	}

	INT find( __charwt * lpuszWord )
	{
		INT nID = -1;
		if ( lpuszWord )
		{
			prefixSrch( lpuszWord, NULL, &nID );
		}
		return nID;
	}

	//	for win32
	//	保存词典到文件
	INT saveDict( LPCTSTR lpcszDictFilename )
	{
		INT i;
		INT len;
		HANDLE hFile	= NULL;
		DWORD dwDummy	= 0;
		STDATRIEHEAD stTrieHead;

		if ( 0 == m_stTrieHead.nSize )
		{
			return 0;
		}

		hFile = CreateFile( lpcszDictFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFile)
		{
			//	Write HEADER INFO
			memcpy( & stTrieHead, & m_stTrieHead, sizeof( STDATRIEHEAD ) );

			//	Write STDAELM
			for ( i = m_stTrieHead.nCount - 1; i >= 0; i-- )
			{
				if ( __getCheck( i ) > 0 )
				{
					break;
				}
			}
			stTrieHead.nCount	= i + 1;
			stTrieHead.nProp	= ( stTrieHead.nSize > stTrieHead.nProp ) ? stTrieHead.nProp : stTrieHead.nSize + 1;
			stTrieHead.nText	= stTrieHead.nOffset + 1;
			
			//	HEADER
			dwDummy	= 0;
			WriteFile( hFile, &stTrieHead, sizeof( STDATRIEHEAD ), &dwDummy, NULL );
			//i = fwrite( &stTrieHead, sizeof(CHAR), sizeof( STDATRIEHEAD ), fp );
			assert( dwDummy == sizeof( STDATRIEHEAD ) );

			//	STDAELM
			len = sizeof(CHAR);
			len = sizeof( STDAELM );
			len = sizeof( STDAELM ) * stTrieHead.nCount;
			dwDummy	= 0;
			WriteFile( hFile, stTrieHead.pstElm, len, &dwDummy, NULL );
			//i = fwrite( stTrieHead.pstElm , sizeof(CHAR), len, fp );
			assert( (size_t)dwDummy == (size_t)len );

			//	STWORDPROP
			len = sizeof( STWORDPROP ) * stTrieHead.nProp;
			dwDummy	= 0;
			WriteFile( hFile, stTrieHead.pstProp, len, &dwDummy, NULL );
			//i = fwrite( stTrieHead.pstProp, sizeof(CHAR), len, fp );
			assert( (size_t)dwDummy == (size_t)len );

			//	_KEYBUF
			len = sizeof( __charwt ) * stTrieHead.nText;
			dwDummy	= 0;
			WriteFile( hFile, stTrieHead.puszText, len, &dwDummy, NULL );
			//i = fwrite( stTrieHead.puszText, sizeof(CHAR), len, fp );
			assert( (size_t)dwDummy == (size_t)len );

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
	INT loadDict( LPCTSTR lpcszDictFilename, bool bRdOnly = false )
	{
		return __dynac_load( lpcszDictFilename );
	}


private:
	INT __word_len( __charwt * s )
	{
		if ( s == NULL )
		{
			return 0;
		}
		__charwt * p = s;
		while( *p )
		{
			p++;
		}
		return (INT)( p - s );
	}

	VOID __resizeElm( INT nIdx )
	{
		if ( nIdx >= m_stTrieHead.nCount )
		{
			size_t __nsize	= ( ( nIdx + 511 ) & ~511 ) * 3 / 2;
			STDAELM * p	= (STDAELM *)realloc( m_stTrieHead.pstElm, __nsize * sizeof(STDAELM) );
			if ( p == NULL )
			{
				__nsize = ( nIdx + 1023) & ~511;
				p = (STDAELM *)realloc( m_stTrieHead.pstElm, __nsize * sizeof(STDAELM) );
				assert ( p != NULL );
			}
			m_stTrieHead.pstElm = p;
			for ( size_t i = m_stTrieHead.nCount ; i < __nsize; i++ )
			{
				m_stTrieHead.pstElm[i].nBase	= 0;
				m_stTrieHead.pstElm[i].nCheck	= 0;
			}
			m_stTrieHead.nCount = __nsize;
		}
	}

	VOID __resizeProp( INT nIdx )
	{
		if ( nIdx >= m_stTrieHead.nProp )
		{
			size_t __nsize	= ( nIdx + 127 ) & ~63;
			m_stTrieHead.pstProp = (STWORDPROP *)realloc( m_stTrieHead.pstProp, __nsize * sizeof(STWORDPROP) );
			assert( m_stTrieHead.pstProp != NULL );
			memset( m_stTrieHead.pstProp + m_stTrieHead.nProp, 0, sizeof( STWORDPROP ) * ( __nsize - m_stTrieHead.nProp) );
			m_stTrieHead.nProp = __nsize;
		}
	}

	VOID __resizeText( INT nInc )
	{
		if ( nInc + m_stTrieHead.nOffset >= m_stTrieHead.nText )
		{
			size_t __nsize	= ( ( m_stTrieHead.nOffset + nInc + 1023 ) & ~1023) * 3 / 2;
			__charwt * p	= (__charwt *)realloc( m_stTrieHead.puszText, __nsize * sizeof(__charwt) );
			if ( p == NULL )
			{
				__nsize = ( m_stTrieHead.nOffset + nInc + 1023 ) & ~1023;
				p = (__charwt *)realloc( m_stTrieHead.puszText, __nsize * sizeof(__charwt) );
				assert( p != NULL );
			}
			m_stTrieHead.puszText	= p;
			m_stTrieHead.nText	= __nsize;
		}
	}

	INT __getBase( INT nIdx )
	{
		return ( nIdx >= m_stTrieHead.nCount ) ? 0 : m_stTrieHead.pstElm[nIdx].nBase;
	}

	VOID __setBase( INT nIdx, INT v )
	{
		__resizeElm( nIdx );
		m_stTrieHead.pstElm[nIdx].nBase = v;
	}

	INT __getCheck( INT nIdx )
	{
		return ( nIdx >= m_stTrieHead.nCount ) ? 0 : m_stTrieHead.pstElm[ nIdx ].nCheck;
	}

	VOID __setCheck( INT nIdx, INT v )
	{
		__resizeElm( nIdx );
		m_stTrieHead.pstElm[nIdx].nCheck = v;
	}

	_Tp *__getProp( INT nIdx )
	{
		INT n = __getBase( nIdx );
		return ( n >= m_stTrieHead.nProp ) ? NULL : &m_stTrieHead.pstProp[n].sProp;
	}

	VOID __setProp( INT nIdx, const _Tp &iProp )
	{
		INT n = __getBase( nIdx );
		__resizeProp( n ); 
		m_stTrieHead.pstProp[n].sProp = iProp;
	}

	VOID __setText( INT nIdx, __charwt *__p, INT __len )
	{
		INT n = __getBase( nIdx ); 
		__resizeProp( n ); __resizeText ( __len + 1 );
		m_stTrieHead.pstProp[n].offset = m_stTrieHead.nOffset;
		memcpy( m_stTrieHead.puszText + m_stTrieHead.nOffset, __p, __len * sizeof(__charwt) );
		m_stTrieHead.puszText[m_stTrieHead.nOffset + __len] = 0;
		m_stTrieHead.nOffset += __len + 1;
	}

	VOID __pushChild( STRESOLVESTAT *pStat, INT c, INT orig_base)
	{
		INT n = pStat->nCount;
		pStat->nCount++;
		pStat->stArrElm[n].c = c;
		pStat->stArrElm[n].nBase = orig_base;
	}

	INT __collectChild( STRESOLVESTAT *pStat, INT parent, INT c )
	{
		INT i,idx,nLeft = m_nLeft;
		INT base = __getBase( parent );

		pStat->nCount = 0;
		for ( i = 0; i < CVWTDATRIEAC_ARRAY_SIZE; i++ )
		{
			idx = parent + base + i;
			if ( i == c )
			{
				__pushChild( pStat, i, ( c == 0 ) ? (m_stTrieHead.nSize + 1) : 1 );
			}
			else if ( __getCheck( idx ) == parent )
			{
				__pushChild( pStat, i, __getBase(idx) );
			}
		}
	
		i = 0;
		while( i < pStat->nCount )
		{
			if ( __getCheck( nLeft + pStat->stArrElm[i].c ) != 0 )
			{
				nLeft++;
				i = 0;
				continue;
			}
			i++;
		}

		INT nChr = CVWTDATRIEAC_ARRAY_SIZE / 2 - m_nSpeed * 256;
		if ( pStat->nCount >= nChr )
		{
			m_nLeft = nLeft;
		}

		for ( i = 0; i < pStat->nCount; i++ )
		{
			if ( pStat->stArrElm[i].c != c )
			{
				idx = parent + base + pStat->stArrElm[i].c;
				__setBase ( idx, 0 ); __setCheck( idx, 0 );
			}
		}

		return nLeft;
	}

	VOID __moveChildren( STRESOLVESTAT *pStat, INT parent, INT left )
	{
		INT i,move;
		INT orig_base = __getBase( parent );

		//move = old_left - new_left = ( parent + base(parent) ) - left;
		move = parent + orig_base - left;
		//new_base = orig_base - move = orig_base - ( parent + orig_base - left )
		__setBase( parent, left - parent );

		for ( i = 0; i < pStat->nCount; i++ )
		{
			INT idx = left + pStat->stArrElm[i].c;
			__setCheck( idx, parent );
			if( pStat->stArrElm[i].c == 0 )
				__setBase ( idx, pStat->stArrElm[i].nBase );
			else
				__setBase ( idx, pStat->stArrElm[i].nBase + move);
		}

		for ( i = 0; i < pStat->nCount; i++ )
		{
			INT k;
			INT old_left, old_child;

			//old_child = t = s + base(s) + c
			old_child = parent + orig_base + pStat->stArrElm[i].c; 
			//s = t = old_child ; left = old_child + base(old_child) = old_child + pStat->pstElm[i].nBase
			old_left = old_child + pStat->stArrElm[i].nBase;
		
			for ( k = 0; k < CVWTDATRIEAC_ARRAY_SIZE; k++ )
			{
				INT orig_check = __getCheck( old_left + k);
				if ( orig_check == old_child && __getCheck( orig_check ) == 0 )
				{
					__setCheck( old_left + k, orig_check - move );
				}
			}
		}
	}

	INT __resolveConflict( INT parent, __charwt c )
	{
		INT nLeft;
		STRESOLVESTAT curChild;

		nLeft = __collectChild( &curChild,  parent, c);
		__moveChildren( &curChild, parent, nLeft);
		return nLeft + c;
	}

	INT __addChr( INT nParent, __charwt cChar )
	{
		INT nNext = nParent + __getBase( nParent ) + cChar;
		if ( __getCheck( nNext ) > 0 )
		{
			return __resolveConflict( nParent , cChar );
		}

		__setCheck( nNext, nParent );

		if ( 0 == cChar )
		{
			__setBase( nNext, m_stTrieHead.nSize + 1 );
		}
		else
		{
			__setBase( nNext, ( 1 == nParent ) ? ( CVWTDATRIEAC_ARRAY_SIZE - nNext ) : 1 );
		}

		return nNext;
	}

	VOID __Initialize( bool bCase )
	{
		memset( &m_stTrieHead, 0, sizeof( STDATRIEHEAD ) );
		memcpy( m_stTrieHead.szMagic, CVWTDATRIEAC_MAGIC, CVWTDATRIEAC_MAGIC_LEN );
		m_stTrieHead.bCase = bCase;
		
		__setBase ( 1, 1 );
		__setCheck( 1, -1);
	}

	VOID __clean_memory()
	{
		if ( m_pszMap )
		{
			//
			//	释放共享内存
			//
			//munmap( m_pszMap, m_nMap );
			m_pszMap = NULL;
			m_nMap = 0;
		}
		else
		{
			//
			//	释放本进程内内存
			//
			if ( m_stTrieHead.pstElm )
			{
				free( m_stTrieHead.pstElm );
				m_stTrieHead.pstElm = NULL;
			}
			if ( m_stTrieHead.pstProp )
			{
				free( m_stTrieHead.pstProp );
				m_stTrieHead.pstProp = NULL;
			}
			if ( m_stTrieHead.puszText)
			{
				free( m_stTrieHead.puszText );
				m_stTrieHead.puszText = NULL;
			}
		}

		memset( &m_stTrieHead, 0, sizeof( STDATRIEHEAD ) );
	}

private:

	INT __dynac_load( LPCTSTR lpcszDictFilename )
	{
		if ( NULL == lpcszDictFilename || 0 == _tcslen(lpcszDictFilename) )
		{
			return 0;
		}

		HANDLE hFile		= NULL;
		DWORD dwDummy		= 0;
		STDATRIEHEAD stTrieHead;

		hFile = CreateFile( lpcszDictFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE != hFile )
		{
			//	HEADER
			dwDummy = 0;
			ReadFile( hFile, &stTrieHead, sizeof( STDATRIEHEAD ), &dwDummy, NULL );
			//i = fread( &stTrieHead, sizeof(CHAR), sizeof( STDATRIEHEAD ), fp );
			if ( dwDummy != sizeof( STDATRIEHEAD ) )
			{
				CloseHandle( hFile );
				return 0;
			}
			if ( memcmp( stTrieHead.szMagic, CVWTDATRIEAC_MAGIC, CVWTDATRIEAC_MAGIC_LEN ) )
			{
				CloseHandle( hFile );
				return 0;
			}

			//
			//	决定装载前，释放现在的内存
			//
			__clean_memory();

			//
			//	重新申请内存
			//
			stTrieHead.pstElm = (STDAELM *)malloc( sizeof( STDAELM ) * stTrieHead.nCount );
			assert( stTrieHead.pstElm != NULL );
			stTrieHead.pstProp = (STWORDPROP *)malloc( sizeof( STWORDPROP ) * stTrieHead.nProp );
			assert( stTrieHead.pstProp != NULL );
			stTrieHead.puszText = (__charwt *)malloc( sizeof( __charwt ) * stTrieHead.nText );
			assert( stTrieHead.puszText != NULL );
			
			//	STDAELM
			dwDummy = 0;
			ReadFile( hFile, stTrieHead.pstElm, sizeof( STDAELM ) * stTrieHead.nCount, &dwDummy, NULL );
			//i = fread( stTrieHead.pstElm, sizeof(CHAR), sizeof( STDAELM ) * stTrieHead.nCount, fp );
			if ( dwDummy != (INT)( sizeof( STDAELM ) * stTrieHead.nCount ) )
			{
				CloseHandle( hFile );
				return 0;
			}

			//	STWORDPROP
			dwDummy = 0;
			ReadFile( hFile, stTrieHead.pstProp, sizeof( STWORDPROP ) * stTrieHead.nProp, &dwDummy, NULL );
			//i = fread( stTrieHead.pstProp, sizeof(CHAR), sizeof( STWORDPROP ) * stTrieHead.nProp, fp );
			if ( dwDummy != (INT)(sizeof( STWORDPROP ) * stTrieHead.nProp ) )
			{
				CloseHandle( hFile );
				return 0;
			}
			
			//	text
			ReadFile( hFile, stTrieHead.puszText, sizeof( __charwt ) * stTrieHead.nText, &dwDummy, NULL );
			//i = fread( stTrieHead.puszText, sizeof(CHAR), sizeof( __charwt ) * stTrieHead.nText, fp );
			if ( dwDummy != (INT)(sizeof( __charwt ) * stTrieHead.nText ) )
			{
				CloseHandle( hFile );
				return 0;
			}

			memcpy( &m_stTrieHead, &stTrieHead, sizeof( STDATRIEHEAD ) );

			CloseHandle( hFile );
			return 1;
		}
		else
		{
			return 0;
		}
	}

};




#endif	// __VWTEMPLATEDATRIEAC_HEADER__