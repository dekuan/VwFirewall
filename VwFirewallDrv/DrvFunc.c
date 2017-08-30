// DrvFunc.cpp: implementation of the ProcPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "DrvFunc.h"


/**
 *	根据驱动器名，获取 device name
 */
BOOLEAN drvfunc_query_device_name( IN OUT LPSTDRIVERINFO pstDriverInfo )
{
	//
	//	INPUT:
	//		pstDriverInfo->uszDosDriverLetter	"C:"
	//
	//	OUTPUT:
	//		pstDriverInfo->wszDosFullDriverLetter	L"\\??\\C:"
	//		pstDriverInfo->uszDeviceName		"\\Device\\HarddiskVolume1"
	//		pstDriverInfo->wszDeviceName		L"\\Device\\HarddiskVolume1"
	//

	BOOLEAN bRet;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL; 
	HANDLE hLink;
	UNICODE_STRING usSymbolLink;
	UNICODE_STRING usLinkName;
	OBJECT_ATTRIBUTES attributes;
	PWCHAR pwsSymbolLink;
	ULONG nReturnLen;


	if ( NULL == pstDriverInfo )
	{
		return FALSE;
	}
	if ( 0 == pstDriverInfo->uszDosDriverLetter[0] )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		pwsSymbolLink = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_PATH );
		if ( pwsSymbolLink )
		{
			RtlZeroMemory( pwsSymbolLink, MAX_PATH );
			RtlInitEmptyUnicodeString( &usSymbolLink, pwsSymbolLink, MAX_PATH );
			usSymbolLink.Length	= 0;

			wcscpy( pstDriverInfo->wszDosFullDriverLetter, L"\\??\\A:" );
			ntStatus = drvfunc_c2w( pstDriverInfo->uszDosDriverLetter, pstDriverInfo->wszDosFullDriverLetter + 4, sizeof(pstDriverInfo->wszDosFullDriverLetter) - 4 * sizeof(WCHAR) );
			if ( NT_SUCCESS( ntStatus ) )
			{
				RtlInitUnicodeString( &usLinkName, pstDriverInfo->wszDosFullDriverLetter );
				InitializeObjectAttributes( &attributes, &usLinkName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
				hLink	 = NULL;
				ntStatus = ZwOpenSymbolicLinkObject( &hLink, GENERIC_READ, &attributes );
				if ( NT_SUCCESS( ntStatus ) )
				{
					nReturnLen = 0;
					ntStatus = ZwQuerySymbolicLinkObject( hLink, &usSymbolLink, &nReturnLen );
					if ( NT_SUCCESS( ntStatus ) )
					{
						if ( usSymbolLink.Length < sizeof(pstDriverInfo->wszDeviceName) &&
							nReturnLen < sizeof(pstDriverInfo->wszDeviceName) )
						{
							ntStatus = drvfunc_w2c_ex( usSymbolLink.Buffer, (USHORT)(usSymbolLink.Length), pstDriverInfo->uszDeviceName, sizeof(pstDriverInfo->uszDeviceName) );
							if ( NT_SUCCESS( ntStatus ) )
							{
								bRet = TRUE;
								RtlMoveMemory( pstDriverInfo->wszDeviceName, usSymbolLink.Buffer, usSymbolLink.Length );
							}
						}
					}
					ZwClose( hLink );
					hLink = NULL;
				}
			}

			ExFreePool( pwsSymbolLink );
			pwsSymbolLink = NULL;
		}
		else
		{
			//	...
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_dos_device_name_by_driver_letter"));
	}

	return bRet;
}

/**
 *	取得当前系统的 Windows 目录
 */
BOOLEAN drvfunc_get_windowsdir( IN PDRIVER_OBJECT DriverObject, OUT PWCHAR pwszWindowDir, IN USHORT uSize )
{
	BOOLEAN bRet;
	HANDLE hFileHandle;
	PWCHAR pwszBuffer;
	WCHAR  wcTemp;
	USHORT uLen;
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	PFILE_NAME_INFORMATION pstNameInfo;
	PFILE_OBJECT pstFileObject;
	POBJECT_NAME_INFORMATION pstObjectNameInfo;


	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}

	//	...
	bRet	= FALSE;

	__try
	{
		//	打开的文件名是 "\SystemRoot"
		RtlInitUnicodeString( &usFileName, L"\\SystemRoot" );
		InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );

		//	Callers of ZwCreateFile must be running at IRQL = PASSIVE_LEVEL.		
		ntStatus = ZwCreateFile
			(
				&hFileHandle, 
				(SYNCHRONIZE | FILE_READ_ATTRIBUTES), 
				&ObjectAttributes, &IoStatus, 
				NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN,
				FILE_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0
			);
		if ( NT_SUCCESS( ntStatus ) && NT_SUCCESS( IoStatus.Status ) )
		{
			ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
			if ( NT_SUCCESS( ntStatus ) )
			{
				pstObjectNameInfo	= NULL;
				ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
				if ( NT_SUCCESS( ntStatus ) )
				{
					wcTemp = pstObjectNameInfo->Name.Buffer[ pstObjectNameInfo->Name.Length - sizeof(WCHAR) ];
					if ( L'\\' != wcTemp && L'/' != wcTemp )
					{
						_snwprintf( pwszWindowDir, uSize/sizeof(WCHAR)-1, L"\\??\\%s\\", pstObjectNameInfo->Name.Buffer );
					}
					else
					{
						_snwprintf( pwszWindowDir, uSize/sizeof(WCHAR)-1, L"\\??\\%s", pstObjectNameInfo->Name.Buffer );
					}

					//	..
					bRet = TRUE;
				}
			}

			ZwClose( hFileHandle );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_windowsdir"));
	}

	return bRet;
}


/**
 *	获取当前系统本地时间
 */
NTSTATUS drvfunc_get_localtime( PCHAR pszCurrentTime, ULONG uSize )
{
	NTSTATUS ntStatus;
	LARGE_INTEGER lnSystemTime;
	LARGE_INTEGER lnLocalTime;
	TIME_FIELDS tfTimeFields;

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		if ( pszCurrentTime )
		{
			//
			//	Callers of KeQuerySystemTime can be running at any IRQL. 
			//	Callers of ExSystemTimeToLocalTime can be running at any IRQL.
			//	Callers of RtlTimeToTimeFields can be running at any IRQL if both input buffers are resident.
			//
			KeQuerySystemTime( &lnSystemTime );
			ExSystemTimeToLocalTime( &lnSystemTime, &lnLocalTime );
			RtlTimeToTimeFields( &lnLocalTime, &tfTimeFields );
		
			_snprintf( pszCurrentTime, uSize/sizeof(char)-1, "%d-%02d-%02d %02d:%02d:%02d",
				tfTimeFields.Year, tfTimeFields.Month, tfTimeFields.Day,
				tfTimeFields.Hour, tfTimeFields.Minute, tfTimeFields.Second );
			// ..
			ntStatus = STATUS_SUCCESS;
			KdPrint(("MyGetLocalTime: Current time is %s", pszCurrentTime ));
		}
		else
		{
			KdPrint(("MyGetLocalTime: pszCurrentTime is a NULL point"));
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_localtime"));
	}

	return ntStatus;
}


/////////////////////////////////////////////////////////////////
//  类别:
//         文件系统操作
//  功能:
//         将Unicode 字符串转换为Char字符串
//	   暂时考虑不用
//  参数：
//         
//  返回值：
//         STATUS_SUCCESS	成功
//	########## 废弃 2010-04-15
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_w2a( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS ntStatus;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;

	if ( NULL == pSourceString || NULL == pDestString || 0 == nBufferLen )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		usSource.Buffer = pSourceString;
		usSource.Length = wcslen(pSourceString)*sizeof(WCHAR);
		
		ntStatus = RtlUnicodeStringToAnsiString( &asDest, &usSource, TRUE );
		if ( NT_SUCCESS(ntStatus) )
		{
			if ( asDest.Length < nBufferLen )
			{
				RtlCopyMemory( pDestString, asDest.Buffer, asDest.Length );
				pDestString[ asDest.Length ] = '\0';
			}
			else
			{
				pDestString[0] = '\0';
			}

			RtlFreeAnsiString( &asDest );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_w2a"));
	}

	return ntStatus;
}

