//////////////////////////////////////////////////////////////////////////
//	RdpStruct.h

#ifndef __RDPSTRUCT_HEADER__
#define __RDPSTRUCT_HEADER__


#pragma pack(1)


enum
{
	CS_CORE		= 0xC001,
	CS_SECURITY	= 0xC002,
	CS_NET		= 0xC003,
	CS_CLUSTER	= 0xC004,
	CS_MONITOR	= 0xC005
};
enum
{
	SC_CORE		= 0x0C01,
	SC_SECURITY	= 0x0C02,
	SC_NET		= 0x0C03
};

//	TS_UD_CS_CORE :: colorDepth
//	TS_UD_CS_CORE :: postBeta2ColorDepth
enum
{
	RNS_UD_COLOR_4BPP	= 0xCA00,	//	4 bits-per-pixel (bpp)
	RNS_UD_COLOR_8BPP	= 0xCA01,	//	8 bpp
	RNS_UD_COLOR_16BPP_555	= 0xCA02,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
	RNS_UD_COLOR_16BPP_565	= 0xCA03,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
	RNS_UD_COLOR_24BPP	= 0xCA04	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)
};

//	TS_UD_CS_CORE :: highColorDepth
enum
{
	HIGH_COLOR_4BPP		= 0x0004,	//	4 bpp
	HIGH_COLOR_8BPP		= 0x0008,	//	8 bpp
	HIGH_COLOR_15BPP	= 0x000F,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
	HIGH_COLOR_16BPP	= 0x0010,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
	HIGH_COLOR_24BPP	= 0x0018	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)
};

//	TS_UD_CS_CORE :: supportedColorDepths
enum
{
	RNS_UD_24BPP_SUPPORT	= 0x0001,	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)
	RNS_UD_16BPP_SUPPORT	= 0x0002,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
	RNS_UD_15BPP_SUPPORT	= 0x0004,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
	RNS_UD_32BPP_SUPPORT	= 0x0008	//	32-bit RGB mask (8 bits for the alpha channel, 8 bits for red, 8 bits for green, and 8 bits for blue)
};

enum
{
	RNS_UD_SAS_DEL	= 0xAA03
};

//	TS_UD_CS_CORE :: earlyCapabilityFlags
enum
{
	RNS_UD_CS_SUPPORT_ERRINFO_PDU		= 0x0001,	//	Indicates that the client supports the Set Error Info PDU (section 2.2.5.1).
	RNS_UD_CS_WANT_32BPP_SESSION		= 0x0002,	//	Indicates that the client is requesting a session color depth of 32 bpp.
								//	This flag is necessary because the highColorDepth field does not support a value of 32. If this flag is set, the highColorDepth field SHOULD be set to 24 to provide an acceptable fallback for the scenario where the server does not support 32 bpp color.
	RNS_UD_CS_SUPPORT_STATUSINFO_PDU	= 0x0004,	//	Indicates that the client supports the Server Status Info PDU (section 2.2.5.2).
	RNS_UD_CS_STRONG_ASYMMETRIC_KEYS	= 0x0008,	//	Indicates that the client supports asymmetric keys larger than 512 bits for use with the Server Certificate (section 2.2.1.4.3.1) sent in the Server Security Data block (section 2.2.1.4.3).
	RNS_UD_CS_VALID_CONNECTION_TYPE		= 0x0020,	//	Indicates that the connectionType field contains valid data.
	RNS_UD_CS_SUPPORT_MONITOR_LAYOUT_PDU	= 0x0040	//	Indicates that the client supports the Monitor Layout PDU (section 2.2.12.1).
};

//	TS_UD_CS_CORE :: connectionType
enum
{
	CONNECTION_TYPE_MODEM		= 0x01,		//	Modem (56 Kbps)
	CONNECTION_TYPE_BROADBAND_LOW	= 0x02,		//	Low-speed broadband (256 Kbps - 2 Mbps)
	CONNECTION_TYPE_SATELLITE	= 0x03,		//	Satellite (2 Mbps - 16 Mbps with high latency)
	CONNECTION_TYPE_BROADBAND_HIGH	= 0x04,		//	High-speed broadband (2 Mbps - 10 Mbps)
	CONNECTION_TYPE_WAN		= 0x05,		//	WAN (10 Mbps or higher with high latency)
	CONNECTION_TYPE_LAN		= 0x06		//	LAN (10 Mbps or higher)
};

