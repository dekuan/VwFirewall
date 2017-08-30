// DrvFuncReg.c: implementation of the DrvFuncReg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrvFuncReg.h"




/**
 *	从注册表读取 DWORD 值
 */
BOOLEAN drvfuncreg_get_value_ulong( UNICODE_STRING * pusRegistryPath, UNICODE_STRING * pusKeyName, ULONG * pulValue )
{
	BOOL bRet;
	NTSTATUS ntStatus;
	HANDLE hDrvKey;
	OBJECT_ATTRIBUTES attributes;

	PUCHAR puszBuffer	= NULL;
	ULONG  ulBufferLen	= 512;

	PKEY_VALUE_PARTIAL_INFORMATION pKeyValue;
	ULONG ulResultLength;
	ULONG dwDisable = 0;

	if ( NULL == pusRegistryPath || NULL == pusKeyName || NULL == pulValue )
	{
		return FALSE;
	}

	bRet = FALSE;

	//	分配数据缓冲区
	puszBuffer = (PUCHAR)ExAllocatePool( NonPagedPool, ulBufferLen );
	if ( puszBuffer )
	{
		InitializeObjectAttributes( &attributes, pusRegistryPath, OBJ_CASE_INSENSITIVE, NULL, NULL );
		ntStatus = ZwOpenKey( &hDrvKey, KEY_READ, &attributes );
		if ( NT_SUCCESS( ntStatus ) )
		{
			dwDisable = 0;
			ntStatus = ZwQueryValueKey( hDrvKey, pusKeyName, KeyValuePartialInformation, puszBuffer, ulBufferLen, &ulResultLength );
			if ( NT_SUCCESS( ntStatus ) )
			{
				pKeyValue = (PKEY_VALUE_PARTIAL_INFORMATION) puszBuffer;
				if ( pKeyValue->DataLength <= sizeof(ULONG) )
				{
					bRet = TRUE;
					RtlCopyMemory( pulValue, pKeyValue->Data, pKeyValue->DataLength );
				}
			}

			ZwClose( hDrvKey );
		}

		//	释放资源
		ExFreePool( puszBuffer );
		puszBuffer = NULL;
	}

	return bRet;	
}