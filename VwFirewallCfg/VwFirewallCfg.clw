; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTabRdp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "VwFirewallCfg.h"

ClassCount=34
Class1=CVwFirewallCfgApp
Class2=CVwFirewallCfgDlg
Class3=CAboutDlg

ResourceCount=32
Resource1=IDD_TAB_SECURITYCFG_OBJECT
Resource2=IDR_MAINFRAME
Resource3=IDR_MENU_ADD_EP
Resource4=IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS
Resource5=IDD_TAB_LICENSE
Class4=CTabBlock
Class5=CTabException
Class6=CTabGeneral
Resource6=IDR_MENU_SYSTRAY
Resource7=IDD_DLG_SPLASH
Class7=CTabAbout
Resource8=IDD_DLG_DMADD
Class8=CTabUpdate
Resource9=IDD_VWFIREWALLCFG_DIALOG
Class9=CTabLicense
Class10=CTabSetting
Class11=CTabFilter
Class12=CDlgEnterKey
Class13=CDlgBuildDict
Class14=CTabAntiSql
Resource10=IDD_DLG_SETRDPPORT
Class15=CTabDomain
Class16=CDlgDmAdd
Resource11=IDR_MENU_SVC_LIST
Class17=CDlgExpire
Resource12=IDD_TAB_SECURITYCFG_SERVICE
Class18=CDlgImport
Resource13=IDD_DLG_RECOVER_REG
Class19=CTabRdp
Resource14=IDD_TAB_SECURITYCFG
Class20=CDlgClientAdd
Resource15=IDD_DLG_CLIENTADD
Class21=CDlgSetRDPPort
Resource16=IDD_DLG_PROTECTEDDIR_ADD
Class22=CDlgSplash
Resource17=IDD_ABOUTBOX
Class23=CTabFile
Resource18=IDD_DLG_ENTERKEY
Class24=CDlgProtectedDirAdd
Resource19=IDD_TAB_RDP
Resource20=IDD_DIALOG_EXPIRE
Class25=CTabSecurityCfg
Resource21=IDD_TAB_FILE
Class26=CTabSecurityCfgACLSFile
Resource22=IDD_DLG_DETAIL
Class27=CTabSecurityCfgACLSFolder
Resource23=IDD_TAB_SECURITYCFG_ACLS_FILE
Resource24=IDD_DLG_IMPORT
Class28=CDlgDetail
Resource25=IDD_TAB_DOMAIN
Class29=CTabSecurityCfgACLSAntiVirus
Resource26=IDD_TAB_SETTING
Class30=CTabSecurityCfgService
Resource27=IDD_DLG_RECOVER_SECURITY
Resource28=IDD_TAB_UPDATE
Class31=CDlgService
Resource29=IDD_DLG_SERVICE
Class32=CTabSecurityCfgObject
Resource30=IDD_TAB_ABOUT
Class33=CDlgRecoverReg
Resource31=IDD_TAB_SECURITYCFG_ACLS_FOLDER
Class34=CDlgRecoverSecurity
Resource32=IDR_MENU_ACLS_LIST

[CLS:CVwFirewallCfgApp]
Type=0
HeaderFile=VwFirewallCfg.h
ImplementationFile=VwFirewallCfg.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CVwFirewallCfgApp

[CLS:CVwFirewallCfgDlg]
Type=0
HeaderFile=VwFirewallCfgDlg.h
ImplementationFile=VwFirewallCfgDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDM_MENU_SYSTRAY_EXIT

[CLS:CAboutDlg]
Type=0
HeaderFile=VwFirewallCfgDlg.h
ImplementationFile=VwFirewallCfgDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_VWFIREWALLCFG_DIALOG]
Type=1
Class=CVwFirewallCfgDlg
ControlCount=11
Control1=IDC_STATIC_TITIMAGE,static,1342177294
Control2=IDC_STATIC_LINK_VW,static,1342308352
Control3=IDC_STATIC_SPLIT1,static,1342308352
Control4=IDC_STATIC_LINK_DISCUSS,static,1342308352
Control5=IDC_STATIC_LINK_AD,static,1342308352
Control6=IDC_STATIC_SPLIT2,static,1342308352
Control7=IDC_STATIC_LINK_HELPDOC,static,1342308352
Control8=IDC_BUTTON_STATUS,button,1342242816
Control9=IDC_BUTTON_LICENSE,button,1342242816
Control10=IDC_BUTTON_UPDATE,button,1342242816
Control11=IDC_BUTTON_SECURITYCFG,button,1342242816