//	TS_UD_CS_SEC :: encryptionMethods
enum
{
	E_40BIT_ENCRYPTION_FLAG		= 0x00000001,	//	40-bit session keys MUST be used to encrypt data (with RC4) and generate Message Authentication Codes (MAC).
	E_128BIT_ENCRYPTION_FLAG	= 0x00000002,	//	128-bit session keys MUST be used to encrypt data (with RC4) and generate MACs.
	E_56BIT_ENCRYPTION_FLAG		= 0x00000008,	//	56-bit session keys MUST be used to encrypt data (with RC4) and generate MACs.
	E_FIPS_ENCRYPTION_FLAG		= 0x00000010	//	All encryption and Message Authentication Code generation routines MUST be Federal Information Processing Standard (FIPS) 140-1 compliant.
};

//	BER: Application-Defined Type
enum
{
	MCS_TYPE_CONNECTINITIAL		= 0x65		//	APPLICATION 101 = Connect-Initial
};


//	User Data Header
typedef struct tagTS_UD_HEADER
{
	USHORT type;
	USHORT length;

}TS_UD_HEADER, *LPTS_UD_HEADER;

//	Client Core Data
//	length = 212 = 0xd8
typedef struct tagTS_UD_CS_CORE
{
	TS_UD_HEADER header;		//	(4 bytes):  type field MUST be set to CS_CORE (0xC001)
	UINT version;			//	(4 bytes):  A 32-bit, unsigned integer.
					//		    0x00080001	- RDP 4.0 clients
					//		    0x00080004	- RDP 5.0, 5.1, 5.2, 6.0, 6.1, and 7.0 clients
	USHORT desktopWidth;		//	(2 bytes):  A 16-bit, unsigned integer. The requested desktop width in pixels (up to a maximum value of 4096 pixels).
	USHORT desktopHeight;		//	(2 bytes):  A 16-bit, unsigned integer. The requested desktop height in pixels (up to a maximum value of 2048 pixels).
	USHORT colorDepth;		//	(2 bytes):  A 16-bit, unsigned integer. The requested color depth. Values in this field MUST be ignored if the postBeta2ColorDepth field is present.
					//	            RNS_UD_COLOR_4BPP	- 4 bits-per-pixel (bpp)
					//		    RNS_UD_COLOR_8BPP	- 8 bpp
	USHORT SASSequence;		//	(2 bytes):  A 16-bit, unsigned integer. Secure access sequence. This field SHOULD be set to RNS_UD_SAS_DEL (0xAA03).
	UINT   keyboardLayout;		//	(4 bytes):  A 32-bit, unsigned integer. Keyboard layout (active input locale identifier). For a list of possible input locales, see [MSDN-MUI].
	UINT   clientBuild;		//	(4 bytes):  A 32-bit, unsigned integer. The build number of the client.
	WCHAR  clientName[16];		//	(32 bytes): Name of the client computer. This field contains up to 15 Unicode characters plus a null terminator.

	UINT   keyboardType;		//	(4 bytes):  A 32-bit, unsigned integer. The keyboard type.
					//		    0x00000001	- IBM PC/XT or compatible (83-key) keyboard
					//		    0x00000002	- Olivetti "ICO" (102-key) keyboard
					//		    0x00000003	- IBM PC/AT (84-key) and similar keyboards
					//		    0x00000004	- IBM enhanced (101-key or 102-key) keyboard
					//		    0x00000005	- Nokia 1050 and similar keyboards
					//		    0x00000006	- Nokia 9140 and similar keyboards
					//		    0x00000007	- Japanese keyboard

	UINT   keyboardSubType;		//	(4 bytes):  A 32-bit, unsigned integer. The keyboard subtype (an original equipment manufacturer-dependent value).

	UINT   keyboardFunctionKey;	//	(4 bytes):  A 32-bit, unsigned integer. The number of function keys on the keyboard.

	WCHAR  imeFileName[32];		//	(64 bytes): A 64-byte field. The Input Method Editor (IME) file name associated with the input locale. This field contains up to 31 Unicode characters plus a null terminator.

	USHORT postBeta2ColorDepth;	//	(2 bytes):  A 16-bit, unsigned integer. The requested color depth.
					//		    Values in this field MUST be ignored if the highColorDepth field is present.
					//		    RNS_UD_COLOR_4BPP		= 0xCA00,	//	4 bits-per-pixel (bpp)
					//		    RNS_UD_COLOR_8BPP		= 0xCA01,	//	8 bpp
					//		    RNS_UD_COLOR_16BPP_555	= 0xCA02,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
					//		    RNS_UD_COLOR_16BPP_565	= 0xCA03,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
					//		    RNS_UD_COLOR_24BPP		= 0xCA04	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)

	
	USHORT clientProductId;		//	(2 bytes):  A 16-bit, unsigned integer. The client product ID.
					//		    This field SHOULD be initialized to 1.
					//		    If this field is present, all of the preceding fields MUST also be present.
					//		    If this field is not present, all of the subsequent fields MUST NOT be present.

	UINT   serialNumber;		//	(4 bytes):  A 32-bit, unsigned integer. Serial number.
					//		    This field SHOULD be initialized to 0.
					//		    If this field is present, all of the preceding fields MUST also be present. If this field is not present, all of the subsequent fields MUST NOT be present.

	USHORT highColorDepth;		//	(2 bytes):  A 16-bit, unsigned integer. The requested color depth.
					//		    HIGH_COLOR_4BPP	= 0x0004,	//	4 bpp
					//		    HIGH_COLOR_8BPP	= 0x0008,	//	8 bpp
					//		    HIGH_COLOR_15BPP	= 0x000F,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
					//		    HIGH_COLOR_16BPP	= 0x0010,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
					//		    HIGH_COLOR_24BPP	= 0x0018	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)

	USHORT supportedColorDepths;	//	(2 bytes):  A 16-bit, unsigned integer. Specifies the high color depths that the client is capable of supporting.
					//		    If this field is present, all of the preceding fields MUST also be present. If this field is not present, all of the subsequent fields MUST NOT be present.
					//		    RNS_UD_24BPP_SUPPORT	= 0x0001,	//	24-bit RGB mask (8 bits for red, 8 bits for green, and 8 bits for blue)
					//		    RNS_UD_16BPP_SUPPORT	= 0x0002,	//	16-bit 565 RGB mask (5 bits for red, 6 bits for green, and 5 bits for blue)
					//		    RNS_UD_15BPP_SUPPORT	= 0x0004,	//	15-bit 555 RGB mask (5 bits for red, 5 bits for green, and 5 bits for blue)
					//		    RNS_UD_32BPP_SUPPORT	= 0x0008	//	32-bit RGB mask (8 bits for the alpha channel, 8 bits for red, 8 bits for green, and 8 bits for blue)

	USHORT earlyCapabilityFlags;	//	(2 bytes):  A 16-bit, unsigned integer. It specifies capabilities early in the connection sequence.
					//		    RNS_UD_CS_SUPPORT_ERRINFO_PDU	= 0x0001,	//	Indicates that the client supports the Set Error Info PDU (section 2.2.5.1).
					//		    RNS_UD_CS_WANT_32BPP_SESSION	= 0x0002,	//	Indicates that the client is requesting a session color depth of 32 bpp. This flag is necessary because the highColorDepth field does not support a value of 32. If this flag is set, the highColorDepth field SHOULD be set to 24 to provide an acceptable fallback for the scenario where the server does not support 32 bpp color.
					//		    RNS_UD_CS_SUPPORT_STATUSINFO_PDU	= 0x0004,	//	Indicates that the client supports the Server Status Info PDU (section 2.2.5.2).
					//		    RNS_UD_CS_STRONG_ASYMMETRIC_KEYS	= 0x0008,	//	Indicates that the client supports asymmetric keys larger than 512 bits for use with the Server Certificate (section 2.2.1.4.3.1) sent in the Server Security Data block (section 2.2.1.4.3).
					//		    RNS_UD_CS_VALID_CONNECTION_TYPE	= 0x0020,	//	Indicates that the connectionType field contains valid data.
					//		    RNS_UD_CS_SUPPORT_MONITOR_LAYOUT_PDU= 0x0040	//	Indicates that the client supports the Monitor Layout PDU (section 2.2.12.1).

	BYTE clientDigProductId[64];	//	(64 bytes): Contains a value that uniquely identifies the client.
					//		    If this field is present, all of the preceding fields MUST also be present. If this field is not present, all of the subsequent fields MUST NOT be present.

	BYTE connectionType;		//	(1 byte):   An 8-bit unsigned integer.
					//		    Hints at the type of network connection being used by the client.
					//		    This field only contains valid data if the RNS_UD_CS_VALID_CONNECTION_TYPE (0x0020) flag is present in the earlyCapabilityFlags field.
					//		    If this field is present, all of the preceding fields MUST also be present. If this field is not present, all of the subsequent fields MUST NOT be present.
					//		    CONNECTION_TYPE_MODEM		= 0x01,		//	Modem (56 Kbps)
					//		    CONNECTION_TYPE_BROADBAND_LOW	= 0x02,		//	Low-speed broadband (256 Kbps - 2 Mbps)
					//		    CONNECTION_TYPE_SATELLITE		= 0x03,		//	Satellite (2 Mbps - 16 Mbps with high latency)
					//		    CONNECTION_TYPE_BROADBAND_HIGH	= 0x04,		//	High-speed broadband (2 Mbps - 10 Mbps)
					//		    CONNECTION_TYPE_WAN			= 0x05,		//	WAN (10 Mbps or higher with high latency)
					//		    CONNECTION_TYPE_LAN			= 0x06		//	LAN (10 Mbps or higher)

	BYTE pad1octet;			//	(1 byte):   An 8-bit, unsigned integer.
					//		    Padding to align the serverSelectedProtocol field on the correct byte boundary.
					//		    If this field is present, all of the preceding fields MUST also be present. If this field is not present, all of the subsequent fields MUST NOT be present.

	UINT serverSelectedProtocol;	//	(4 bytes):  A 32-bit, unsigned integer that contains the value returned by the server in the selectedProtocol field of the RDP Negotiation Response (section 2.2.1.2.1).
					//		    In the event that an RDP Negotiation Response was not received from the server, this field MUST be initialized to PROTOCOL_RDP (0). This field MUST be present if an RDP Negotiation Request (section 2.2.1.1.1) was sent to the server. If this field is present, then all of the preceding fields MUST also be present.

}TS_UD_CS_CORE, *LPTS_UD_CS_CORE;


