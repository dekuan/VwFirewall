# Microsoft Developer Studio Project File - Name="VwFirewallCfg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VwFirewallCfg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VwFirewallCfg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VwFirewallCfg.mak" CFG="VwFirewallCfg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VwFirewallCfg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VwFirewallCfg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VwFirewallCfg - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del Release\*.obj
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VwFirewallCfg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "VwFirewallCfg - Win32 Release"
# Name "VwFirewallCfg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BackgroundUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BindStatusCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BkDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ColorStatic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeAdsiOpIIS.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeCaptionStatic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeHttp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeThreadSleep.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgClientAdd.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDmAdd.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEnterKey.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExpire.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgImport.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProtectedDirAdd.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRecoverReg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRecoverSecurity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgService.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetRDPPort.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSplash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\HardwareInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\Ini.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ModuleInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ParseCfgData.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcFunc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcSecurityDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcVwAd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcWinFirewall.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryAclsInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\TabBaseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TabDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\TabFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TabLicense.cpp
# End Source File
# Begin Source File

SOURCE=.\TabRdp.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSAntiVirus.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgService.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\TabUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\VwFirewallCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\VwFirewallCfg.rc
# End Source File
# Begin Source File

SOURCE=.\VwFirewallCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwFirewallConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwIniFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwLicence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwOnlineLicenseCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\XingBalloon.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BackgroundUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BCMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BindStatusCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BkDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\BtnST.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeAdsiOpIIS.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeCaptionStatic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeHttp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\DeThreadSleep.h
# End Source File
# Begin Source File

SOURCE=.\DlgClientAdd.h
# End Source File
# Begin Source File

SOURCE=.\DlgDetail.h
# End Source File
# Begin Source File

SOURCE=.\DlgDmAdd.h
# End Source File
# Begin Source File

SOURCE=.\DlgEnterKey.h
# End Source File
# Begin Source File

SOURCE=.\DlgExpire.h
# End Source File
# Begin Source File

SOURCE=.\DlgImport.h
# End Source File
# Begin Source File

SOURCE=.\DlgProtectedDirAdd.h
# End Source File
# Begin Source File

SOURCE=.\DlgRecoverReg.h
# End Source File
# Begin Source File

SOURCE=.\DlgRecoverSecurity.h
# End Source File
# Begin Source File

SOURCE=.\DlgService.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetRDPPort.h
# End Source File
# Begin Source File

SOURCE=.\DlgSplash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\HardwareInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\HyperLink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\Ini.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ModuleInfo.h
# End Source File
# Begin Source File

SOURCE=.\ParseCfgData.h
# End Source File
# Begin Source File

SOURCE=.\ProcFunc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcSecurityDesc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcVwAd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\ProcWinFirewall.h
# End Source File
# Begin Source File

SOURCE=.\QueryAclsInfo.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabAbout.h
# End Source File
# Begin Source File

SOURCE=.\TabBaseInfo.h
# End Source File
# Begin Source File

SOURCE=.\TabDomain.h
# End Source File
# Begin Source File

SOURCE=.\TabFile.h
# End Source File
# Begin Source File

SOURCE=.\TabLicense.h
# End Source File
# Begin Source File

SOURCE=.\TabRdp.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfg.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSAntiVirus.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSFile.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgACLSFolder.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgBase.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgObject.h
# End Source File
# Begin Source File

SOURCE=.\TabSecurityCfgService.h
# End Source File
# Begin Source File

SOURCE=.\TabSetting.h
# End Source File
# Begin Source File

SOURCE=.\TabUpdate.h
# End Source File
# Begin Source File

SOURCE=..\..\VwInclude\VwConst.h
# End Source File
# Begin Source File

SOURCE=..\..\VwInclude\VwConstBase.h
# End Source File
# Begin Source File

SOURCE=..\..\VwInclude\VwConstCommon.h
# End Source File
# Begin Source File

SOURCE=.\VwFirewallCfg.h
# End Source File
# Begin Source File

SOURCE=.\VwFirewallCfgDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwFirewallConfigFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwFirewallDrvIOCtl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwIniFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwLicence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\VwOnlineLicenseCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeSafe\DeInclude\XingBalloon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\icon\32x32-status-domain-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\32x32-status-domain-x.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\32x32-status-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\32x32-status-rdp-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\32x32-status-rdp-x.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\32x32-status-x.ico"
# End Source File
# Begin Source File

SOURCE=.\bmp\bg_white.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\bg_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_block_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_block_zhcn_.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_exception_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_exception_zhcn_.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_filter_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_filter_zhcn_.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_general_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\btn_general_zhcn_.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\button_apply.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\button_detect.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\button_fix.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\button_restore.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\Hand.cur
# End Source File
# Begin Source File

SOURCE=.\icon\icon.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_alert.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_arrow_r.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_info.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading1.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading2.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading3.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading4.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading5.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_loading6.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_ok.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_okb.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_unknown.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_wait_s.ico
# End Source File
# Begin Source File

SOURCE=.\icon\icon_x.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\imglist_tree.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\license_check.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\license_key.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\o.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\o.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\shield_green.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\shield_green.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\shield_red.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\shield_red.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\shield_yellow.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\shield_yellow.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\splash_chs.bmp
# End Source File
# Begin Source File

SOURCE=".\icon\status-domain-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-domain-x.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-file-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-file-x.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-rdp-ok.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-rdp-x.ico"
# End Source File
# Begin Source File

SOURCE=".\icon\status-x.ico"
# End Source File
# Begin Source File

SOURCE=.\bmp\status_o.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\status_wait.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\status_x.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\systray_run.ico
# End Source File
# Begin Source File

SOURCE=.\icon\systray_running.ico
# End Source File
# Begin Source File

SOURCE=.\icon\systray_stopped.ico
# End Source File
# Begin Source File

SOURCE=.\bmp\title_chs.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\title_en.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\title_zhcn.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\update.ico
# End Source File
# Begin Source File

SOURCE=.\res\VwFirewallCfg.ico
# End Source File
# Begin Source File

SOURCE=.\res\VwFirewallCfg.rc2
# End Source File
# Begin Source File

SOURCE=.\icon\x.bmp
# End Source File
# Begin Source File

SOURCE=.\icon\x.ico
# End Source File
# Begin Source File

SOURCE=.\icon\xptheme.bin
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