[CLS:CTabBlock]
Type=0
HeaderFile=TabBlock.h
ImplementationFile=TabBlock.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT_IN_BLOCK_WORDS
VirtualFilter=dWC

[CLS:CTabException]
Type=0
HeaderFile=TabException.h
ImplementationFile=TabException.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabException
VirtualFilter=dWC

[CLS:CTabGeneral]
Type=0
HeaderFile=TabGeneral.h
ImplementationFile=TabGeneral.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabGeneral
VirtualFilter=dWC

[MNU:IDR_MENU_SYSTRAY]
Type=1
Class=CVwFirewallCfgDlg
Command1=IDM_MENU_SYSTRAY_DEHOMEPAGE
Command2=IDM_MENU_SYSTRAY_OPEN
Command3=IDM_MENU_SYSTRAY_OPENWORKINGDIR
Command4=IDM_MENU_SYSTRAY_OPENHELPDOC
Command5=IDM_MENU_SYSTRAY_STARTAUTO
Command6=IDM_MENU_SYSTRAY_EXIT
CommandCount=6

[DLG:IDD_TAB_ABOUT]
Type=1
Class=CTabAbout
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC_ABOUT_MORE,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:CTabAbout]
Type=0
HeaderFile=TabAbout.h
ImplementationFile=TabAbout.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTabAbout

[DLG:IDD_TAB_UPDATE]
Type=1
Class=CTabUpdate
ControlCount=3
Control1=IDC_STATIC_CURRVER,static,1342308352
Control2=IDC_STATIC_DOWNLOAD,static,1342308352
Control3=IDC_BUTTON_CHECKFORUPDATE,button,1342242816

[CLS:CTabUpdate]
Type=0
HeaderFile=TabUpdate.h
ImplementationFile=TabUpdate.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabUpdate
VirtualFilter=dWC

[DLG:IDD_TAB_LICENSE]
Type=1
Class=CTabLicense
ControlCount=5
Control1=IDC_RICHEDIT_INFO,RICHEDIT,1350631812
Control2=IDC_STATIC_HOWTOBUY,static,1342308352
Control3=IDC_BUTTON_ENTER_KEY,button,1342242816
Control4=IDC_BUTTON_ONLINE_CHECK,button,1342242816
Control5=IDC_STATIC,static,1342308352

[CLS:CTabLicense]
Type=0
HeaderFile=TabLicense.h
ImplementationFile=TabLicense.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTabLicense

[DLG:IDD_TAB_SETTING]
Type=1
Class=CTabSetting
ControlCount=24
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC_ABOUT_MORE,static,1342308352
Control4=IDC_STATIC_STATUS,static,1342177283
Control5=IDC_STATIC_FIREWALLSTATUS,static,1342308352
Control6=IDC_STATIC_CTLLINK_FIREWALL,static,1342308352
Control7=IDC_STATIC_STATUS_FILE,static,1342177283
Control8=IDC_STATIC_STATUS_FILE_TXT,static,1342308352
Control9=IDC_STATIC_CTLLINK_FILE,static,1342308352
Control10=IDC_STATIC_CTLLINK_FILE_SET,static,1342308352
Control11=IDC_STATIC_CTLLINK_FILE_SP,static,1342308352
Control12=IDC_STATIC_PFILE_BLOCKCOUNT,static,1342308352
Control13=IDC_STATIC_STATUS_RDP,static,1342177283
Control14=IDC_STATIC_STATUS_RDP_TXT,static,1342308352
Control15=IDC_STATIC_CTLLINK_RDP,static,1342308352
Control16=IDC_STATIC_CTLLINK_RDP_SET,static,1342308352
Control17=IDC_STATIC_CTLLINK_RDP_SP,static,1342308352
Control18=IDC_STATIC_PRDP_BLOCKCOUNT,static,1342308352
Control19=IDC_STATIC_STATUS_DOMAIN,static,1342177283
Control20=IDC_STATIC_STATUS_DOMAIN_TXT,static,1342308352
Control21=IDC_STATIC_CTLLINK_DOMAIN,static,1342308352
Control22=IDC_STATIC_CTLLINK_DOMAIN_SET,static,1342308352
Control23=IDC_STATIC_CTLLINK_DOMAIN_SP,static,1342308352
Control24=IDC_STATIC_PDOMAIN_BLOCKCOUNT,static,1342308352

[CLS:CTabSetting]
Type=0
HeaderFile=TabSetting.h
ImplementationFile=TabSetting.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabSetting
VirtualFilter=dWC

[CLS:CTabFilter]
Type=0
HeaderFile=tabfilter.h
ImplementationFile=tabfilter.cpp
BaseClass=CDialog
LastObject=CTabFilter
Filter=D
VirtualFilter=dWC

