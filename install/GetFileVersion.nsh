;Function Description
;GetFileVersion
;Thanks KiCHiK (Based on his example for command "GetDLLVersion")
; 
; 
;Get version information from executable file.
; 
; 
;Syntax:
;${GetFileVersion} "[Executable]" $var
; 
;"[Executable]"      ; Executable file (*.exe *.dll ...)
;$var                ; Result: Version number
; 
; 
;Note:
;-Error flag if file doesn't exist
;-Error flag if file doesn't contain version information
;
;
;Example: 
;
;Section
;	${GetFileVersion} "C:\ftp\program.exe" $R0
;	; $R0="1.1.0.12"
;SectionEnd
;
;

Function GetFileVersion
	!define GetFileVersion `!insertmacro GetFileVersionCall`

	!macro GetFileVersionCall _FILE _RESULT
		Push `${_FILE}`
		Call GetFileVersion
		Pop ${_RESULT}
	!macroend

	Exch $0
	Push $1
	Push $2
	Push $3
	Push $4
	Push $5
	Push $6
	ClearErrors

	GetDllVersion '$0' $1 $2
	IfErrors error
	IntOp $3 $1 >> 16
	IntOp $3 $3 & 0x0000FFFF
	IntOp $4 $1 & 0x0000FFFF
	IntOp $5 $2 >> 16
	IntOp $5 $5 & 0x0000FFFF
	IntOp $6 $2 & 0x0000FFFF
	StrCpy $0 '$3.$4.$5.$6'
	goto end

	error:
	SetErrors
	StrCpy $0 ''

	end:
	Pop $6
	Pop $5
	Pop $4
	Pop $3
	Pop $2
	Pop $1
	Exch $0
FunctionEnd

Function GetFileVersion_IsNeedUpdate
         ;
         ; ${GetFileVersion_IsNeedUpdate} "sInsFile" "sDestFile"
         ; ${GetFileVersion_IsNeedUpdate} "$INSTDIR\DeLib.dll" "$SYSDIR\DeLib.dll" $g_bDllNeedUpdate
         ;
         ; sInsFile
         ; sDestFile
         ;

         ;------------------------
         ;Unique number for labels
         !define GETFILEVERSION_UNIQUE ${__FILE__}${__LINE__}

	!define GetFileVersion_IsNeedUpdate `!insertmacro GetFileVersion_IsNeedUpdateCall`

	!macro GetFileVersion_IsNeedUpdateCall _INS_FILE _DEST_FILE _RESULT
		Push `${_INS_FILE}`
		Push `${_DEST_FILE}`
		Call GetFileVersion_IsNeedUpdate
		Pop ${_RESULT}
	!macroend

	Pop $1  ;  sDestFile
	Pop $0  ;  sInsFile
        Push $0
        Push $1

	Push $R0
	Push $R1
	Push $R2
	Push $R3

	ClearErrors

        ; get file version of InsFile
	GetDllVersion '$0' $R0 $R1

	; get file version of DestFile
	GetDllVersion '$1' $R2 $R3

	IfErrors GetFileVersion_IsNeedUpdate.error_${GETFILEVERSION_UNIQUE}

        ; return value
        StrCpy $0 "0"

        ; IntCmpU
        ; val1 val2 jump_if_equal [jump_if_val1_less] [jump_if_val1_more]
        IntCmpU $R0 $R2 0 GetFileVersion_IsNeedUpdate.done_${GETFILEVERSION_UNIQUE} GetFileVersion_IsNeedUpdate.upgrade_${GETFILEVERSION_UNIQUE}
        IntCmpU $R1 $R3 GetFileVersion_IsNeedUpdate.done_${GETFILEVERSION_UNIQUE} GetFileVersion_IsNeedUpdate.done_${GETFILEVERSION_UNIQUE} \
                GetFileVersion_IsNeedUpdate.upgrade_${GETFILEVERSION_UNIQUE}

;  need update
GetFileVersion_IsNeedUpdate.upgrade_${GETFILEVERSION_UNIQUE}:
        StrCpy $0 "1"
        goto GetFileVersion_IsNeedUpdate.done_${GETFILEVERSION_UNIQUE}


;  process error
GetFileVersion_IsNeedUpdate.error_${GETFILEVERSION_UNIQUE}:
	SetErrors
	StrCpy $0 "0"

;  done
GetFileVersion_IsNeedUpdate.done_${GETFILEVERSION_UNIQUE}:
        Pop $R3
        Pop $R2
        Pop $R1
        Pop $R0
	Exch $0

FunctionEnd