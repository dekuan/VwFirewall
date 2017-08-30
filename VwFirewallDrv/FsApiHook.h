#ifndef __APIHOOKFS_HEADER__
#define __APIHOOKFS_HEADER__


/**
 *	Hook Config
 */
#define FSAPIHOOKFS_CFG_ZWCREATEFILE			0x01	//	Zw functions list
#define FSAPIHOOKFS_CFG_ZWOPENFILE			0x02
#define FSAPIHOOKFS_CFG_ZWREADFILE			0x04
#define FSAPIHOOKFS_CFG_ZWWRITEFILE			0x08
#define FSAPIHOOKFS_CFG_ZWQUERYINFORMATIONFILE		0x10
#define FSAPIHOOKFS_CFG_ZWSETINFORMATIONFILE		0x20
#define FSAPIHOOKFS_CFG_ZWCREATEDIRECTORYOBJECT		0x40

#define FSAPIHOOKFS_ACTION_PASS				0x00	//	放过本次操作
#define FSAPIHOOKFS_ACTION_DENIED			0x01	//	拒绝本次操作


/**
 *	Zw Functions List
 */
typedef NTSTATUS (*PFNZWCREATEFILE)
		(
			OUT PHANDLE FileHandle,
			IN ACCESS_MASK DesiredAccess,
			IN POBJECT_ATTRIBUTES ObjectAttributes,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN PLARGE_INTEGER AllocationSize OPTIONAL,
			IN ULONG FileAttributes,
			IN ULONG ShareAccess,
			IN ULONG CreateDisposition,
			IN ULONG CreateOptions,
			IN PVOID EaBuffer OPTIONAL,
			IN ULONG EaLength
		);
typedef NTSTATUS (*PFNZWOPENFILE)
		(
			OUT PHANDLE FileHandle,
			IN ACCESS_MASK DesiredAccess,
			IN POBJECT_ATTRIBUTES ObjectAttributes,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN ULONG ShareAccess,
			IN ULONG OpenOptions
		);

typedef NTSTATUS (*PFNZWREADFILE)
		(
			IN HANDLE FileHandle,
			IN HANDLE Event OPTIONAL,
			IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
			IN PVOID ApcContext OPTIONAL,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			OUT PVOID Buffer,
			IN ULONG Length,
			IN PLARGE_INTEGER ByteOffset OPTIONAL,
			IN PULONG Key OPTIONAL
		);

typedef NTSTATUS (*PFNZWWRITEFILE)
		(
			IN HANDLE FileHandle,
			IN HANDLE Event OPTIONAL,
			IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
			IN PVOID ApcContext OPTIONAL,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN PVOID Buffer,
			IN ULONG Length,
			IN PLARGE_INTEGER ByteOffset OPTIONAL,
			IN PULONG Key OPTIONAL
		);

typedef NTSTATUS (*PFNZWQUERYINFORMATIONFILE)
		(
			IN HANDLE FileHandle,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			OUT PVOID FileInformation,
			IN ULONG Length,
			IN FILE_INFORMATION_CLASS FileInformationClass
		);

typedef NTSTATUS (*PFNZWSETINFORMATIONFILE)
		(
			IN HANDLE FileHandle,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN PVOID FileInformation,
			IN ULONG Length,
			IN FILE_INFORMATION_CLASS FileInformationClass
		);

typedef NTSTATUS (*PFNZWCREATEDIRECTORYOBJECT)
		(
			OUT PHANDLE DirectoryHandle,
			IN ACCESS_MASK DesiredAccess,
			IN POBJECT_ATTRIBUTES ObjectAttributes
		);


/**
 *	保存原始函数地址指针
 */
extern DWORD g_dwFsApiHookConfig;
extern PFNZWCREATEFILE g_pfnOrgZwCreateFile;
extern PFNZWOPENFILE g_pfnOrgZwOpenFile;
extern PFNZWREADFILE g_pfnOrgZwReadFile;
extern PFNZWWRITEFILE g_pfnOrgZwWriteFile;
extern PFNZWQUERYINFORMATIONFILE g_pfnOrgZwQueryInformationFile;
extern PFNZWSETINFORMATIONFILE g_pfnOrgZwSetInformationFile;
extern PFNZWCREATEDIRECTORYOBJECT g_pfnOrgZwCreateDirectoryObject;


/**
 *	函数定义
 */
VOID  fsapihook_set_config( DWORD dwHookConfig );
VOID  fsapihook_hook_install();
VOID  fsapihook_hook_uninstall();

DWORD fsapihook_check_action( IN HANDLE hFileHandle, BOOLEAN bCheckAllParentDirs );
DWORD fsapihook_check_action_by_filename( IN PWCHAR pwszFilename, IN USHORT uFileNameLength );
DWORD fsapihook_check_action_for_zwcreatefile( IN POBJECT_ATTRIBUTES pObjectAttributes );

NTSTATUS NewZwCreateFile
(
	OUT PHANDLE  FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
);
NTSTATUS NewZwOpenFile
(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
);
NTSTATUS NewZwReadFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
);
NTSTATUS NewZwWriteFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
);
NTSTATUS NewZwQueryInformationFile
(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);
NTSTATUS NewZwSetInformationFile
(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);
NTSTATUS NewZwCreateDirectoryObject
(
	OUT PHANDLE DirectoryHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);




#endif	// __APIHOOKFS_HEADER__