//	CS_CLUSTER
typedef struct tagTS_UD_CS_CLUSTER
{
	TS_UD_HEADER header;		//	(4 bytes):  GCC user data block header as described in User Data Header (section 2.2.1.3.1).
					//		    The User Data Header type field MUST be set to CS_SECURITY (0xC004).
	UINT Flags;			//	TS_UD_CS_CLUSTER::Flags = 0x0d
					//	0x0d
					//	= 0x03 << 2 | 0x01
					//	= REDIRECTION_VERSION4 << 2 | REDIRECTION_SUPPORTED
	UINT RedirectedSessionID;

}TS_UD_CS_CLUSTER, *LPTS_UD_CS_CLUSTER;

//	Client Security Data
typedef struct tagTS_UD_CS_SEC
{
	TS_UD_HEADER header;		//	(4 bytes):  GCC user data block header as described in User Data Header (section 2.2.1.3.1).
					//		    The User Data Header type field MUST be set to CS_SECURITY (0xC002).
	UINT encryptionMethods;		//	(4 bytes):  A 32-bit, unsigned integer.
					//		    Cryptographic encryption methods supported by the client and used in conjunction with Standard RDP Security The server MUST select one of these methods.
					//		    Section 5.3.2 describes how the client and server negotiate the security parameters for a given connection.
					//		    E_40BIT_ENCRYPTION_FLAG	= 0x00000001,	//	40-bit session keys MUST be used to encrypt data (with RC4) and generate Message Authentication Codes (MAC).
					//		    E_128BIT_ENCRYPTION_FLAG	= 0x00000002,	//	128-bit session keys MUST be used to encrypt data (with RC4) and generate MACs.
					//		    E_56BIT_ENCRYPTION_FLAG	= 0x00000008,	//	56-bit session keys MUST be used to encrypt data (with RC4) and generate MACs.
					//		    E_FIPS_ENCRYPTION_FLAG	= 0x00000010	//	All encryption and Message Authentication Code generation routines MUST be Federal Information Processing Standard (FIPS) 140-1 compliant.

	UINT extEncryptionMethods;	//	(4 bytes):  A 32-bit, unsigned integer.
					//		    This field is used exclusively for the French locale.
					//		    In French locale clients, encryptionMethods MUST be set to 0 and extEncryptionMethods MUST be set to the value to which encryptionMethods would have been set.
					//		    For non-French locale clients, this field MUST be set to 0.

}TS_UD_CS_SEC, *LPTS_UD_CS_SEC;