[DLG:IDD_DLG_ENTERKEY]
Type=1
Class=CDlgEnterKey
ControlCount=8
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COMBO_REGIP,combobox,1344340227
Control5=IDC_EDIT_REGKEY,edit,1350631552
Control6=IDC_BUTTON_SAVE,button,1342242816
Control7=IDCANCEL,button,1342242816
Control8=IDC_BUTTON_COPY,button,1342242816

[CLS:CDlgEnterKey]
Type=0
HeaderFile=DlgEnterKey.h
ImplementationFile=DlgEnterKey.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgEnterKey

[CLS:CDlgBuildDict]
Type=0
HeaderFile=DlgBuildDict.h
ImplementationFile=DlgBuildDict.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgBuildDict
VirtualFilter=dWC

[CLS:CTabAntiSql]
Type=0
HeaderFile=TabAntiSql.h
ImplementationFile=TabAntiSql.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT_IN_ANTISQL_WORDS
VirtualFilter=dWC

[DLG:IDD_TAB_DOMAIN]
Type=1
Class=CTabDomain
ControlCount=8
Control1=IDC_BUTTON_SAVE,button,1342242816
Control2=IDC_BUTTON_RESTORE,button,1342242816
Control3=IDC_BUTTON_ADD,button,1342242816
Control4=IDC_BUTTON_DEL,button,1342242816
Control5=IDC_BUTTON_MOD,button,1342242816
Control6=IDC_LIST_DOMAIN,SysListView32,1350631425
Control7=IDC_BUTTON_IMP,button,1342242816
Control8=IDC_STATIC,static,1342308352

[CLS:CTabDomain]
Type=0
HeaderFile=TabDomain.h
ImplementationFile=TabDomain.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabDomain
VirtualFilter=dWC

[DLG:IDD_DLG_DMADD]
Type=1
Class=CDlgDmAdd
ControlCount=8
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_CHECK_USE,button,1342242819
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_PANDOMAIN,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_SAVE,button,1342242817
Control8=IDCANCEL,button,1342242816

[CLS:CDlgDmAdd]
Type=0
HeaderFile=DlgDmAdd.h
ImplementationFile=DlgDmAdd.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgDmAdd
VirtualFilter=dWC

[DLG:IDD_DIALOG_EXPIRE]
Type=1
Class=CDlgExpire
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC_INFO,static,1342308352
Control4=IDC_STATIC_HOWTORENEW,static,1342308352

[CLS:CDlgExpire]
Type=0
HeaderFile=DlgExpire.h
ImplementationFile=DlgExpire.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgExpire
VirtualFilter=dWC

[DLG:IDD_DLG_IMPORT]
Type=1
Class=CDlgImport
ControlCount=4
Control1=IDC_STATIC,button,1342177287
Control2=IDC_BUTTON_IMPORT,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_LIST_DOMAIN,SysListView32,1350631425

[CLS:CDlgImport]
Type=0
HeaderFile=DlgImport.h
ImplementationFile=DlgImport.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_BUTTON_IMPORT
VirtualFilter=dWC

[DLG:IDD_TAB_RDP]
Type=1
Class=CTabRdp
ControlCount=8
Control1=IDC_BUTTON_SAVE,button,1342242816
Control2=IDC_BUTTON_RESTORE,button,1342242816
Control3=IDC_BUTTON_ADD,button,1342242816
Control4=IDC_BUTTON_DEL,button,1342242816
Control5=IDC_BUTTON_MOD,button,1342242816
Control6=IDC_LIST_RDP,SysListView32,1350631425
Control7=IDC_BUTTON_MODRDPPORT,button,1342242816
Control8=IDC_STATIC,static,1342308352

[CLS:CTabRdp]
Type=0
HeaderFile=TabRdp.h
ImplementationFile=TabRdp.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabRdp
VirtualFilter=dWC

[DLG:IDD_DLG_CLIENTADD]
Type=1
Class=CDlgClientAdd
ControlCount=8
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_CHECK_USE,button,1342242819
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_CLIENTNAME,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_SAVE,button,1342242817
Control8=IDCANCEL,button,1342242816

[CLS:CDlgClientAdd]
Type=0
HeaderFile=DlgClientAdd.h
ImplementationFile=DlgClientAdd.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgClientAdd
VirtualFilter=dWC

[DLG:IDD_DLG_SETRDPPORT]
Type=1
Class=CDlgSetRDPPort
ControlCount=13
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_ORG_PORT,edit,1350633600
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_NEW_PORT,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_CHECK_ADDTO_FW_EXCEPTION,button,1342242819
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_BUTTON_SAVE,button,1476460544
Control13=IDCANCEL,button,1342242816