/////////////////////////////////////////////////////////////////
//  类别:
//         文件系统操作
//  功能:
//         将 WCHAR 字符串转换为 CHAR 字符串
//  参数：
//         
//  返回值：
//         STATUS_SUCCESS	成功
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_w2c( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS ntStatus;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;

	if ( NULL == pSourceString || NULL == pDestString || 0 == nBufferLen )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		usSource.Buffer		= pSourceString;
		usSource.Length		= wcslen(pSourceString)*sizeof(WCHAR);
		
		asDest.Length		= 0;
		asDest.Buffer		= pDestString;
		asDest.MaximumLength	= nBufferLen ;
		pDestString[0]		= '\0';
		
		ntStatus = RtlUnicodeStringToAnsiString( &asDest, &usSource, FALSE );
		if ( NT_SUCCESS(ntStatus) )
		{
			pDestString[ asDest.Length ] = '\0';
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_w2c"));
	}
	
	return ntStatus;
}
NTSTATUS drvfunc_w2c_ex( IN PWCHAR pwszSourceString, IN USHORT nSourceStringLength, OUT PCHAR pszDestString, IN USHORT nDestBufferLength )
{
	//
	//	pwszSourceString	- [in]  源字符串
	//	nSourceStringLength	- [in]  源字符串长度，以 byte 为单位
	//	pszDestString		- [out] 输出字符串
	//	nDestBufferLength	- [in]  输出字符串长度，以 byte 为单位
	//	RETURN			- NTSTATUS
	//

	NTSTATUS ntStatus;
	UNICODE_STRING usSource;
	ANSI_STRING asDest;

	if ( NULL == pwszSourceString || 0 == nSourceStringLength || NULL == pszDestString || 0 == nDestBufferLength )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		usSource.Buffer		= pwszSourceString;
		usSource.Length		= nSourceStringLength;
		
		asDest.Length		= 0;
		asDest.Buffer		= pszDestString;
		asDest.MaximumLength	= nDestBufferLength ;
		pszDestString[0]	= '\0';

		ntStatus = RtlUnicodeStringToAnsiString( &asDest, &usSource, FALSE );
		if ( NT_SUCCESS(ntStatus) )
		{
			pszDestString[ asDest.Length ] = '\0';
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_w2c_ex"));
	}

	return ntStatus;
}

/////////////////////////////////////////////////////////////////
//  类别:
//         文件系统操作
//  功能:
//         将 UNICODE_STRING 字符串转换为 CHAR 字符串
//  参数：
//         
//  返回值：
//         STATUS_SUCCESS	成功
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_u2c( IN PUNICODE_STRING pusSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen )
{
	NTSTATUS ntStatus;
	ANSI_STRING asDest;

	if ( NULL == pusSourceString || NULL == pDestString || 0 == nBufferLen )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		//	目标字符串
		asDest.Length		= 0;
		asDest.Buffer		= pDestString;
		asDest.MaximumLength	= nBufferLen ;
		pDestString[0]		= '\0';

		ntStatus = RtlUnicodeStringToAnsiString( &asDest, pusSourceString, FALSE );
		if ( NT_SUCCESS(ntStatus) )
		{
			pDestString[ asDest.Length ] = '\0';
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_u2c"));
	}

	return ntStatus;
}

/////////////////////////////////////////////////////////////////
//  类别:
//         文件系统操作
//  功能:
//         将Char 字符串转换为Unicode字符串
//  参数：
//         
//  返回值：
//         STATUS_SUCCESS	成功
/////////////////////////////////////////////////////////////////
NTSTATUS drvfunc_c2w( IN PCHAR pSourceString, OUT PWCHAR pDestString, IN USHORT nDestBufferLenInBytes )
{
	//
	//	pSourceString
	//	pDestString
	//	nBufferLen	- The maximum length in bytes of Buffer.
	//
	NTSTATUS ntStatus;
	ANSI_STRING asSource;
	UNICODE_STRING usDest;

	if ( NULL == pSourceString || NULL == pDestString || 0 == nDestBufferLenInBytes )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		asSource.Buffer		= pSourceString;
		asSource.Length		= (USHORT)strlen(pSourceString);

		usDest.Length		= 0;
		usDest.Buffer		= pDestString;
		usDest.MaximumLength	= nDestBufferLenInBytes ;
		pDestString[0]		= L'\0';

		ntStatus = RtlAnsiStringToUnicodeString( &usDest, &asSource, FALSE );
		if ( NT_SUCCESS(ntStatus) )
		{
			pDestString[ usDest.Length/2 ] = L'\0';
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_c2w"));
	}

	return ntStatus;
}
NTSTATUS drvfunc_c2w_ex( IN PCHAR pSourceString, IN USHORT nSourceStringLength, OUT PWCHAR pDestString, IN USHORT nDestBufferLenInBytes )
{
	//
	//	pSourceString
	//	pDestString
	//	nBufferLen	- The maximum length in bytes of Buffer.
	//
	NTSTATUS ntStatus;
	ANSI_STRING asSource;
	UNICODE_STRING usDest;
	
	if ( NULL == pSourceString || 0 == nSourceStringLength || NULL == pDestString || 0 == nDestBufferLenInBytes )
	{
		return STATUS_UNSUCCESSFUL;
	}
	
	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		asSource.Buffer		= pSourceString;
		asSource.Length		= nSourceStringLength;

		usDest.Length		= 0;
		usDest.Buffer		= pDestString;
		usDest.MaximumLength	= nDestBufferLenInBytes ;
		pDestString[0]		= L'\0';
		
		ntStatus = RtlAnsiStringToUnicodeString( &usDest, &asSource, FALSE );
		if ( NT_SUCCESS(ntStatus) )
		{
			pDestString[ usDest.Length/2 ] = L'\0';
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_c2w_ex"));
	}
	
	return ntStatus;
}

NTSTATUS drvfunc_a2u( IN PCHAR pSourceString, OUT PUNICODE_STRING pusDest )
{
	NTSTATUS ntStatus;
	ANSI_STRING asSource;

	if ( NULL == pSourceString || NULL == pusDest )
	{
		return STATUS_UNSUCCESSFUL;
	}

	//	...
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		asSource.Buffer = pSourceString;
		asSource.Length = (USHORT)strlen(pSourceString);

		ntStatus = RtlAnsiStringToUnicodeString( pusDest, &asSource, TRUE );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_a2u"));
	}

	return ntStatus;
}



//////////////////////////////////////////////////////////////////////////
//	for file


/**
 *	在一个路径的字符串中计算从头开始到最后一个 "/" or "\" 的位置
 *	从而得到目录的长度，其长度不包括最后一个 "\" 的长度
 *	input:  "C:\Windows\AppDIr\myfile.exe"
 *	output: the length of "C:\Windows\AppDIr"
 */