//	TS_UD_CS_NET
typedef struct tagTS_UD_CS_NET
{
	TS_UD_HEADER header;		//	(4 bytes):  GCC user data block header as described in User Data Header (section 2.2.1.3.1).
					//		    The User Data Header type field MUST be set to CS_SECURITY (0xC003).
	UINT channelCount;		//	(4 bytes):  A 32-bit, unsigned integer. The number of requested static virtual channels (the maximum allowed is 31).
	
	BYTE * channelDefArray;		//	(variable): A variable-length array containing the information for 
					//		    requested static virtual channels encapsulated in CHANNEL_DEF structures (section 2.2.1.3.4.1).
					//		    The number of CHANNEL_DEF structures which follows is given by the channelCount field.

}TS_UD_CS_NET, *LPTS_UD_CS_NET;




//	tpktHeader
typedef struct tagTpktHeader
{
	BYTE btVersion;			//	(1 bytes):  version
	BYTE btReserved;		//	(1 bytes):  reserved
	USHORT uLength;			//	(2 bytes):  length of full packet 整个包的长度

}STTPKTHEADER, *LPSTTPKTHEADER;

//	x224Data
typedef struct tagX224Data
{
	BYTE btLengthIndicator;		//	(1 bytes):  Length indicator = 2
	BYTE btType;			//	(1 bytes):  Type = 0xf0 = Data TPDU
	BYTE btEOT;			//	(1 bytes):  EOT

}STX224DATA, *LPSTX224DATA;