[CLS:CDlgSetRDPPort]
Type=0
HeaderFile=DlgSetRDPPort.h
ImplementationFile=DlgSetRDPPort.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgSetRDPPort
VirtualFilter=dWC

[DLG:IDD_DLG_SPLASH]
Type=1
Class=CDlgSplash
ControlCount=1
Control1=IDC_STATIC_IMAGE,static,1342177294

[CLS:CDlgSplash]
Type=0
HeaderFile=DlgSplash.h
ImplementationFile=DlgSplash.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgSplash
VirtualFilter=dWC

[DLG:IDD_TAB_FILE]
Type=1
Class=CTabFile
ControlCount=11
Control1=IDC_BUTTON_SAVE,button,1342242816
Control2=IDC_BUTTON_RESTORE,button,1342242816
Control3=IDC_BUTTON_ADD_DIR,button,1342242816
Control4=IDC_BUTTON_DEL_DIR,button,1342242816
Control5=IDC_BUTTON_MOD_DIR,button,1342242816
Control6=IDC_LIST_PROTECTEDDIR,SysListView32,1350631425
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BUTTON_ADD_EP,button,1342242816
Control9=IDC_BUTTON_DEL_EP,button,1342242816
Control10=IDC_LIST_EXCEPTEDPROCESS,SysListView32,1350647809
Control11=IDC_STATIC,static,1342308352

[CLS:CTabFile]
Type=0
HeaderFile=TabFile.h
ImplementationFile=TabFile.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabFile
VirtualFilter=dWC

[DLG:IDD_DLG_PROTECTEDDIR_ADD]
Type=1
Class=CDlgProtectedDirAdd
ControlCount=14
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_CHECK_USE,button,1342242819
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_DIR,edit,1350631552
Control6=IDC_BUTTON_SAVE,button,1342242817
Control7=IDCANCEL,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_CHECK_TREE,button,1342242819
Control10=IDC_BUTTON_BROWSERDIR,button,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_ECPEXT,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352

[CLS:CDlgProtectedDirAdd]
Type=0
HeaderFile=DlgProtectedDirAdd.h
ImplementationFile=DlgProtectedDirAdd.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHECK_TREE
VirtualFilter=dWC

[MNU:IDR_MENU_ADD_EP]
Type=1
Class=?
CommandCount=0

[DLG:IDD_TAB_SECURITYCFG]
Type=1
Class=CTabSecurityCfg
ControlCount=7
Control1=IDC_RADIO_ACLS_FILE,button,1342181385
Control2=IDC_RADIO_ACLS_FOLDER,button,1342181385
Control3=IDC_RADIO_ACLS_ANTIVIRUS,button,1342181385
Control4=IDC_RADIO_OBJECT,button,1342181385
Control5=IDC_RADIO_SERVICE,button,1342181385
Control6=IDC_STATIC_INFO,static,1342308352
Control7=IDC_STATIC_INFOICON,static,1342177283

[CLS:CTabSecurityCfg]
Type=0
HeaderFile=TabSecurityCfg.h
ImplementationFile=TabSecurityCfg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_STATIC_STATUSICON

[DLG:IDD_TAB_SECURITYCFG_ACLS_FILE]
Type=1
Class=CTabSecurityCfgACLSFile
ControlCount=5
Control1=IDC_LIST_GROUP,SysListView32,1350631425
Control2=IDC_BUTTON_DETECT,button,1342242816
Control3=IDC_BUTTON_FIX,button,1476460544
Control4=IDC_PROGRESS_BAR,msctls_progress32,1350565888
Control5=IDC_BUTTON_RECOVER,button,1476460544

[DLG:IDD_TAB_SECURITYCFG_ACLS_FOLDER]
Type=1
Class=CTabSecurityCfgACLSFolder
ControlCount=5
Control1=IDC_LIST_GROUP,SysListView32,1350631425
Control2=IDC_BUTTON_DETECT,button,1342242816
Control3=IDC_BUTTON_FIX,button,1476460544
Control4=IDC_PROGRESS_BAR,msctls_progress32,1350565888
Control5=IDC_BUTTON_RECOVER,button,1476460544

[CLS:CTabSecurityCfgACLSFile]
Type=0
HeaderFile=tabsecuritycfgaclsfile.h
ImplementationFile=tabsecuritycfgaclsfile.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTabSecurityCfgACLSFile

