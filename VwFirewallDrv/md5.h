/*
**********************************************************************
** md5.h -- Header file for implementation of MD5                   **
** RSA Data Security, Inc. MD5 Message Digest Algorithm             **
** Created: 2/17/90 RLR                                             **
** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version              **
** Revised (for MD5): RLR 4/27/91                                   **
**   -- G modified to have y&~z instead of y&z                      **
**   -- FF, GG, HH modified to add in last register done            **
**   -- Access pattern: round 2 works mod 5, round 3 works mod 3    **
**   -- distinct additive constant for each step                    **
**   -- round 4 added, working mod 7                                **
**********************************************************************
*/
/*
**********************************************************************
** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
**                                                                  **
** License to copy and use this software is granted provided that   **
** it is identified as the "RSA Data Security, Inc. MD5 Message     **
** Digest Algorithm" in all material mentioning or referencing this **
** software or this function.                                       **
**                                                                  **
** License is also granted to make and use derivative works         **
** provided that such works are identified as "derived from the RSA **
** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
** material mentioning or referencing the derived work.             **
**                                                                  **
** RSA Data Security, Inc. makes no representations concerning      **
** either the merchantability of this software or the suitability   **
** of this software for any particular purpose.  It is provided "as **
** is" without express or implied warranty of any kind.             **
**                                                                  **
** These notices must be retained in any copies of any part of this **
** documentation and/or software.                                   **
**********************************************************************
*/


//	typedef a 32 bit type
#ifndef __UINT4
#define __UINT4
	typedef unsigned long int UINT4;
#endif

//	Data structure for MD5 (Message Digest) computation
typedef struct tagMd5Ctx
{
	UINT4 i[2];			//	number of _bits_ handled mod 2^64
	UINT4 buf[4];			//	scratch buffer
	unsigned char in[64];		//	input buffer
	unsigned char digest[16];	//	actual digest after MD5Final call
} MD5_CTX;

static unsigned char PADDING[64] =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//	F, G and H are basic MD5 functions: selection, majority, parity
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

//	ROTATE_LEFT rotates x left n bits
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//	FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
//	Rotation is separate from addition to prevent recomputation
#define FF(a, b, c, d, x, s, ac) \
{(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) \
{(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) \
{(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) \
{(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}




//BOOL md5_string( LPCTSTR lpszString, DWORD dwSpecStringLen, LPTSTR lpszMd5, DWORD dwSize );
BOOLEAN md5_string( UCHAR * puszString, DWORD dwSpecStringLen, UCHAR * puszMd5, DWORD dwSize );

VOID md5_func_init();
VOID md5_func_update();
void md5_func_final();

VOID md5_func_transform( UINT4 * buf, UINT4 * in );