//
//	for "BER: Type Length"
//
typedef struct tagBERTypeLengthInfo
{
	BYTE  btInfo;		//	第一字节的 INFO
	UINT  uByteNumber;	//	指后面用几个字节表示整个 BER 包的长度
	ULONG ulValue;		//	长度值

}STBERTYPELENGTHINFO, *LPSTBERTYPELENGTHINFO;
static BOOLEAN rdpstruct_get_ber_length( BYTE * pbtBuffer, STBERTYPELENGTHINFO * pstInfo )
{
	//
	//	BER: Type Length = 404 bytes
	//	82 01 94
	//	This is the BER encoded definite long variant of the ASN.1 length field. The long variant layout is constructed as follows:
	//
	//	  7 6 5 4 3 2 1 0     7 6 5 4 3 2 1 0         7 6 5 4 3 2 1 0
	//	+-----------------+ +-----------------+     +-----------------+
	//	| 1 (0 < n < 127) | | L L L L L L L L | ... | L L L L L L L L |
	//	+-----------------+ +-----------------+     +-----------------+
	//	         1                   2                     n + 1
	//
	//	Since the most significant bit of the first byte (0x82) is set,
	//	the low seven bits contain the number of length bytes, which means that the number of length bytes is 2.
	//	Hence 0x01 and 0x94 are length bytes,
	//	which indicates that the length is greater than 256 bytes and less than 65536 bytes, specifically 0x194 (404) bytes.
	//
	BOOLEAN bRet;
	BYTE    btValue;

	if ( NULL == pbtBuffer || NULL == pstInfo )
	{
		return FALSE;
	}

	bRet = FALSE;
	pstInfo->btInfo		= pbtBuffer[ 0 ];
	pstInfo->uByteNumber	= 0;
	pstInfo->ulValue	= 0;

	if ( 1 == ( pstInfo->btInfo >> 7 ) )
	{
		//	第一位是 1，后 7 位是指后面用几个字节表示整个 BER 包的长度
		pstInfo->uByteNumber = ( ( 1 << pstInfo->btInfo ) >> 1 );
		if ( 1 == pstInfo->uByteNumber )
		{
			btValue = (BYTE)( pbtBuffer + 1 );
			pstInfo->ulValue = btValue;
		}
		else if ( 2 == pstInfo->uByteNumber )
		{
			pstInfo->ulValue = MAKEWORD( pbtBuffer[ 2 ], pbtBuffer[ 1 ] );
		}
		else if ( 4 == pstInfo->uByteNumber )
		{
			pstInfo->ulValue = MAKELONG( MAKEWORD( pbtBuffer[ 4 ], pbtBuffer[ 3 ] ), MAKEWORD( pbtBuffer[ 2 ], pbtBuffer[ 1 ] ) );
		}

		bRet = TRUE;
	}

	return bRet;
}

