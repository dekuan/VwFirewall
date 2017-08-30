;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 多语言包
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LangString g_sMlProductName ${LANG_ENGLISH} "VirtualWall Firewall"
LangString g_sMlProductName ${LANG_SIMPCHINESE} "微盾防火墙"

LangString g_sMlUninstall ${LANG_ENGLISH} "Uninstall"
LangString g_sMlUninstall ${LANG_SIMPCHINESE} "卸载"

LangString g_sMlHomepage ${LANG_ENGLISH} "VirtualWall Homepage"
LangString g_sMlHomepage ${LANG_SIMPCHINESE} "微盾官方网站"

LangString g_sMlHelpDoc ${LANG_ENGLISH} "Help Documents"
LangString g_sMlHelpDoc ${LANG_SIMPCHINESE} "帮助文档"

LangString g_sMlError_OSMust2003 ${LANG_ENGLISH} "Sorry, this software must be installed on the Windows 2003 operating system!"
LangString g_sMlError_OSMust2003 ${LANG_SIMPCHINESE} "对不起，该软件必须安装在 Windows 2003 操作系统上！"

LangString g_sMlError_TestSys32Dir ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR directory can be written into the DLL files."
LangString g_sMlError_TestSys32Dir ${LANG_SIMPCHINESE} "安装无法继续，请确认“$SYSDIR”目录可以写入 DLL 文件。"

LangString g_sMlError_TestSysDriverDir ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR\drivers directory can be written into the DLL files."
LangString g_sMlError_TestSysDriverDir ${LANG_SIMPCHINESE} "安装无法继续，请确认“$SYSDIR\drivers”目录可以写入 DLL 文件。"

LangString g_sMlError_TestSysDriverDir2 ${LANG_ENGLISH} "We cann't continue to install, make sure that the $SYSDIR\drivers directory can be written into the .sys files."
LangString g_sMlError_TestSysDriverDir2 ${LANG_SIMPCHINESE} "安装无法继续，请确认“$SYSDIR\drivers”目录可以写入 .sys 文件。"

LangString g_sMlInsRestartIISNote ${LANG_ENGLISH} "Note: We will stop IIS Service, Are you ready?"
LangString g_sMlInsRestartIISNote ${LANG_SIMPCHINESE} "注意:安装过程将停止 IIS 服务，是否准备好了？"

LangString g_sMlInsSuccess ${LANG_ENGLISH} "Congratulations! The software was installed successfully. You must reboot this computer after using it. $\r$\nAre you want to reboot this computer now?"
LangString g_sMlInsSuccess ${LANG_SIMPCHINESE} "恭喜，软件安装完成，必须需要重启操作系统才能正常使用。$\r$\n是否现在就重启计算机？"

LangString g_sMlUninsCompleted ${LANG_ENGLISH} "$(g_sMlProductName) ${PRODUCT_VERSION} was uninstalled completed! "
LangString g_sMlUninsCompleted ${LANG_SIMPCHINESE} "$(g_sMlProductName) ${PRODUCT_VERSION} 卸载完成！"

LangString g_sMlUninsConfirm ${LANG_ENGLISH} "Are you sure you want to uninstall $(g_sMlProductName) ${PRODUCT_VERSION}?"
LangString g_sMlUninsConfirm ${LANG_SIMPCHINESE} "确认要完全卸载“$(g_sMlProductName) ${PRODUCT_VERSION}”"

LangString g_sMlUninsRestartIISNote ${LANG_ENGLISH} "Note: We will stop IIS Service, Are you ready?"
LangString g_sMlUninsRestartIISNote ${LANG_SIMPCHINESE} "注意:卸载过程将停止 IIS 服务，是否准备好了？"

LangString g_sMlDllUpgradeMustReboot ${LANG_ENGLISH} "In order to make the program work correctly, we need to upgrade some shared DLL files, but the DLL files is being used by other programs.$\r$\nWe must restart this computer to complete the upgrade, Are you want to reboot this computer now?"
LangString g_sMlDllUpgradeMustReboot ${LANG_SIMPCHINESE} "软件使用的共享 DLL 文件需要升级后才能保证程序正常运行。由于该 DLL 文件正在被其他程序使用，无法更新！$\r$\n必须重启计算机才能完成升级，是否现在就重启计算机？"

LangString g_sMlDllUpgradeMustReboot2 ${LANG_ENGLISH} "Please rerun this installation program after reboot!"
LangString g_sMlDllUpgradeMustReboot2 ${LANG_SIMPCHINESE} "请在重启计算机后再次运行该安装程序！"



;
;   多语言选择对话框
;
!macro ShowMultiLanguageSelectionDialog

   ;Language selection dialog

   Push ""
   Push ${LANG_ENGLISH}
   Push English
   Push ${LANG_SIMPCHINESE}
   Push "Simplified Chinese"
   Push A ; A means auto count languages
          ; for the auto count to work the first empty push (Push "") must remain
   LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

   Pop $LANGUAGE
   StrCmp $LANGUAGE "cancel" 0 +2
          Abort

!macroend
