#ifndef __PROCESSSTRUCT_HEADER__
#define __PROCESSSTRUCT_HEADER__


//
//	http://msdn.microsoft.com/en-us/library/ff557160(VS.85).aspx
//	Constant			Value	Operating system
//	--------------------------------------------------------------------------------
//	_NT_TARGET_VERSION_WIN7		0x601	Windows 7 and Windows Server 2008 R2
//	_NT_TARGET_VERSION_LONGHORN	0x600	Windows Server 2008
//	_NT_TARGET_VERSION_VISTA	0x600	Windows Vista
//	_NT_TARGET_VERSION_WS03		0x502	Windows Server 2003
//	_NT_TARGET_VERSION_WINXP	0x501	Windows XP
//
//
//
//	PsSetLoadImageNotifyRoutine
//	http://msdn.microsoft.com/en-us/library/ff559957(VS.85).aspx
//

#if ( _NT_TARGET_VERSION_WS03 == _NT_TARGET_VERSION || _NT_TARGET_VERSION_WINXP == _NT_TARGET_VERSION )

typedef struct  _MY_IMAGE_INFO {
    union {
        ULONG  Properties;
        struct {
            ULONG ImageAddressingMode  : 8; //code addressing mode
            ULONG SystemModeImage      : 1; //system mode image
            ULONG ImageMappedToAllPids : 1; //mapped in all processes
            ULONG Reserved             : 22;
        };
    };
    PVOID  ImageBase;
    ULONG  ImageSelector;
    ULONG  ImageSize;
    ULONG  ImageSectionNumber;
} MYIMAGE_INFO, *PMYIMAGE_INFO;

#else
	//	_NT_TARGET_VERSION_WIN7
	//	_NT_TARGET_VERSION_LONGHORN
	//	_NT_TARGET_VERSION_VISTA

typedef struct _MY_IMAGE_INFO {
    union {
        ULONG Properties;
        struct {
            ULONG ImageAddressingMode  : 8;  // Code addressing mode
            ULONG SystemModeImage      : 1;  // System mode image
            ULONG ImageMappedToAllPids : 1;  // Image mapped into all processes
            ULONG ExtendedInfoPresent  : 1;  // IMAGE_INFO_EX available
            ULONG Reserved             : 21;
        };
    };
    PVOID       ImageBase;
    ULONG       ImageSelector;
    SIZE_T      ImageSize;
    ULONG       ImageSectionNumber;
} MYIMAGE_INFO, *PMYIMAGE_INFO;

#endif




#endif	// __PROCESSSTRUCT_HEADER__