//
//	for "ASN.1 BER encoded Integer type"
//
typedef struct tagBEREncodedIntegerTypeInfo
{
	BYTE  btType;		//	0x02 = ASN.1 BER encoded Integer type
	UINT  uByteNumber;	//	指后面用几个字节表示整个 BER 包的长度
	ULONG ulValue;		//	值

}STBERENCODEDINTEGERTYPEINFO, *LPSTBERENCODEDINTEGERTYPEINFO;
static BOOLEAN rdpstruct_get_ber_encoded_integer_type( BYTE * pbtBuffer, STBERENCODEDINTEGERTYPEINFO * pstInfo )
{
	//
	//	0x02 0x01 0x22
	//	The first byte (0x02) is the ASN.1 BER encoded Integer type.
	//	The length of the integer is given by the second byte (1 byte), and the actual value is 34 (0x22).
	//

	BOOLEAN bRet;
	BYTE    btValue;

	if ( NULL == pbtBuffer || NULL == pstInfo )
	{
		return FALSE;
	}

	bRet = FALSE;
	pstInfo->btType		= pbtBuffer[ 0 ];	//	The first byte (0x02) is the ASN.1 BER encoded Integer type.
	pstInfo->uByteNumber	= pbtBuffer[ 1 ];	//	The length of the integer is given by the second byte (1 byte)
	pstInfo->ulValue	= 0;

	//	type 必须是 0x02
	if ( 0x02 == pstInfo->btType )
	{
		if ( 1 == pstInfo->uByteNumber )
		{
			btValue = (BYTE)( pbtBuffer + 2 );
			pstInfo->ulValue = btValue;
		}
		else if ( 2 == pstInfo->uByteNumber )
		{
			pstInfo->ulValue = MAKEWORD( pbtBuffer[ 3 ], pbtBuffer[ 2 ] );
		}
		else if ( 4 == pstInfo->uByteNumber )
		{
			pstInfo->ulValue = MAKELONG( MAKEWORD( pbtBuffer[ 5 ], pbtBuffer[ 4 ] ), MAKEWORD( pbtBuffer[ 3 ], pbtBuffer[ 2 ] ) );
		}

		bRet = TRUE;
	}

	return bRet;
}

//
//	for "UserData::value length"
//
typedef struct tagUserDataValueLengthInfo
{
	BYTE   btInfo;		//	第一字节的 INFO
	USHORT utValue;		//	长度值

}STUSERDATAVALUELENGTHINFO, *LPSTUSERDATAVALUELENGTHINFO;
static BOOLEAN rdpstruct_get_userdata_value_length( BYTE * pbtBuffer, STUSERDATAVALUELENGTHINFO * pstInfo )
{
	//
	//	81 1c -> UserData::value length = 284 bytes
	//	Since the most significant bit of the first byte (0x81) is set to 1 and the following bit is set to 0,
	//	the length is given by the low six bits of the first byte and the second byte.
	//	Hence, the value is 0x11c, which is 284 bytes.
	//

	BOOLEAN bRet;
	BYTE    btFlagBit1;
	BYTE    btFlagBit2;
	BYTE    btArrTemp[ 2 ];
	BYTE    btValue;

	if ( NULL == pbtBuffer || NULL == pstInfo )
	{
		return FALSE;
	}

	bRet = FALSE;
	pstInfo->btInfo		= pbtBuffer[ 0 ];
	pstInfo->utValue	= 0;

	//	要求第一个字节：第一位必须是1，第二位是0
	btValue		= pstInfo->btInfo;
	btFlagBit1	= ( btValue >> 7 );

	btValue		= pstInfo->btInfo;
	btFlagBit2	= ( ( (BYTE)( btValue << 1 ) ) >> 7 );

	if ( 1 == btFlagBit1 && 0 == btFlagBit2 )
	{
		btValue		= pbtBuffer[ 0 ];
		btArrTemp[ 0 ]	= ( ( (BYTE)( btValue << 2 ) ) >> 2 );
		btArrTemp[ 1 ]	= pbtBuffer[ 1 ];

		pstInfo->utValue = MAKEWORD( pbtBuffer[ 1 ], btArrTemp[ 0 ] );

		bRet = TRUE;
	}

	return bRet;
}






