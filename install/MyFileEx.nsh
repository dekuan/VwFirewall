!echo "Macro - Upgrade File"
!ifndef ASS_VERBOSE
  !define ASS_VERBOSE 3
!endif

!verbose push
!verbose ${ASS_VERBOSE}

!ifndef FILEEX_INCLUDED

!define FILEEX_INCLUDED

; Macro - Upgrade File
; Written by Joost Verburg
; ------------------------
;
; Parameters:
; LOCALFILE   - Location of the new file (on the compiler system)
; DESTFILE    - Location of the file that should be upgraded (on the user's system)
; TEMPBASEDIR - Directory on the user's system to store a temporary file when the system has
;               to be rebooted.
;               For Win9x support, this should be on the same volume as the DESTFILE!
;               The Windows temp directory could be located on any volume, so you cannot use
;               this directory.
;
; Define UPGRADEFILE_FORCE if you want to upgrade a File that does not have to be registered.
;
; Note: If you want to support Win9x, you can only use short filenames (8.3).
;
; Example of usage:
; !insertmacro FileEx "dllname.dll" "$SYSDIR\dllname.dll" "$INSTDIR"
;

!macro FileEx LOCALFILE DESTFILE TEMPBASEDIR
  !verbose push
  !verbose ${ASS_VERBOSE}

  Push $R0
  Push $R1
  Push $R2
  Push $R3

  ;------------------------
  ;Unique number for labels

  !define UPGRADEFILE_UNIQUE ${__FILE__}${__LINE__}

  ;------------------------
  ;Check file and version

  IfFileExists "${DESTFILE}" 0 upgradefile.copy_${UPGRADEFILE_UNIQUE}

!ifdef UPGRADEFILE_FORCE
  Goto upgradefile.upgrade_${UPGRADEFILE_UNIQUE}
!else
  ClearErrors
  !ifdef UPGRANDEFILE_FILETIME
    GetFileTimeLocal "${LOCALFILE}" $R0 $R1
    GetFileTime "${DESTFILE}" $R2 $R3
  !else
    GetDLLVersionLocal "${LOCALFILE}" $R0 $R1
    GetDLLVersion "${DESTFILE}" $R2 $R3
  !endif

  IfErrors upgradefile.upgrade_${UPGRADEFILE_UNIQUE}

  IntCmpU $R0 $R2 0 upgradefile.done_${UPGRADEFILE_UNIQUE} upgradefile.upgrade_${UPGRADEFILE_UNIQUE}
  IntCmpU $R1 $R3 upgradefile.done_${UPGRADEFILE_UNIQUE} upgradefile.done_${UPGRADEFILE_UNIQUE} \
    upgradefile.upgrade_${UPGRADEFILE_UNIQUE}
 !endif ; UPGRADEFILE_FORCE

  ;------------------------
  ;Let's upgrade the File!

  upgradefile.upgrade_${UPGRADEFILE_UNIQUE}:

  SetOverwrite try

  ;------------------------
  ;Try to copy the File directly

  ClearErrors
    StrCpy $R0 "${DESTFILE}"
    Call :upgradefile.file_${UPGRADEFILE_UNIQUE}
  IfErrors 0 upgradefile.done_${UPGRADEFILE_UNIQUE}

  ;------------------------
  ;File is in use. Copy it to a temp file and Rename it on reboot.

  GetTempFileName $R0 "${TEMPBASEDIR}"
    Call :upgradefile.file_${UPGRADEFILE_UNIQUE}
  Rename /REBOOTOK $R0 "${DESTFILE}"

  Goto upgradefile.done_${UPGRADEFILE_UNIQUE}

  ;------------------------
  ;File does not exist - just extract

  upgradefile.copy_${UPGRADEFILE_UNIQUE}:
    StrCpy $R0 "${DESTFILE}"
    Call :upgradefile.file_${UPGRADEFILE_UNIQUE}

  ;------------------------
  ;Done

  upgradefile.done_${UPGRADEFILE_UNIQUE}:

  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0

  ;------------------------
  ;End

  Goto upgradefile.end_${UPGRADEFILE_UNIQUE}

  ;------------------------
  ;Called to extract the File

  upgradefile.file_${UPGRADEFILE_UNIQUE}:
    File /oname=$R0 "${LOCALFILE}"
    Return

  upgradefile.end_${UPGRADEFILE_UNIQUE}:

 ;------------------------
 ;Restore settings

 SetOverwrite lastused

 !undef UPGRADEFILE_UNIQUE

 !verbose pop
!macroend

!endif
!verbose pop