[CLS:CTabSecurityCfgACLSFolder]
Type=0
HeaderFile=TabSecurityCfgACLSFolder.h
ImplementationFile=TabSecurityCfgACLSFolder.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTabSecurityCfgACLSFolder

[DLG:IDD_DLG_DETAIL]
Type=1
Class=CDlgDetail
ControlCount=2
Control1=IDC_LIST_DETAIL,SysListView32,1350631425
Control2=IDCANCEL,button,1342242816

[CLS:CDlgDetail]
Type=0
HeaderFile=DlgDetail.h
ImplementationFile=DlgDetail.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgDetail
VirtualFilter=dWC

[MNU:IDR_MENU_ACLS_LIST]
Type=1
Class=CTabSecurityCfgObject
Command1=IDM_MENU_LIST_DETAIL
CommandCount=1

[DLG:IDD_TAB_SECURITYCFG_ACLS_ANTIVIRUS]
Type=1
Class=CTabSecurityCfgACLSAntiVirus
ControlCount=5
Control1=IDC_LIST_GROUP,SysListView32,1350631425
Control2=IDC_BUTTON_DETECT,button,1342242816
Control3=IDC_BUTTON_FIX,button,1476460544
Control4=IDC_PROGRESS_BAR,msctls_progress32,1350565888
Control5=IDC_BUTTON_RECOVER,button,1476460544

[CLS:CTabSecurityCfgACLSAntiVirus]
Type=0
HeaderFile=TabSecurityCfgACLSAntiVirus.h
ImplementationFile=TabSecurityCfgACLSAntiVirus.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTabSecurityCfgACLSAntiVirus

[DLG:IDD_TAB_SECURITYCFG_SERVICE]
Type=1
Class=CTabSecurityCfgService
ControlCount=5
Control1=IDC_LIST_GROUP,SysListView32,1350631425
Control2=IDC_BUTTON_DETECT,button,1342242816
Control3=IDC_BUTTON_FIX,button,1476460544
Control4=IDC_PROGRESS_BAR,msctls_progress32,1350565888
Control5=IDC_BUTTON_RECOVER,button,1476460544

[CLS:CTabSecurityCfgService]
Type=0
HeaderFile=TabSecurityCfgService.h
ImplementationFile=TabSecurityCfgService.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabSecurityCfgService
VirtualFilter=dWC

[MNU:IDR_MENU_SVC_LIST]
Type=1
Class=CTabSecurityCfgACLSFolder
Command1=IDM_MENU_LIST_SVC_START
Command2=IDM_MENU_LIST_SVC_STOP
CommandCount=2

[DLG:IDD_DLG_SERVICE]
Type=1
Class=CDlgService
ControlCount=1
Control1=IDC_STATIC_HINT,static,1342308353

[CLS:CDlgService]
Type=0
HeaderFile=DlgService.h
ImplementationFile=DlgService.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgService
VirtualFilter=dWC

[DLG:IDD_TAB_SECURITYCFG_OBJECT]
Type=1
Class=CTabSecurityCfgObject
ControlCount=5
Control1=IDC_LIST_GROUP,SysListView32,1350631425
Control2=IDC_BUTTON_DETECT,button,1342242816
Control3=IDC_BUTTON_FIX,button,1476460544
Control4=IDC_PROGRESS_BAR,msctls_progress32,1350565888
Control5=IDC_BUTTON_RECOVER,button,1476460544

[CLS:CTabSecurityCfgObject]
Type=0
HeaderFile=TabSecurityCfgObject.h
ImplementationFile=TabSecurityCfgObject.cpp
BaseClass=CDialog
Filter=D
LastObject=CTabSecurityCfgObject
VirtualFilter=dWC

[DLG:IDD_DLG_RECOVER_REG]
Type=1
Class=CDlgRecoverReg
ControlCount=4
Control1=IDC_LIST_FILE,SysListView32,1350631425
Control2=IDCANCEL,button,1342242816
Control3=IDC_BUTTON_RECOVER,button,1476460544
Control4=IDC_STATIC_HINT,static,1342308352

[CLS:CDlgRecoverReg]
Type=0
HeaderFile=DlgRecoverReg.h
ImplementationFile=DlgRecoverReg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_STATIC_HINT

[DLG:IDD_DLG_RECOVER_SECURITY]
Type=1
Class=CDlgRecoverSecurity
ControlCount=1
Control1=IDC_STATIC_HINT,static,1342308353

[CLS:CDlgRecoverSecurity]
Type=0
HeaderFile=DlgRecoverSecurity.h
ImplementationFile=DlgRecoverSecurity.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgRecoverSecurity
VirtualFilter=dWC