/*
//	Client MCS Connect Initial PDU with GCC Conference Create Request
typedef struct tagMcsCiPDUGcc
{
	STTPKTHEADER stTpktHeader;
	STX224DATA stX224Data;

	BYTE btApplicationType_0x7f;		//	(1 bytes): BER: Application-Defined Type = APPLICATION 101 = Connect-Initial
	BYTE btApplicationType_0x65;		//	(1 bytes): BER: Application-Defined Type = APPLICATION 101 = Connect-Initial 0x65 = MCS_TYPE_CONNECTINITIAL

	BYTE btBER1LengthType_0x82;		//	(1 bytes): = 0x82，第一位是 1；后 7 位表示后面表示长度的有几字节；2 正好表示有字节，这里一般就是 2 位，为了简单起见就解析2字节的
	USHORT uBER1Length;			//	(2 bytes):

	BYTE callingDomainSelector[ 3 ];	//	(3 bytes): 04 01 01
	BYTE calledDomainSelector[ 3 ];		//	(3 bytes): 04 01 01
	BYTE upwardFlag[ 3 ];			//	(3 bytes):

	BYTE targetParameters[ 2 ];		//	(2 bytes): 
	
	BYTE DomainParameters_maxChannelIds[ 3 ];	//	(3 bytes): 
	BYTE DomainParameters_maxUserIds[ 3 ];
	BYTE DomainParameters_maxTokenIds[ 3 ];
	BYTE DomainParameters_numPriorities[ 3 ];
	BYTE DomainParameters_minThroughput[ 3 ];
	BYTE DomainParameters_maxHeight[ 3 ];
	BYTE DomainParameters_maxMCSPDUsize[ 4 ];
	BYTE DomainParameters_protocolVersion[ 3 ];
	
	BYTE minimumParameters[ 2 ];
	
	BYTE DomainParameters_maxChannelIds[ 3 ];
	BYTE DomainParameters_maxUserIds[ 3 ];
	BYTE DomainParameters_maxTokenIds[ 3 ];
	BYTE DomainParameters_numPriorities[ 3 ];
	BYTE DomainParameters_minThroughput[ 3 ];
	BYTE DomainParameters_maxHeight[ 3 ];
	BYTE DomainParameters_maxMCSPDUsize[ 4 ];
	BYTE DomainParameters_protocolVersion[ 3 ];

	BYTE maximumParameters[ 2 ];

	BYTE DomainParameters_maxChannelIds[ 4 ];
	BYTE DomainParameters_maxUserIds[ 4 ];
	BYTE DomainParameters_maxTokenIds[ 4 ];
	BYTE DomainParameters_numPriorities[ 3 ];
	BYTE DomainParameters_minThroughput[ 3 ];
	BYTE DomainParameters_maxHeight[ 3 ];
	BYTE DomainParameters_maxMCSPDUsize[ 4 ];
	BYTE DomainParameters_protocolVersion[ 3 ];
	
	BYTE userData1_0x04;			//	(1 bytes): 
	BYTE userData1_0x82;			//	(1 bytes): 
	USHORT userData_Length;			//	(2 bytes): 


}STMCSCIPDUGCC, *LPSTMCSCIPDUGCC;
*/



#pragma pack()







#endif	// __RDPSTRUCT_HEADER__