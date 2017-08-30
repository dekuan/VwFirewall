# Microsoft Developer Studio Project File - Name="VwFirewallDrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VwFirewallDrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VwFirewallDrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VwFirewallDrv.mak" CFG="VwFirewallDrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VwFirewallDrv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VwFirewallDrv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /Oxs /c
# ADD CPP /nologo /Gz /W3 /WX /Oy /Gy /I "$(ddkroot)\inc\ddk" /I "$(ddkroot)\inc" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D i386=1 /FR /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(ddkroot)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /debug /debugtype:coff /machine:IX86 /nodefaultlib /out:"Release\VwFirewallDrv.sys" /libpath:"$(ddkroot)\libfre\i386" /libpath:"$(ddkroot)\lib\i386\free" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Z7 /Oi /Gy /I "$(ddkroot)\inc\ddk" /I "$(ddkroot)\inc" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "_DEBUG" /D _DLL=1 /D _X86_=1 /D i386=1 /FR /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(ddkroot)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"Debug\VwFirewallDrv.sys" /libpath:"$(ddkroot)\libchk\i386" /libpath:"$(ddkroot)\lib\i386\checked" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native

!ENDIF 

# Begin Target

# Name "VwFirewallDrv - Win32 Release"
# Name "VwFirewallDrv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\crc32.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Driver.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrvFunc.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrvFuncReg.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrvThreadLogger.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FsApiHook.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\md5.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcConfig.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcPacket.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcProcess.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcRdp.c

!IF  "$(CFG)" == "VwFirewallDrv - Win32 Release"

!ELSEIF  "$(CFG)" == "VwFirewallDrv - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VwFirewallDrv.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\crc32.h
# End Source File
# Begin Source File

SOURCE=.\Driver.h
# End Source File
# Begin Source File

SOURCE=.\DrvFunc.h
# End Source File
# Begin Source File

SOURCE=.\DrvFuncReg.h
# End Source File
# Begin Source File

SOURCE=.\DrvThreadLogger.h
# End Source File
# Begin Source File

SOURCE=.\FsApiHook.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\NetHeaders.h
# End Source File
# Begin Source File

SOURCE=.\ntifs.h
# End Source File
# Begin Source File

SOURCE=.\ntifs_for_wdk.h
# End Source File
# Begin Source File

SOURCE=.\ProcConfig.h
# End Source File
# Begin Source File

SOURCE=.\ProcessStruct.h
# End Source File
# Begin Source File

SOURCE=.\ProcPacket.h
# End Source File
# Begin Source File

SOURCE=.\ProcProcess.h
# End Source File
# Begin Source File

SOURCE=.\ProcRdp.h
# End Source File
# Begin Source File

SOURCE=.\RdpStruct.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\SysZwFunc.h
# End Source File
# Begin Source File

SOURCE=.\VwFirewallDrvIOCtl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
