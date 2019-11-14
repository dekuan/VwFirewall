// MultiString.cpp: implementation of the CMultiString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiString.h"
#include <assert.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiString::CMultiString()
{
	m_nBufLen=0;
	m_pBuf=NULL;
}

CMultiString::~CMultiString()
{

}

/*
	下面两个函数中，StringList是一个char**的指针(ppList)
	可以用类似下面的循环进行处理
	while(*p)	//	最后一个字符串指针为NULL
	{
		*p是以0结尾的字符串，在这里可以进行处理
		...
		p++;
	}
*/

/*
	将pBuf指向的长度为nBufLen的MultiString格式的缓冲区转换为StringList
	如果转换成功，则返回StringList的指针，String的个数通过pnNum返回
	如果转换失败，则返回NULL
	StringList自己的空间由函数的调用者释放
	其指向的每一个字符串的空间为传入的pBuf，因此不能释放StringList
	的每一个字符串，只需要释放pBuf即可（如果有必要的话）
	因此，函数调用者也不应随意修改每一个String的值，应将其看为只读的
*/
char** CMultiString::MultiStringToStringList(char *pBuf, int nBufLen, int *pnNum)
{
	if ( nBufLen < 2 )	//	空的MultiString(只有一个\0)
		return NULL;

	TCHAR ** ppRet		= NULL;
	INT   nCount		= 0;
	TCHAR * p = pBuf + nBufLen - 1;
	INT   nTailZero		= 0;

	do
	{
		if ( '\0' == *p )
			nTailZero ++;
		p--;
	}
	while( ('\0'==*p) && (p>pBuf) );

	assert( nTailZero > 1 );
	if((nTailZero<2) || ('\0' == *p))
		return NULL;

	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}
	while ( p > pBuf );

	ppRet = (TCHAR**)malloc( (nCount+sizeof(TCHAR))*sizeof(TCHAR*) );
	if ( NULL == ppRet )
		return NULL;

	p = pBuf;
	for ( int i = 0; i < nCount; i++ )
	{
		ppRet[i]	= p;
		while( *p++ );
	}
	ppRet[nCount] = NULL;
	*pnNum = nCount;

	return ppRet;
}
WCHAR ** CMultiString::MultiStringToStringListW( WCHAR * pBuf, INT nBufLen, INT * pnNum )
{
	if ( nBufLen < 2 )	//	空的MultiString(只有一个\0)
		return NULL;
	
	WCHAR ** ppRet		= NULL;
	INT   nCount		= 0;
	WCHAR * p = pBuf + nBufLen - 1;
	INT   nTailZero		= 0;

	do
	{
		if ( '\0' == *p )
			nTailZero ++;
		p--;
	}
	while( ('\0'==*p) && (p>pBuf) );
	
	assert( nTailZero > 1 );
	if((nTailZero<2) || ('\0' == *p))
		return NULL;
	
	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}
	while ( p > pBuf );
	
	ppRet = (WCHAR**)malloc( (nCount+sizeof(WCHAR))*sizeof(WCHAR*) );
	if ( NULL == ppRet )
		return NULL;
	
	p = pBuf;
	for ( int i = 0; i < nCount; i++ )
	{
		ppRet[i]	= p;
		while( *p++ );
	}
	ppRet[nCount] = NULL;
	*pnNum = nCount;

	return ppRet;
}

/*
	将pBuf指向的长度为nBufLen的MultiString格式的缓冲区转换为StringList
	最多转换*pnNum-1个，StringList由调用者提供缓冲区(ppStrList)
	如果转换成功，则返回TRUE，同时通过pnNum给出string的个数
	如果转换失败，则返回FALSE
*/
BOOL CMultiString::MultiStringToStringList(char* pBuf, int nBufLen, char** ppStrList, IN OUT int* pnNum)
{
	int nCount;
	char* p=pBuf+nBufLen-1;
	int	nTailZero;
	nTailZero=0;
	do
	{
		if(*p=='\0')
			nTailZero++;
		p--;
	}while((*p=='\0') && (p>pBuf));

	assert(nTailZero > 1);
	if((nTailZero<2) || ('\0' == *p))
		return FALSE;

	p++;
	nCount=0;
	do
	{
		if(*p=='\0')
			nCount++;
		p--;
	}while(p>pBuf);

	p=pBuf;
	for(int i=0;(i<nCount)&&(i<*pnNum-1);i++)
	{
		ppStrList[i]=p;
		while(*p++)	;
	}
	ppStrList[i]=NULL;
	*pnNum=i;

	return TRUE;
}

/*
	将ppStrList指向的StringList转换为MultiString格式的缓冲区
	最多转换nStrNum个或者StringList包括的所有的字符串
	如果转换成功，返回指向MultiString的指针，长度通过pnLen返回
	如果转换失败，则返回NULL
	MultiString的空间由函数调用者释放
*/
char* CMultiString::StringListToMultiString(char **ppStrList, int nStrNum, int *pnLen)
{
	int i;
	int nLen;
	char** ppList;
	char* pRet;
	char* pCopy;
	i=0;
	nLen=0;
	ppList=ppStrList;
	while((*ppList) && (i<nStrNum))
	{
		nLen+=strlen(*ppList);
		nLen++;	//	for the last '\0'
		ppList++;
		i++;
	}
	nLen++;	//	for the last two '\0'

	pRet=(char*)malloc(nLen*sizeof(char));
	if(NULL == pRet)
		return NULL;

	pCopy=pRet;
	i=0;
	ppList=ppStrList;
	while((*ppList) && (i<nStrNum))
	{
		strcpy(pCopy,*ppList);
		pCopy+=strlen(*ppList);
		pCopy++;

		ppList++;
		i++;
	}
	*pCopy='\0';
	*pnLen=nLen;
	return pRet;
}