INT drvfunc_get_dir_length( UCHAR * lpuszFilePath, UINT uFilePathLength )
{
	INT nDirLen;
	
	if ( NULL == lpuszFilePath || 0 == uFilePathLength )
	{
		return 0;
	}

	//	..
	nDirLen = 0;

	__try
	{
		if ( drvfunc_is_dir_a( lpuszFilePath, uFilePathLength ) )
		{
			//	本身就是文件夹，不用再计算了
			nDirLen = uFilePathLength;
		}
		else
		{
			//
			//	找到文件全路径中，目录的结束点
			//
			for ( nDirLen = ( uFilePathLength - sizeof(CHAR) ); nDirLen >= 0; nDirLen -- )
			{
				if ( '\\' == lpuszFilePath[ nDirLen ] || '/' == lpuszFilePath[ nDirLen ] )
				{
					//
					//	加加后就是 Dir 的长度了
					//	长度不包括最后一个 "\" 的长度
					//
					break;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_dir_length"));
	}

	return nDirLen;
}
INT drvfunc_get_dir_length_w( WCHAR * lpwszFilePath, UINT uFilePathLength )
{
	//
	//	lpwszFilePath		- path
	//	uFilePathLength		- length of path in bytes
	//	RETURN			- length of dir in bytes
	//

	INT nRet;
	INT nDirLen;
	
	if ( NULL == lpwszFilePath || 0 == uFilePathLength )
	{
		return 0;
	}
	
	//	..
	nRet	= 0;
	
	__try
	{
		if ( drvfunc_is_dir_w( lpwszFilePath, uFilePathLength ) )
		{
			//	本身就是文件夹，不用再计算了
			//	in bytes
			nRet = uFilePathLength;
		}
		else
		{
			//
			//	找到文件全路径中，目录的结束点
			//
			nDirLen = ( uFilePathLength/sizeof(WCHAR) - 1 );
			for ( ; nDirLen >= 0; nDirLen -- )
			{
				if ( '\\' == lpwszFilePath[ nDirLen ] || '/' == lpwszFilePath[ nDirLen ] )
				{
					//
					//	加加后就是 Dir 的长度了
					//	长度不包括最后一个 "\" 的长度
					//
					nRet = nDirLen * sizeof(WCHAR);
					break;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_dir_length_w"));
	}
	
	return nRet;
}


/**
 *	判断是否是目录
 */
BOOLEAN  drvfunc_is_dir_a( UCHAR * lpuszFilePath, UINT uFilePathLength )
{
	BOOLEAN bRet;
	NTSTATUS ntStatus;
	PWCHAR pwszNewFilePath = NULL;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpuszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}
	if ( 0 == lpuszFilePath[0] )
	{
		return FALSE;
	}
	
	//	...
	bRet = FALSE;
	
	//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
	pwszNewFilePath = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
	if ( pwszNewFilePath )
	{
		RtlZeroMemory( pwszNewFilePath, MAX_WPATH );
		
		ntStatus = drvfunc_c2w_ex( lpuszFilePath, (USHORT)uFilePathLength, pwszNewFilePath, MAX_WPATH );
		if ( NT_SUCCESS( ntStatus ) )
		{
			bRet = drvfunc_is_dir_w( pwszNewFilePath, uFilePathLength );
		}

		ExFreePool( pwszNewFilePath );
		pwszNewFilePath = NULL;
	}

	return bRet;
}
BOOLEAN  drvfunc_is_dir_w( WCHAR * lpwszFilePath, UINT uFilePathLength )
{
	//
	//	lpwszFilePath	- 
	//	uFilePathLength	- length of lpwszFilePath in chars
	//
	BOOLEAN bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	FILE_STANDARD_INFORMATION StandardInfo;
	NTSTATUS ntStatus;
	BOOLEAN bMakeNewPathSucc;
	PWCHAR pwszNewFileName;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpwszFilePath || 0 == uFilePathLength )
	{
		return FALSE;
	}
	if ( 0 == lpwszFilePath[0] )
	{
		return FALSE;
	}
	
	//
	//	初始化返回值
	//
	bRet = FALSE;
	
	__try
	{
		bMakeNewPathSucc	= FALSE;
		pwszNewFileName		= NULL;

		pwszNewFileName = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
		if ( pwszNewFileName )
		{
			RtlZeroMemory( pwszNewFileName, MAX_WPATH );

			//	L"\\??\\C:\\DeFirewall.log"
			if ( 0 != wcsncmp( L"\\??\\", lpwszFilePath, 4 ) )
			{
				pwszNewFileName[ 0 ]	= L'\\';
				pwszNewFileName[ 1 ]	= L'?';
				pwszNewFileName[ 2 ]	= L'?';
				pwszNewFileName[ 3 ]	= L'\\';
				_snwprintf( pwszNewFileName + 4, min( uFilePathLength/sizeof(WCHAR), MAX_WPATH/sizeof(WCHAR) - 1 - 4 ), L"%s", lpwszFilePath );
				RtlInitUnicodeString( &usFileName, pwszNewFileName );

				bMakeNewPathSucc = TRUE;
			}
			else if ( uFilePathLength > 2 && L':' == pwszNewFileName[ 1 ] )
			{
				_snwprintf( pwszNewFileName, min( uFilePathLength/sizeof(WCHAR), MAX_WPATH/sizeof(WCHAR) - 1 ), L"%s", lpwszFilePath );
				RtlInitUnicodeString( &usFileName, pwszNewFileName );

				bMakeNewPathSucc = TRUE;
			}
		}

		if ( bMakeNewPathSucc )
		{
			InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );		
			ntStatus = ZwCreateFile
				(
					&hFileHandle,			// returned file handle
					FILE_READ_ATTRIBUTES,		// desired access
					&ObjectAttributes,		// ptr to object attributes
					&IoStatus,			// ptr to I/O status block
					NULL,				// alloc size = none
					FILE_ATTRIBUTE_NORMAL,
					FILE_SHARE_READ,
					FILE_OPEN,
					FILE_DIRECTORY_FILE,
					NULL,				// eabuffer
					0				// ealength
				);
			if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
			{
				ntStatus = ZwQueryInformationFile( hFileHandle, &IoStatus, &StandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
				if ( NT_SUCCESS( ntStatus ) )
				{
					bRet = StandardInfo.Directory;
				}

				ZwClose( hFileHandle );
			}
		}

		//
		//	free memory
		//
		if ( pwszNewFileName )
		{
			ExFreePool( pwszNewFileName );
			pwszNewFileName = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_is_dir_w"));
	}
	
	return bRet;
}

/**
 *	判断文件是否存在
 */
BOOLEAN drvfunc_is_dir_exist_a( UCHAR * pucszFilename )
{
	BOOLEAN bRet;
	NTSTATUS ntStatus;
	PWCHAR pwszNewFilename = NULL;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == pucszFilename )
	{
		return FALSE;
	}
	if ( 0 == pucszFilename[0] )
	{
		return FALSE;
	}
	
	//	...
	bRet = FALSE;
	
	//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
	pwszNewFilename = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
	if ( pwszNewFilename )
	{
		RtlZeroMemory( pwszNewFilename, MAX_WPATH );
		
		ntStatus = drvfunc_c2w( pucszFilename, pwszNewFilename, MAX_WPATH );
		if ( NT_SUCCESS( ntStatus ) )
		{
			bRet = drvfunc_is_dir_exist_w( pwszNewFilename );
		}
		
		ExFreePool( pwszNewFilename );
		pwszNewFilename = NULL;
	}
	
	return bRet;
}
BOOLEAN drvfunc_is_dir_exist_w( IN LPCWSTR lpcwszFilename )
{
	BOOLEAN bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_STANDARD_INFORMATION stStandardInfo;
	//WCHAR wszFilenameNew[ MAX_PATH ];
	PWCHAR pwszNewFileName;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpcwszFilename )
	{
		return FALSE;
	}
	if ( wcslen(lpcwszFilename) <= 4 )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		pwszNewFileName	= NULL;
		
		//
		//	L"\\??\\C:\\DeFirewall.log"
		//
		if ( 0 != wcsncmp( L"\\??\\", lpcwszFilename, 4 ) )
		{
			//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
			pwszNewFileName = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
			if ( pwszNewFileName )
			{
				//	Callers of RtlZeroMemory at Any level
				RtlZeroMemory( pwszNewFileName, MAX_WPATH );
				_snwprintf( pwszNewFileName, MAX_WPATH/sizeof(WCHAR)-1, L"\\??\\%s", lpcwszFilename );
				RtlInitUnicodeString( &usFileName, pwszNewFileName );
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			RtlInitUnicodeString( &usFileName, lpcwszFilename );
		}

		InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
		
		//	Callers of ZwCreateFile must be running at IRQL = PASSIVE_LEVEL and with APCs enabled.		
		ntStatus = ZwCreateFile
			(
				&hFileHandle,			// returned file handle
				FILE_LIST_DIRECTORY,		// desired access
				&ObjectAttributes,		// ptr to object attributes
				&IoStatus,			// ptr to I/O status block
				NULL,				// alloc size = none
				FILE_ATTRIBUTE_NORMAL,		// 有点疑问，如果失败可以尝试设置 NULL
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_DIRECTORY_FILE,		// 原来是 FILE_SYNCHRONOUS_IO_NONALERT 现在修改成 
				NULL,				// eabuffer
				0				// ealength
			);
		if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
		{
			bRet = TRUE;
			ZwClose( hFileHandle );
		}
		
		//
		//	free memory
		//
		if ( pwszNewFileName )
		{
			ExFreePool( pwszNewFileName );
			pwszNewFileName = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_is_dir_exist_w"));
	}
	
	return bRet;
}


/**
 *	检查文件是否存在
 */
BOOLEAN drvfunc_is_file_exist_a( UCHAR * pucszFilename )
{
	BOOLEAN bRet;
	NTSTATUS ntStatus;
	PWCHAR pwszNewFilename = NULL;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == pucszFilename )
	{
		return FALSE;
	}
	if ( 0 == pucszFilename[0] )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	//	Callers of ExAllocatePool must be executing at IRQL <= DISPATCH_LEVEL
	pwszNewFilename = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
	if ( pwszNewFilename )
	{
		RtlZeroMemory( pwszNewFilename, MAX_WPATH );
		
		ntStatus = drvfunc_c2w( pucszFilename, pwszNewFilename, MAX_WPATH );
		if ( NT_SUCCESS( ntStatus ) )
		{
			bRet = drvfunc_is_file_exist( pwszNewFilename );
		}

		ExFreePool( pwszNewFilename );
		pwszNewFilename = NULL;
	}

	return bRet;
}
BOOLEAN drvfunc_is_file_exist( LPCWSTR lpcwszFilename )
{
	BOOLEAN bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	PWCHAR pwszNewFileName;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpcwszFilename )
	{
		return FALSE;
	}
	if ( 0 == wcslen(lpcwszFilename) )
	{
		return FALSE;
	}
	
	//
	//	初始化返回值
	//
	bRet = FALSE;
	
	__try
	{
		pwszNewFileName	= NULL;

		//
		//	L"\\??\\C:\\DeFirewall.log"
		//
		if ( 0 != wcsncmp( L"\\??\\", lpcwszFilename, 4 ) )
		{
			pwszNewFileName = (PWCHAR)ExAllocatePool( NonPagedPool, MAX_WPATH );
			if ( pwszNewFileName )
			{
				RtlZeroMemory( pwszNewFileName, MAX_WPATH );
				_snwprintf( pwszNewFileName, MAX_WPATH/sizeof(WCHAR)-1, L"\\??\\%s", lpcwszFilename );
				RtlInitUnicodeString( &usFileName, pwszNewFileName );
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			RtlInitUnicodeString( &usFileName, lpcwszFilename );
		}

		InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );		
		ntStatus = ZwCreateFile
			(
				&hFileHandle,			// returned file handle
				SYNCHRONIZE | FILE_READ_DATA,	// desired access
				&ObjectAttributes,		// ptr to object attributes
				&IoStatus,			// ptr to I/O status block
				NULL,				// alloc size = none
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,				// eabuffer
				0				// ealength
			);
		if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
		{
			bRet = TRUE;
			ZwClose( hFileHandle );
		}

		//
		//	free memory
		//
		if ( pwszNewFileName )
		{
			ExFreePool( pwszNewFileName );
			pwszNewFileName = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_is_file_exist"));
	}
	
	return bRet;
}

/**
 *	获取文件内容的长度
 */
DWORD drvfunc_get_filesize( LPCWSTR lpcwszFilename )
{
	DWORD dwRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_STANDARD_INFORMATION stStandardInfo;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return 0;
	}
	if ( NULL == lpcwszFilename )
	{
		return 0;
	}
	if ( 0 == wcslen(lpcwszFilename) )
	{
		return 0;
	}
	
	//
	//	初始化返回值
	//
	dwRet = 0;

	__try
	{
		RtlInitUnicodeString( &usFileName, lpcwszFilename );
		InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
		
		ntStatus = ZwCreateFile
			(
				&hFileHandle,			// returned file handle
				SYNCHRONIZE | FILE_READ_DATA,	// desired access
				&ObjectAttributes,		// ptr to object attributes
				&IoStatus,			// ptr to I/O status block
				NULL,				// alloc size = none
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,				// eabuffer
				0				// ealength
			);
		if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
		{
			ntStatus = ZwQueryInformationFile( hFileHandle, &IoStatus, &stStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
			if ( NT_SUCCESS( ntStatus ) )
			{
				dwRet = stStandardInfo.EndOfFile.u.LowPart;
			}
			
			ZwClose( hFileHandle );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_filesize"));
	}
	
	return dwRet;
}


/**
 *	获取文件内容
 */
BOOLEAN drvfunc_get_filecontent( IN LPCWSTR lpcwszFilename, OUT LPSTR lpszContent, IN DWORD dwSize )
{
	BOOLEAN bRet;
	HANDLE hFileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS ntStatus;
	FILE_POSITION_INFORMATION stFilePos;
	FILE_STANDARD_INFORMATION stStandardInfo;
	
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpcwszFilename )
	{
		return FALSE;
	}
	if ( 0 == wcslen(lpcwszFilename) )
	{
		return FALSE;
	}
	if ( NULL == lpszContent || 0 == dwSize )
	{
		return FALSE;
	}
	
	
	//
	//	初始化返回值
	//
	bRet	= FALSE;
	
	__try
	{
		RtlInitUnicodeString( &usFileName, lpcwszFilename );
		InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
		
		ntStatus = ZwCreateFile
			(
				&hFileHandle,			// returned file handle
				SYNCHRONIZE | FILE_READ_DATA,	// desired access
				&ObjectAttributes,		// ptr to object attributes
				&IoStatus,			// ptr to I/O status block
				NULL,				// alloc size = none
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,				// eabuffer
				0				// ealength
			);
		if ( NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatus.Status) )
		{
			ntStatus = ZwQueryInformationFile( hFileHandle, &IoStatus, &stStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );
			if ( NT_SUCCESS( ntStatus ) && stStandardInfo.EndOfFile.QuadPart > 0 )
			{
				ntStatus = ZwReadFile
					(
						hFileHandle, NULL, NULL, NULL,
						&IoStatus,
						lpszContent,
						( dwSize - sizeof(CHAR) ),
						NULL, NULL
					);
				if ( NT_SUCCESS(ntStatus) && IoStatus.Information == ( dwSize - sizeof(CHAR) ) )
				{
					bRet = TRUE;
				}
			}
			else
			{
				KdPrint( ("procconfig_load_protected_dir: cann't get file size,status=0x%4x\n", ntStatus) );
			}
			
			ZwClose( hFileHandle );
		}
		else
		{
			KdPrint( ("procconfig_load_protected_dir: fail to open file, status=0x%4x\n", ntStatus) );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_filecontent"));
	}
	
	return bRet;
}

BOOLEAN drvfunc_save_filecontent( IN LPCWSTR lpcwszFilename, IN CONST PVOID lpvContent, IN DWORD dwSize, IN BOOLEAN bAppend )
{
	//
	//	lpcwszFilename	- IN，类似 L"\\??\\C:\\DeAntiHack.log"
	//	lpvContent	- IN 文件内容
	//	dwSize		- IN 文件内容大小
	//

	BOOLEAN bRet;
	NTSTATUS ntStatus;
	HANDLE FileHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING usFileName;
	IO_STATUS_BLOCK IoStatus;
	FILE_POSITION_INFORMATION stFilePos;
	FILE_STANDARD_INFORMATION stStandardInfo;
	ULONG nWriteBytes;

	//	...
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}
	if ( NULL == lpcwszFilename || NULL == lpvContent || 0 == dwSize )
	{
		return FALSE;
	}
	if ( 0 == wcslen( lpcwszFilename ) )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		//
		//	#define LOG_FILEPATH	L"\\??\\C:\\DeAntiHack.log"
		//
		RtlInitUnicodeString( &usFileName, lpcwszFilename );
		InitializeObjectAttributes
			(
				&ObjectAttributes,			// ptr to structure
				&usFileName,				// ptr to file spec
				OBJ_CASE_INSENSITIVE,	// attributes
				NULL,					// root directory handle
				NULL					// ptr to security descriptor
			);
		/*
		ntStatus = IoCreateFileSpecifyDeviceObjectHint
			(
				&FileHandle,
				GENERIC_READ | GENERIC_WRITE,
				&ObjectAttributes,
				&IoStatus,
				NULL,
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				FILE_OPEN,
				FILE_NON_DIRECTORY_FILE,
				NULL,
				0,
				CreateFileTypeNone,
				NULL,
				IO_IGNORE_SHARE_ACCESS_CHECK,
				NULL
			);
		*/

		ntStatus = ZwCreateFile
			(
				&FileHandle,				// returned file handle
				(SYNCHRONIZE | FILE_WRITE_DATA),		// desired access
				&ObjectAttributes,			// ptr to object attributes
				&IoStatus,				// ptr to I/O status block
				NULL,					// alloc size = none
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				FILE_OVERWRITE_IF,
				FILE_NON_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,					// eabuffer
				0					// ealength
			);
		if ( NT_SUCCESS( ntStatus ) && NT_SUCCESS( IoStatus.Status ) )
		{
			if ( bAppend )
			{
				//
				//	从文件尾部追加内容
				//
				ntStatus = ZwQueryInformationFile
					(
						FileHandle, &IoStatus, &stStandardInfo,
						sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation
					);
				if ( NT_SUCCESS( ntStatus ) )
				{
					stFilePos.CurrentByteOffset = stStandardInfo.EndOfFile;
					ntStatus = ZwSetInformationFile
						(
							FileHandle, &IoStatus, &stFilePos, 
							sizeof(FILE_POSITION_INFORMATION), FilePositionInformation
						);
					if ( NT_SUCCESS( ntStatus ) )
					{
						nWriteBytes	= dwSize;
						ntStatus	= ZwWriteFile
							(
								FileHandle, NULL, NULL, NULL,
								&IoStatus, lpvContent, nWriteBytes, NULL, NULL
							);
						if ( NT_SUCCESS( ntStatus ) && IoStatus.Information == nWriteBytes )
						{
							bRet = TRUE;
							//KdPrint(("drvfunc_save_filecontent: Write Data %s\n", pszData ));
						}
						else
						{
							KdPrint( ("drvfunc_save_filecontent: Write Data is Error,status=0x%4x, Pre Write=%d, WriteBytes=%d\n",
								ntStatus, nWriteBytes, nWriteBytes ));
						}
					}
					else
					{
						KdPrint(("drvfunc_save_filecontent: cann't Set End of File,status=0x%4x\n", ntStatus));
					}
				}
				else
				{
					KdPrint(("drvfunc_save_filecontent: cann't get file size,status=0x%4x\n", ntStatus));
				}
			}
			else
			{
				//
				//	覆盖文件内容
				//
				nWriteBytes	= dwSize;
				ntStatus	= ZwWriteFile
					(
						FileHandle, NULL, NULL, NULL,
						&IoStatus, lpvContent, nWriteBytes, NULL, NULL
					);
				if ( NT_SUCCESS( ntStatus ) && IoStatus.Information == nWriteBytes )
				{
					bRet = TRUE;
				}
				else
				{
					KdPrint( ("drvfunc_save_filecontent: Write Data is Error,status=0x%4x, Pre Write=%d, WriteBytes=%d\n",
						ntStatus, nWriteBytes, nWriteBytes ));
				}
			}

			// ..
			ZwClose( FileHandle );
		}
		else
		{
			KdPrint( ("drvfunc_save_filecontent: Open %ws is fail, status=0x%4x\n", lpcwszFilename, ntStatus) );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_save_filecontent"));
	}

	return bRet;
}


/**
 *	根据文件句柄获取文件 DOS 全路径
 *	UNICODE_STRING
 */
BOOLEAN drvfunc_get_filename_by_handle( IN HANDLE hFileHandle, OUT UNICODE_STRING * pusFilename )
{
	 BOOLEAN bRet = FALSE;
	 NTSTATUS ntStatus;
	 
	 PWCHAR pBuffer;
	 PFILE_OBJECT pstFileObject;
	 POBJECT_NAME_INFORMATION pstObjectNameInfo;
	 
	 
	 if ( NULL == hFileHandle || NULL == pusFilename )
	 {
		 return FALSE;
	 }
	 
	 __try
	 {
		 ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
		 if ( NT_SUCCESS( ntStatus ) )
		 {
			 ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
			 if ( NT_SUCCESS( ntStatus ) )
			 {
				 //pusFilename->Length = 0;
				 RtlCopyUnicodeString( pusFilename, & pstObjectNameInfo->Name );
				 
				 bRet = TRUE;
			 }
		 }
	 }
	 __except( EXCEPTION_EXECUTE_HANDLER )
	 {
		 KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_filename_by_handle"));
	 }
	 
	 return bRet;
}

/**
 *	根据文件句柄获取文件 DOS 全路径
 *	Ansi
 */
UINT drvfunc_get_ansifilename_by_handle( IN HANDLE hFileHandle, OUT PCHAR pszFilename, IN USHORT uSize )
{
	 //
	 //	hFileHandle	- [in]  文件句柄
	 //	pszFilename	- [out] 返回缓冲区
	 //	uSize		- [in]  返回缓冲区的大小
	 //	RETURN		- 如果成功，则返回拷贝的文件名长度
	 //
	 
	 UINT uRet	= 0;
	 NTSTATUS ntStatus;
	 
	 BOOL bValidFilePath;
	 PWCHAR pBuffer;
	 PFILE_OBJECT pstFileObject;
	 POBJECT_NAME_INFORMATION pstObjectNameInfo;
	 
	 if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	 {
		 return 0;
	 }
	 if ( NULL == hFileHandle || NULL == pszFilename || 0 == uSize )
	 {
		 return 0;
	 }
	 
	 __try
	 {
		 ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
		 if ( NT_SUCCESS( ntStatus ) )
		 {
			 ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
			 if ( NT_SUCCESS( ntStatus ) )
			 {
				 bValidFilePath = FALSE;
				 if ( pstObjectNameInfo->Name.Length >= 12 &&
					 L'\\' == pstObjectNameInfo->Name.Buffer[0] &&
					 L'?'  == pstObjectNameInfo->Name.Buffer[1] &&
					 L'?'  == pstObjectNameInfo->Name.Buffer[2] &&
					 L'\\' == pstObjectNameInfo->Name.Buffer[3] &&
					 L':' == pstObjectNameInfo->Name.Buffer[5] )
				{
					 //	"\??\c:\windows"
					 bValidFilePath = TRUE;
				}
				else if ( pstObjectNameInfo->Name.Length >= 6 &&
					L':' == pstObjectNameInfo->Name.Buffer[1] )
				{
					//	"c:\"
					bValidFilePath = TRUE;
				}
				if ( bValidFilePath )
				{
					ntStatus = drvfunc_u2c( & pstObjectNameInfo->Name, pszFilename, uSize );
					if ( NT_SUCCESS( ntStatus ) )
					{
						//
						//	如果转换成功，那么计算一下字符串的长度
						//
						uRet = strlen(pszFilename);
					}
				}
			 }
		 }
	 }
	 __except( EXCEPTION_EXECUTE_HANDLER )
	 {
		 KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_ansifilename_by_handle"));
	 }
	 
	 return uRet;
}
UINT drvfunc_get_widefilename_by_handle( IN HANDLE hFileHandle, OUT PWCHAR pwszFilename, IN USHORT uSize )
{
	//
	//	hFileHandle	- [in]  文件句柄
	//	pszFilename	- [out] 返回缓冲区
	//	uSize		- [in]  返回缓冲区的大小 in bytes
	//	RETURN		- 如果成功，则返回拷贝的文件名长度 in bytes
	//

	UINT uRet;
	NTSTATUS ntStatus;
	BOOL bValidFilePath;
	PWCHAR pBuffer;
	PFILE_OBJECT pstFileObject;
	POBJECT_NAME_INFORMATION pstObjectNameInfo;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return 0;
	}
	if ( NULL == hFileHandle || NULL == pwszFilename || 0 == uSize )
	{
		return 0;
	}
	

	//	...
	uRet	= 0;

	__try
	{
		ntStatus = ObReferenceObjectByHandle( hFileHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (void **)&pstFileObject, NULL );
		if ( NT_SUCCESS( ntStatus ) )
		{
			ntStatus = IoQueryFileDosDeviceName( pstFileObject, &pstObjectNameInfo );
			if ( NT_SUCCESS( ntStatus ) )
			{
				bValidFilePath = FALSE;
				if ( pstObjectNameInfo->Name.Length >= 12 &&
					L'\\' == pstObjectNameInfo->Name.Buffer[0] &&
					L'?'  == pstObjectNameInfo->Name.Buffer[1] &&
					L'?'  == pstObjectNameInfo->Name.Buffer[2] &&
					L'\\' == pstObjectNameInfo->Name.Buffer[3] &&
					L':' == pstObjectNameInfo->Name.Buffer[5] )
				{
					//	"\??\c:\windows"
					bValidFilePath = TRUE;
				}
				else if ( pstObjectNameInfo->Name.Length >= 6 &&
					L':' == pstObjectNameInfo->Name.Buffer[1] )
				{
					//	"c:\"
					bValidFilePath = TRUE;
				}
				if ( bValidFilePath )
				{
					if ( pstObjectNameInfo->Name.Length < uSize )
					{
						RtlCopyMemory( pwszFilename, pstObjectNameInfo->Name.Buffer, pstObjectNameInfo->Name.Length );
						pwszFilename[ pstObjectNameInfo->Name.Length/sizeof(WCHAR) ] = L'\0';
						uRet = pstObjectNameInfo->Name.Length;
					}
					else
					{
						RtlCopyMemory( pwszFilename, pstObjectNameInfo->Name.Buffer, uSize - sizeof(WCHAR) );
						pwszFilename[ uSize/sizeof(WCHAR) - 1 ] = L'\0';
						uRet = uSize - sizeof(WCHAR);
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_widefilename_by_handle"));
	}
	
	return uRet;
}

/**
 *	删除文件
 */
BOOLEAN drvfunc_delete_file( IN UNICODE_STRING * pusFilename )
{
	 //
	 //	pusFilename	- 文件名，L"\\??\\h:\\aa"
	 //	RETURN		- TRUE / FALSE
	 //
	 
	 BOOL bRet;
	 OBJECT_ATTRIBUTES ObjectAttributes;
	 
	 if ( ! pusFilename->Buffer || 0 == pusFilename->Length )
	 {
		 return FALSE;
	 }
	 
	 bRet = FALSE;
	 
	 __try
	 {
		 InitializeObjectAttributes( &ObjectAttributes, pusFilename, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL );
		 bRet = NT_SUCCESS( ZwDeleteFile( &ObjectAttributes ) );
	 }
	 __except( EXCEPTION_EXECUTE_HANDLER )
	 {
		 KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_delete_file"));
	 }
	 
	 return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	for process

/**
 *	获取操作系统版本号
 */
NTSTATUS drvfunc_get_os_version( PULONG pOsMajorVersion, PULONG pOsMinorVersion )
{
	NTSTATUS ntStatus;
	UNICODE_STRING usFunctionName;
	RTL_OSVERSIONINFOW stVersionInfo;

	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return STATUS_UNSUCCESSFUL;
	}
	if ( NULL == pOsMajorVersion || NULL == pOsMinorVersion )
	{
		KdPrint( ("drvfunc_get_os_version:: NULL == pOsMajorVersion || NULL == pOsMinorVersion\n") );
		return STATUS_UNSUCCESSFUL;
	}

	//	..
	ntStatus = STATUS_UNSUCCESSFUL;

	__try
	{
		#if WINVER >= 0x0501
			
			//	This case is supported on Windows XP/2003 and later operating system versions
			stVersionInfo.dwOSVersionInfoSize = sizeof( RTL_OSVERSIONINFOW );
			ntStatus = RtlGetVersion( &stVersionInfo );

			if ( NT_SUCCESS( ntStatus ) )
			{
				*pOsMajorVersion = stVersionInfo.dwMajorVersion;
				*pOsMinorVersion = stVersionInfo.dwMinorVersion;
				KdPrint( ("drvfunc_get_os_version::RtlGetVersion successfully.\n") );
			}
			else
			{
				KdPrint( ("drvfunc_get_os_version::RtlGetVersion failed.\n") );
			}
		#else
			//	This case is supported on Windows 2000/98/me versions of the operating system
			PsGetVersion( pOsMajorVersion, pOsMinorVersion, NULL, NULL );
			// ..
			ntStatus = STATUS_SUCCESS;
			KdPrint( ("drvfunc_get_os_version::PsGetVersion successfully.\n") );
		#endif
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_os_version"));
	}

	return ntStatus;
}


/////////////////////////////////////////////////////////////////
// 取得进程偏移量
ULONG drvfunc_get_processname_offset()
{
	ULONG uRet;
	ULONG i;
	PEPROCESS stCurProc;

	uRet	= 0;

	__try
	{
		stCurProc = PsGetCurrentProcess();

		//
		//	Scan for 12KB, hoping the KPEB never grows that big!
		//	PAGE_SIZE=4k/8k
		//
		for ( i = 0; i < 3 * PAGE_SIZE; i ++ )
		{
			if ( ! strncmp( SYSNAME, (PCHAR)stCurProc + i, strlen(SYSNAME) ) )
			{
				uRet = i;
				break;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_processname_offset"));
	}
	
	return uRet;
}

/**
 *	获取进程名
 *	取得当前进程名,不带路径（例:notepad.exe）
 */
BOOLEAN drvfunc_get_processname( IN ULONG uProcessNameOffset, OUT PCHAR pProcessName, IN ULONG uSize )
{
	//
	//	pProcessName	- [out] Pointer to get the process name, etc: "notepad.exe".
	//

	PEPROCESS       stCurProc;
	char            *nameptr;
	ULONG           i;

	//
	// We only do this if we determined the process name offset
	//
	if ( 0 == uProcessNameOffset )
	{
		return FALSE;
	}

	__try
	{
		//
		// Get a pointer to the current process block
		//
		stCurProc = PsGetCurrentProcess();
		
		//
		// Dig into it to extract the name. Make sure to leave enough room
		// in the buffer for the appended process ID.
		//
		nameptr   = (PCHAR)stCurProc + uProcessNameOffset;

		//..
		_snprintf( pProcessName, uSize/sizeof(char)-1, "%s", nameptr );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_processname"));
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////
//  类别:
//         文件系统操作
//  功能:
//         取得当前进程名完整路径名（例:C:\WINNT\notepad.exe）
//  参数：
//         
//  返回值：
//         STATUS_SUCCESS	成功
//  原理：取得 EPROCESS->PEB->RTL_USER_PROCESS_PARAMETER->ImagePathName
//	  从这原理也可以取出Command Line
/*
  PEPROCESS should be a pointer to _EPROCESS structure.
  I don't think the definition of _EPROCESS is available for public.
  A simple debug in XP SP1 gives me following attributes:
  --------------------------------------------------------------------------------
   +0x000 Pcb              : _KPROCESS
   +0x06c ProcessLock      : _EX_PUSH_LOCK
   +0x070 CreateTime       : _LARGE_INTEGER
   +0x078 ExitTime         : _LARGE_INTEGER
   +0x080 RundownProtect   : _EX_RUNDOWN_REF
   +0x084 UniqueProcessId  : Ptr32 Void
   +0x088 ActiveProcessLinks : _LIST_ENTRY
   +0x090 QuotaUsage       : [3] Uint4B
   +0x09c QuotaPeak        : [3] Uint4B
   +0x0a8 CommitCharge     : Uint4B
   +0x0ac PeakVirtualSize  : Uint4B
   +0x0b0 VirtualSize      : Uint4B
   +0x0b4 SessionProcessLinks : _LIST_ENTRY
   +0x0bc DebugPort        : Ptr32 Void
   +0x0c0 ExceptionPort    : Ptr32 Void
   +0x0c4 ObjectTable      : Ptr32 _HANDLE_TABLE
   +0x0c8 Token            : _EX_FAST_REF
   +0x0cc WorkingSetLock   : _FAST_MUTEX
   +0x0ec WorkingSetPage   : Uint4B
   +0x0f0 AddressCreationLock : _FAST_MUTEX
   +0x110 HyperSpaceLock   : Uint4B
   +0x114 ForkInProgress   : Ptr32 _ETHREAD
   +0x118 HardwareTrigger  : Uint4B
   +0x11c VadRoot          : Ptr32 Void
   +0x120 VadHint          : Ptr32 Void
   +0x124 CloneRoot        : Ptr32 Void
   +0x128 NumberOfPrivatePages : Uint4B
   +0x12c NumberOfLockedPages : Uint4B
   +0x130 Win32Process     : Ptr32 Void
   +0x134 Job              : Ptr32 _EJOB
   +0x138 SectionObject    : Ptr32 Void
   +0x13c SectionBaseAddress : Ptr32 Void
   +0x140 QuotaBlock       : Ptr32 _EPROCESS_QUOTA_BLOCK
   +0x144 WorkingSetWatch  : Ptr32 _PAGEFAULT_HISTORY
   +0x148 Win32WindowStation : Ptr32 Void
   +0x14c InheritedFromUniqueProcessId : Ptr32 Void
   +0x150 LdtInformation   : Ptr32 Void
   +0x154 VadFreeHint      : Ptr32 Void
   +0x158 VdmObjects       : Ptr32 Void
   +0x15c DeviceMap        : Ptr32 Void
   +0x160 PhysicalVadList  : _LIST_ENTRY
   +0x168 PageDirectoryPte : _HARDWARE_PTE
   +0x168 Filler           : Uint8B
   +0x170 Session          : Ptr32 Void
   +0x174 ImageFileName    : [16] UChar
   +0x184 JobLinks         : _LIST_ENTRY
   +0x18c LockedPagesList  : Ptr32 Void
   +0x190 ThreadListHead   : _LIST_ENTRY
   +0x198 SecurityPort     : Ptr32 Void
   +0x19c PaeTop           : Ptr32 Void
   +0x1a0 ActiveThreads    : Uint4B
   +0x1a4 GrantedAccess    : Uint4B
   +0x1a8 DefaultHardErrorProcessing : Uint4B
   +0x1ac LastThreadExitStatus : Int4B
   +0x1b0 Peb              : Ptr32 _PEB
   +0x1b4 PrefetchTrace    : _EX_FAST_REF
   +0x1b8 ReadOperationCount : _LARGE_INTEGER
   +0x1c0 WriteOperationCount : _LARGE_INTEGER
   +0x1c8 OtherOperationCount : _LARGE_INTEGER
   +0x1d0 ReadTransferCount : _LARGE_INTEGER
   +0x1d8 WriteTransferCount : _LARGE_INTEGER
   +0x1e0 OtherTransferCount : _LARGE_INTEGER
   +0x1e8 CommitChargeLimit : Uint4B
   +0x1ec CommitChargePeak : Uint4B
   +0x1f0 AweInfo          : Ptr32 Void
   +0x1f4 SeAuditProcessCreationInfo : _SE_AUDIT_PROCESS_CREATION_INFO
   +0x1f8 Vm               : _MMSUPPORT
   +0x238 LastFaultCount   : Uint4B
   +0x23c ModifiedPageCount : Uint4B
   +0x240 NumberOfVads     : Uint4B
   +0x244 JobStatus        : Uint4B
   +0x248 Flags            : Uint4B
   +0x248 CreateReported   : Pos 0, 1 Bit
   +0x248 NoDebugInherit   : Pos 1, 1 Bit
   +0x248 ProcessExiting   : Pos 2, 1 Bit
   +0x248 ProcessDelete    : Pos 3, 1 Bit
   +0x248 Wow64SplitPages  : Pos 4, 1 Bit
   +0x248 VmDeleted        : Pos 5, 1 Bit
   +0x248 OutswapEnabled   : Pos 6, 1 Bit
   +0x248 Outswapped       : Pos 7, 1 Bit
   +0x248 ForkFailed       : Pos 8, 1 Bit
   +0x248 HasPhysicalVad   : Pos 9, 1 Bit
   +0x248 AddressSpaceInitialized : Pos 10, 2 Bits
   +0x248 SetTimerResolution : Pos 12, 1 Bit
   +0x248 BreakOnTermination : Pos 13, 1 Bit
   +0x248 SessionCreationUnderway : Pos 14, 1 Bit
   +0x248 WriteWatch       : Pos 15, 1 Bit
   +0x248 ProcessInSession : Pos 16, 1 Bit
   +0x248 OverrideAddressSpace : Pos 17, 1 Bit
   +0x248 HasAddressSpace  : Pos 18, 1 Bit
   +0x248 LaunchPrefetched : Pos 19, 1 Bit
   +0x248 InjectInpageErrors : Pos 20, 1 Bit
   +0x248 VmTopDown        : Pos 21, 1 Bit
   +0x248 Unused3          : Pos 22, 1 Bit
   +0x248 Unused4          : Pos 23, 1 Bit
   +0x248 VdmAllowed       : Pos 24, 1 Bit
   +0x248 Unused           : Pos 25, 5 Bits
   +0x248 Unused1          : Pos 30, 1 Bit
   +0x248 Unused2          : Pos 31, 1 Bit
   +0x24c ExitStatus       : Int4B
   +0x250 NextPageColor    : Uint2B
   +0x252 SubSystemMinorVersion : UChar
   +0x253 SubSystemMajorVersion : UChar
   +0x252 SubSystemVersion : Uint2B
   +0x254 PriorityClass    : UChar
   +0x255 WorkingSetAcquiredUnsafe : UChar
   +0x258 Cookie           : Uint4B
*/
BOOLEAN drvfunc_get_processfullname( OUT PCHAR pszFullProcessName, IN ULONG uSize, OUT USHORT * puFullnameLength )
{
	BOOLEAN  bRet;
	NTSTATUS ntStatus;
	ULONG uOsMajorVersion = 0;
	ULONG uOsMinorVersion = 0;
//	WCHAR wszProName[ MAX_PATH ] = {0};
	UNICODE_STRING usSource;
	ANSI_STRING asDest;
#if AMD64
	__int64 dwAddress = 0;
#else
	DWORD dwAddress = 0;
#endif

//	if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
//	{
//		return FALSE;
//	}
	if ( PASSIVE_LEVEL != KeGetCurrentIrql() )
	{
		return FALSE;
	}

	KdPrint( ("drvfunc_get_processfullname:: enter...") );

	//	...
	bRet = FALSE;

	__try
	{
		ntStatus = drvfunc_get_os_version( &uOsMajorVersion, &uOsMinorVersion );
		if ( NT_SUCCESS( ntStatus )  )
		{
			#if AMD64
				dwAddress = (__int64)PsGetCurrentProcess();
			#else
				dwAddress = (DWORD)PsGetCurrentProcess();
			#endif
			if ( dwAddress > 0 && 0xFFFFFFFF != dwAddress )
			{
				//	目前只支持 Win 2000/xp/2003
				if ( uOsMajorVersion >= 5 && uOsMinorVersion <= 2 )
				{
					//	取得 PEB，不同平台的位置是不同的。
					if ( uOsMajorVersion == 5 && uOsMinorVersion < 2 )
					{
						dwAddress += BASE_PROCESS_PEB_OFFSET;
					}
					else
					{
						dwAddress += W2003_BASE_PROCESS_PEB_OFFSET;
					}

					if ( 0 != (dwAddress = *(DWORD*)dwAddress) )
					{
						// 通过peb取得RTL_USER_PROCESS_PARAMETERS
						dwAddress += BASE_PEB_PROCESS_PARAMETER_OFFSET;
						if ( 0 != (dwAddress = *(DWORD*)dwAddress) )
						{
							// 在 RTL_USER_PROCESS_PARAMETERS->ImagePathName 保存了路径，偏移为38,
							dwAddress += BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME;
							if ( 0 != (dwAddress = *(DWORD*)dwAddress) )
							{
								// ..
								//_snwprintf( wszProName, sizeof(wszProName)/sizeof(WCHAR)-1, "%s", (PCWSTR)dwAddress );	
								usSource.Buffer = (PWSTR)dwAddress;
								usSource.Length = wcslen((PWSTR)dwAddress)*sizeof(WCHAR);
								RtlUnicodeStringToAnsiString( &asDest, &usSource, TRUE );
								
								//	...
								bRet = TRUE;
								if ( puFullnameLength )
								{
									*puFullnameLength = (USHORT)( usSource.Length/sizeof(WCHAR) );
								}

								// ...
								if ( asDest.Length > 4 &&
									0 == strncmp( "\\??\\", asDest.Buffer, 4 ) )
								{
									_snprintf( pszFullProcessName, uSize/sizeof(char)-1, "%s", asDest.Buffer+4 );
								}
								else
								{
									_snprintf( pszFullProcessName, uSize/sizeof(char)-1, "%s", asDest.Buffer );
								}
							}
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_processfullname"));
	}

	return TRUE;
}





//////////////////////////////////////////////////////////////////////////
//	for string


/**
 *	获取制定 "keyname=" 的 INT 值 ( 不区分大小写 )
 */
INT drvfunc_get_ini_value_int( CHAR * pszBuffer, LPCSTR lpszKeyName )
{
	INT   nRet;
	CHAR  * pszLine;
	DWORD dwLineSize;

	if ( NULL == pszBuffer || NULL == lpszKeyName )
	{
		return 0;
	}

	//	...
	nRet = 0;

	__try
	{
		dwLineSize	= 1024;
		pszLine		= (CHAR*)ExAllocatePool( NonPagedPool, dwLineSize );
		if ( pszLine )
		{
			RtlZeroMemory( pszLine, dwLineSize );

			if ( drvfunc_get_casecookie_value( pszBuffer, lpszKeyName, "\r\n", pszLine, dwLineSize, TRUE ) > 0 )
			{
				if ( 0 != pszLine[0] )
				{
					nRet = atoi( pszLine );
				}
			}

			ExFreePool( pszLine );
			pszLine = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_ini_value_int"));
	}	

	return nRet;
}

/**
 *	获取制定 "keyname=" 的 BOOL 值 ( 不区分大小写 )
 */
BOOL drvfunc_get_ini_value_bool( CHAR * pszBuffer, LPCSTR lpszKeyName )
{
	return ( 0 != drvfunc_get_ini_value_int( pszBuffer, lpszKeyName ) ? TRUE : FALSE );
}

/**
 *	获取指定名称的 Cookie Value( 不区分大小写 )
 */
UINT drvfunc_get_casecookie_value( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr, LPSTR lpszValue, DWORD dwSize, BOOL bTrimDbQuote )
{
	//
	//	lpszCookieString	- [in]  整个 COOKIE 的字符串
	//	lpszCookieName		- [in]  COOKIE 名称，可以不区分大小写的查找
	//	lpcszEndStr		- [in]  搜索结尾
	//	lpszValue		- [out] 返回值
	//	dwSize			- [in]  返回值缓冲区大小
	//	bTrimDbQuote		- [in]  是否去除字符串两边的 双引号
	//	RETURN			- 获取的字符串的长度
	//
	//	for example:
	//		drvfunc_get_casecookie_value( "name=xing;", "xing=", ";", szValue, sizeof(szValue) );
	//
	
	UINT    uRetLen;
	LPCTSTR	lpszHead;
	LPTSTR	lpszMove;
	UINT	uStrLen;
	UINT    uCKNameLen;
	UINT    i;


	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpcszEndStr || NULL == lpszValue )
	{
		return 0;
	}
	if ( 0 == lpszCookieString[0] || 0 == lpszCookieName[0] || 0 == lpcszEndStr[0] )
	{
		return 0;
	}

	//	...
	uRetLen = 0;

	__try
	{
		uStrLen		= strlen( lpszCookieString );
		uCKNameLen	= strlen( lpszCookieName );
		
		for ( i = 0; i < uStrLen; i ++ )
		{
			lpszHead = lpszCookieString + i;
			if ( 0 == _strnicmp( lpszHead, lpszCookieName, uCKNameLen ) )
			{
				lpszHead += uCKNameLen;
				if ( lpszHead )
				{
					//
					//	拷贝内容到返回值缓冲区
					//
					_snprintf( lpszValue, dwSize/sizeof(CHAR)-1, "%s", lpszHead );

					//
					//	处理尾部
					//
					lpszMove = strstr( lpszValue, lpcszEndStr );
					if ( lpszMove )
					{
						*lpszMove = 0;
					}

					//
					//	去除首位的 引号双引号
					//
					if ( bTrimDbQuote )
					{
						//	处理头部
						if ( strlen(lpszHead) > 1 && '\"' == lpszHead[0] )
						{
							_snprintf( lpszValue, dwSize/sizeof(CHAR)-1, "%s", lpszHead + 1 );
						}

						//	处理尾部
						lpszMove = strchr( lpszValue, '\"' );
						if ( lpszMove )
						{
							*lpszMove = 0;
						}
					}

					uRetLen = strlen(lpszValue);

					break;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_casecookie_value"));
	}
	
	return uRetLen;
}
/**
 *	获取指定名称的 Cookie Value( 不区分大小写 )
 */
BOOLEAN drvfunc_get_casecookie_boolvalue( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr )
{
	BOOLEAN bRet;
	CHAR * pszBuffer	= NULL;
	UINT uBufferLen		= 32;

	//	...
	bRet	= FALSE;

	__try
	{
		pszBuffer = (CHAR*)ExAllocatePool( NonPagedPool, uBufferLen );
		if ( pszBuffer )
		{
			RtlZeroMemory( pszBuffer, uBufferLen );
			
			if ( drvfunc_get_casecookie_value( lpszCookieString, lpszCookieName, lpcszEndStr, pszBuffer, uBufferLen, TRUE ) > 0 )
			{
				if ( 0 != pszBuffer[0] )
				{
					if ( 0 == _stricmp( pszBuffer, "1" ) ||
						0 == _stricmp( pszBuffer, "true" ) ||
						0 == _stricmp( pszBuffer, "yes" ) )
					{
						bRet = TRUE;
					}
				}
			}

			ExFreePool( pszBuffer );
			pszBuffer = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_get_casecookie_boolvalue"));
	}

	return bRet;
}

/**
 *	简单加密解密
 *		xorkey	- 加密的 key，默认等于 0x79
 *		pData	- 内容，可以是明文也可以是密文
 *		不管是明文还是密文，只要送进来就可以解密或者加密
 */
VOID drvfunc_xorenc( LPSTR pData, BYTE xorkey )
{
	//
	//	pData	- [in/out] Pointer to a null-terminated string containing the Data
	//	xorkey	- [in] Specifies the Encode/Decode key
	//		       default value = 0XFF
	//
	__try
	{
		while ( pData && *pData )
		{
			*pData = *pData ^ xorkey;
			pData += sizeof(CHAR);
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		KdPrint(("EXCEPTION_EXECUTE_HANDLER in: drvfunc_xorenc"));
	}
}