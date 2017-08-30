'***********************************************************************************
'*
'* File:		XCACLS.VBS
'* Created:		April 18, 2001
'* Last Modified:	June 24, 2004
'* Version:		5.2
'*
'* Main Function:  	List/Change ACLS for files and directories
'*
'*
'* Copyright (C) 2001 Microsoft Corporation
'* 
'*
'***********************************************************************************

OPTION EXPLICIT

'********************************************************************
'* Declare main variables
'********************************************************************

    CONST CONST_CurrentBuild          	= "5.2"

    Dim intOpMode, blnQuiet, strOutputFile, objOutputFile, debug_on, timewmi_on
    Dim f_Used, s_Used, t_Used, e_Used, g_Used, r_used
    Dim p_Used, d_used, i_used, o_used, filename_var
    Dim l_Used, q_Used, debug_Used, timewmi_Used, strDefaultDomain, strSystemDomainSid, strSystemDomainName, intPermUpdateCount
    Dim g_var_User(), ObjTrustee_g_var_User(), g_Var_Perm(), g_Var_Spec(), g_Var_PermBit(), g_Var_SpecBit()
    dim r_Var_User(), ObjTrustee_r_var_User()
    Dim p_var_User(), ObjTrustee_p_var_User(), p_Var_Perm(), p_Var_Spec(), p_Var_PermBit(), p_Var_SpecBit()
    Dim d_Var_User(), ObjTrustee_d_var_User(), d_Var_Perm(), d_Var_Spec(), d_Var_PermBit(), d_Var_SpecBit()
    ReDim g_var_User(0), ObjTrustee_g_var_User(0), g_Var_Perm(0), g_Var_Spec(0), g_Var_PermBit(0), g_Var_SpecBit(0)
    Redim r_Var_User(0), ObjTrustee_r_var_User(0)
    ReDim p_var_User(0), ObjTrustee_p_var_User(0), p_Var_Perm(0), p_Var_Spec(0), p_Var_PermBit(0), p_Var_SpecBit(0)
    ReDim d_Var_User(0), ObjTrustee_d_var_User(0), d_Var_Perm(0), d_Var_Spec(0), d_Var_PermBit(0), d_Var_SpecBit(0)
    Dim i_Var, o_Var
    Dim spec_Used, spec_Var
    Dim ObjTrustee_o_var_User, OldDaclParentObj(), strOldDaclParentPath, strOldDaclLastIsItAFolderValue, boolOldDACLParentRevokedUserFound
    Dim fso, InitialfilenameAbsPath, QryBaseNameHasWildcards, QryExtensionHasWildcards
    Dim objService, objLocalService, objLocator
    Dim strRemoteServerName, strRemoteShareName, strRemoteUserName, strRemotePassword
    Dim RemoteServer_Used, RemoteUserName_Used
    Dim DisplayDirPath, ActualDirPath
    Dim BoolUsingCScript
    Dim endTime, startTime
    Dim Global_bool_SID_Used
    

    'This const value is for any use referenced without a domain, if this is TRUE, we will use the local machine name
    'for the domain if its a non-dc. For DC's we will always use the Domain name unless you specify the actual domain to use.
    'If this is FALSE, we will default to the Domain name.

    CONST CONST_USE_LOCAL_FOR_NON_DCs          	= TRUE

    'These are specific to this Script 
    CONST CONST_SHOW_USAGE              	= 3
    CONST CONST_PROCEED                 		= 4 
    CONST CONST_ERROR	                 		= 1

    'When working with NTFS Security, we use constants that match the API documentation
    '********************* ControlFlags *********************
    CONST ALLOW_INHERIT  			= 33796		'Used in ControlFlag to turn on Inheritance
								'Same as: 
								'SE_SELF_RELATIVE + SE_DACL_AUTO_INHERITED + SE_DACL_PRESENT
    CONST DENY_INHERIT   			= 37892		'Used in ControlFlag to turn off Inheritance
								'Same as: 
								'SE_SELF_RELATIVE + SE_DACL_PROTECTED + SE_DACL_AUTO_INHERITED + SE_DACL_PRESENT
    Const SE_OWNER_DEFAULTED 			= 1		'A default mechanism, rather than the the original provider of the security 
								'descriptor, provided the security descriptor's owner security identifier (SID). 

    Const SE_GROUP_DEFAULTED 			= 2		'A default mechanism, rather than the the original provider of the security
								'descriptor, provided the security descriptor's group SID. 

    Const SE_DACL_PRESENT 				= 4		'Indicates a security descriptor that has a DACL. If this flag is not set, 
								'or if this flag is set and the DACL is NULL, the security descriptor allows 
								'full access to everyone.

    Const SE_DACL_DEFAULTED 			= 8		'Indicates a security descriptor with a default DACL. For example, if an 
								'object's creator does not specify a DACL, the object receives the default 
								'DACL from the creator's access token. This flag can affect how the system 
								'treats the DACL, with respect to ACE inheritance. The system ignores this 
								'flag if the SE_DACL_PRESENT flag is not set. 

    Const SE_SACL_PRESENT 				= 16		'Indicates a security descriptor that has a SACL. 

    Const SE_SACL_DEFAULTED 			= 32		'A default mechanism, rather than the the original provider of the security 
								'descriptor, provided the SACL. This flag can affect how the system treats 
								'the SACL, with respect to ACE inheritance. The system ignores this flag if 
								'the SE_SACL_PRESENT flag is not set. 

    Const SE_DACL_AUTO_INHERIT_REQ 	= 256		'Requests that the provider for the object protected by the security descriptor 
								'automatically propagate the DACL to existing child objects. If the provider 
								'supports automatic inheritance, it propagates the DACL to any existing child 
								'objects, and sets the SE_DACL_AUTO_INHERITED bit in the security descriptors 
								'of the object and its child objects.

    Const SE_SACL_AUTO_INHERIT_REQ 		= 512		'Requests that the provider for the object protected by the security descriptor 
								'automatically propagate the SACL to existing child objects. If the provider 
								'supports automatic inheritance, it propagates the SACL to any existing child 
								'objects, and sets the SE_SACL_AUTO_INHERITED bit in the security descriptors of 
								'the object and its child objects.

    Const SE_DACL_AUTO_INHERITED 		= 1024		'Windows 2000 only. Indicates a security descriptor in which the DACL is set up 
								'to support automatic propagation of inheritable ACEs to existing child objects. 
								'The system sets this bit when it performs the automatic inheritance algorithm 
								'for the object and its existing child objects. This bit is not set in security 
								'descriptors for Windows NT versions 4.0 and earlier, which do not support 
								'automatic propagation of inheritable ACEs.

    Const SE_SACL_AUTO_INHERITED 		= 2048		'Windows 2000: Indicates a security descriptor in which the SACL is set up to 
								'support automatic propagation of inheritable ACEs to existing child objects. 
								'The system sets this bit when it performs the automatic inheritance algorithm 
								'for the object and its existing child objects. This bit is not set in security 
								'descriptors for Windows NT versions 4.0 and earlier, which do not support automatic 
								'propagation of inheritable ACEs.

    Const SE_DACL_PROTECTED 			= 4096		'Windows 2000: Prevents the DACL of the security descriptor from being modified 
								'by inheritable ACEs. 

    Const SE_SACL_PROTECTED 				= 8192		'Windows 2000: Prevents the SACL of the security descriptor from being modified 
								'by inheritable ACEs. 

    Const SE_SELF_RELATIVE 				= 32768		'Indicates a security descriptor in self-relative format with all the security 
								'information in a contiguous block of memory. If this flag is not set, the security 
								'descriptor is in absolute format. For more information, see Absolute and 
								'Self-Relative Security Descriptors in the Platform SDK topic Low-Level Access-Control.

    '********************* ACE Flags *********************
    CONST OBJECT_INHERIT_ACE  			= 1 	'Noncontainer child objects inherit the ACE as an effective ACE. For child 
							'objects that are containers, the ACE is inherited as an inherit-only ACE 
							'unless the NO_PROPAGATE_INHERIT_ACE bit flag is also set.

    CONST CONTAINER_INHERIT_ACE 		= 2 	'Child objects that are containers, such as directories, inherit the ACE
							'as an effective ACE. The inherited ACE is inheritable unless the 
							'NO_PROPAGATE_INHERIT_ACE bit flag is also set.  

    CONST NO_PROPAGATE_INHERIT_ACE 	= 4 	'If the ACE is inherited by a child object, the system clears the 
							'OBJECT_INHERIT_ACE and CONTAINER_INHERIT_ACE flags in the inherited ACE. 
							'This prevents the ACE from being inherited by subsequent generations of objects.  

    CONST INHERIT_ONLY_ACE	 			= 8 	'Indicates an inherit-only ACE which does not control access to the object
							'to which it is attached. If this flag is not set, the ACE is an effective
							'ACE which controls access to the object to which it is attached. Both 
							'effective and inherit-only ACEs can be inherited depending on the state of
							'the other inheritance flags. 

    CONST INHERITED_ACE		 			= 16 	'Windows NT 5.0 and later, Indicates that the ACE was inherited. The system sets
							'this bit when it propagates an inherited ACE to a child object. 

    CONST ACEFLAG_VALID_INHERIT_FLAGS = 31 	'Indicates whether the inherit flags are valid.  


    'Two special flags that pertain only to ACEs that are contained in a SACL are listed below. 

    CONST SUCCESSFUL_ACCESS_ACE_FLAG 	= 64 	'Used with system-audit ACEs in a SACL to generate audit messages for successful
							'access attempts. 

    CONST FAILED_ACCESS_ACE_FLAG 		= 128 	'Used with system-audit ACEs in a SACL to generate audit messages for failed
							'access attempts. 

    '********************* ACE Types *********************
    CONST ACCESS_ALLOWED_ACE_TYPE 	= 0 	'Used with Win32_Ace AceTypes
    CONST ACCESS_DENIED_ACE_TYPE 		= 1 	'Used with Win32_Ace AceTypes
    CONST AUDIT_ACE_TYPE 				= 2 	'Used with Win32_Ace AceTypes


    '********************* Access Masks *********************

    Dim Perms_LStr, Perms_SStr, Perms_Const
    'Permission LongNames
    Perms_LStr=Array("Synchronize"			, _
		"Take Ownership"					, _
		"Change Permissions"				, _
		"Read Permissions"					, _
		"Delete"							, _
		"Write Attributes"					, _
		"Read Attributes"					, _
		"Delete Subfolders and Files"			, _
		"Traverse Folder / Execute File"		, _
		"Write Extended Attributes"			, _
		"Read Extended Attributes"			, _
		"Create Folders / Append Data"		, _
		"Create Files / Write Data"			, _
		"List Folder / Read Data"	)
    'Permission Single Character codes
    Perms_SStr=Array("E"		, _
		"D"		, _
		"C"		, _
		"B"		, _
		"A"		, _
		"9"		, _
		"8"		, _
		"7"		, _
		"6"		, _
		"5"		, _
		"4"		, _
		"3"		, _
		"2"		, _
		"1"		)
    'Permission Integer
    Perms_Const=Array(&H100000	, _
		&H80000		, _
		&H40000		, _
		&H20000		, _
		&H10000		, _
		&H100		, _
		&H80		, _
		&H40		, _
		&H20		, _
		&H10		, _
		&H8			, _
		&H4			, _
		&H2			, _
		&H1		)

   Dim OverLook_Perms_Const
    'Permission Integer
    OverLook_Perms_Const=Array(&H80000000			, _
    		&H40000000									, _
    		&H20000000									, _
    		&H10000000									)

	startTime = Timer
    

    'Initializing Default values
    f_Used = FALSE
    s_Used = FALSE
    t_Used = FALSE
    e_Used = FALSE
    g_Used = FALSE
    r_used = FALSE
    p_Used = FALSE
    d_used = FALSE
    i_used = FALSE
    l_Used = FALSE
    q_Used = FALSE
    spec_Used = FALSE
    i_var = 0 'If this is 3 anywhere, then it was set to remove inheritance...default is to keep inheritance if present.
    spec_var = OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE
    RemoteServer_Used = FALSE
    strRemoteServerName = ""
    strRemoteShareName = ""
    RemoteUserName_Used = FALSE
    strRemoteUserName = ""
    strRemotePassword = ""
    debug_Used = FALSE	'Parameter Passed
    timewmi_Used = FALSE	'Parameter Passed
    filename_var = ""
    DisplayDirPath = ""
    ActualDirPath = ""
    Global_bool_SID_Used = False	'This changes to True if any of the users entered are an Explicit SID, and then Usermatch will not check Domain/Username matches.
    boolOldDACLParentRevokedUserFound = False
    strOldDaclParentPath = ""
    ReDim OldDaclParentObj(0)

    debug_on = FALSE	'Actual value checked in script
    timewmi_on = False 'Actual value checked in script, used for timing WMI use
    blnQuiet = FALSE
    strOutputFile = "XCACLS.Log"

    BoolUsingCScript = IsEngineCScript()

    'Parse the command line
    intOpMode = intParseCmdLine()
    If Err.Number Then
	Wscript.Echo "(101) Error while parsing the command line." & vbcrlf & "Error " & Err.Number & ": " & Err.Description
	WScript.Quit
    End if

    'Open the output file so we can use it through out the script
    If l_Used then Call OpenOutputFile()

    Call PrintMsg("Starting XCACLS.VBS (Version: " & CONST_CurrentBuild & ") Script at " & now)


    'FSO is used in several funcitons, so lets set it globally.
    Set fso = WScript.CreateObject("Scripting.FileSystemObject")
    If blnErrorOccurred(" occurred in getting FileSystemObject. (Msg#102)") Then WScript.Quit

    'Put statements in loop to be able to drop out and clear variables
    Do
	If debug_on then Call PrintMsg("Main: Enter")

	'Lets get to the work to be done...
	If Not IsOSSupported() then Exit Do

	Call PrintArguments()	'Show the arguments entered

	'Now lets do the work based upon the arguments entered.
	Select Case intOpMode
	Case CONST_SHOW_USAGE
       		Call ShowUsage()
	Case CONST_PROCEED
		'Lets get the objService object which is used throughout the script

		If Not SetMainVars() then Exit Do

		Call PrintMsg("")
		If g_Used  or r_Used or p_Used or d_Used or o_used then
			If NOT CheckTrustees() then
				Call PrintMsg("Error: No Groups or Names entered were found, exiting script.")
				Exit Do
			End if
		End if

		If QryBaseNameHasWildcards or QryExtensionHasWildcards then
			If debug_on then Call PrintMsg("Wildcard characters detected in """ & DisplayPathString(InitialfilenameAbsPath) & """")
			Select Case DoesPathNameExist(fso.GetParentFolderName(InitialfilenameAbsPath))
			Case 1 'Directory
				Call DoTheWorkOnEverythingUnderDirectory(fso.GetParentFolderName(InitialfilenameAbsPath))
			Case Else
				Call PrintMsg("Error: Directory """ & DisplayPathString(fso.GetParentFolderName(InitialfilenameAbsPath)) & """ not found.")
			End select
		Else
			If debug_on then Call PrintMsg("No Wildcard characters detected for """ & DisplayPathString(filename_var) & """")
			'If a folder is found with the same name, then we work it as a folder and include files under it.
			Select Case DoesPathNameExist(InitialfilenameAbsPath)
			Case 1 'Directory
				Call DoTheWorkOnThisItem(InitialfilenameAbsPath, TRUE)
				If t_used or f_Used or s_Used then
					Call DoTheWorkOnEverythingUnderDirectory(InitialfilenameAbsPath)
				End if
			Case 2 'File
				Call DoTheWorkOnThisItem(InitialfilenameAbsPath, FALSE)
			Case Else
				Call PrintMsg("Error: File/Directory """ & DisplayPathString(InitialfilenameAbsPath) & """ not found.")
			End select
		End if
	Case Else
		Call PrintMsg("")
		Call PrintMsg(intOpMode)
	End Select

	Call blnErrorOccurred(" occurred while in the main routine of the script. (Msg#103)")
	If debug_on then Call PrintMsg("Main: Exit")

	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objService = Nothing
    Set objLocalService = Nothing
    Set objLocator = Nothing
    Call ClearObjectArray(ObjTrustee_g_var_User)
    Call ClearObjectArray(ObjTrustee_r_var_User)
    Call ClearObjectArray(ObjTrustee_p_var_User)
    Call ClearObjectArray(ObjTrustee_d_var_User)
    Call ClearObjectArray(OldDaclParentObj)
    Set ObjTrustee_o_var_User = Nothing

    Call PrintMsg("")
    Call PrintMsg("")

	endTime = Timer
	call PrintMsg("Operation Complete" & vbCrLf & "Elapsed Time: " & (endTime - startTime) & " seconds.")

    Call PrintMsg("")
    Call PrintMsg("Ending Script at " & now)
    Call PrintMsg("")
    Call PrintMsg("")

    If l_Used then
	If strOutputFile <> "" Then
		objOutputFile.Close
	End if
    End if

'********************************************************************
'* End of Main Script
'********************************************************************


'********************************************************************
'*
'* Sub DoTheWorkOnEverythingUnderDirectory()
'* Purpose: Work on Directory path passed to it, and pass paths to DoTheWorkOnThisItem sub
'* Input:   ThisPath - Path to directory
'* Output:  None
'* Notes:   This sub will process every file and folder under the directory passed to it.
'*
'********************************************************************

Sub DoTheWorkOnEverythingUnderDirectory(byval ThisPath)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("DoTheWorkOnEverythingUnderDirectory: Enter")

    Dim objFileSystemSet, objPath, objFileSystemSet2, objPath2, strQuery, strTempPath, booltempItsAFolder
    Dim f, f1, fc

    Do
	If debug_on then Call PrintMsg("DoTheWorkOnEverythingUnderDirectory: Directory passed: """ & ThisPath & """")

	'We already checked for existance so we will assume it exists.

	If RemoteServer_Used then
		strQuery = "Select Name, Drive, Path, FileName from Cim_LogicalFile Where Name=""" & Replace(ThisPath,"\","\\") & """"
        	Set objFileSystemSet = objService.ExecQuery(strQuery,,0)
		If blnErrorOccurred(" occurred setting objFileSystemSet = objService.ExecQuery(" & strQuery & ",,0). (Msg#201)") Then Exit Do

		strTempPath = ""
    		for each objPath in objFileSystemSet
			If objPath.Drive <> "" then
				strTempPath = objPath.Path & objPath.FileName & "\"
				strTempPath = Replace(strTempPath, "\\", "\")
				Exit For
			End if
	    	next

		strQuery = "Select Name, Drive, Path, FileName from Cim_LogicalFile Where Path=""" & Replace(strTempPath,"\","\\") & """"
        	Set objFileSystemSet2 = objService.ExecQuery(strQuery,,0)
		If blnErrorOccurred(" occurred setting objFileSystemSet2 = objService.ExecQuery(" & strQuery & ",,0). (Msg#202)") Then Exit Do

	    	for each objPath2 in objFileSystemSet2
			strTempPath = ""
			booltempItsAFolder = False
			If objPath2.Drive <> "" then
				If UCASE(objPath2.FileType) = "FILE FOLDER" then booltempItsAFolder = True
				strTempPath = objPath2.Name
				If QryBaseNameHasWildcards Or QryExtensionHasWildcards then
					If DoesItMatch(strTempPath) then
						If g_Used  or r_Used or p_Used or d_Used or o_used or i_used then 
							If booltempItsAFolder then
								If s_Used then Call DoTheWorkOnThisItem(strTempPath, booltempItsAFolder)
							Else
								If f_Used then Call DoTheWorkOnThisItem(strTempPath, booltempItsAFolder)
							End if
						Else
							Call DoTheWorkOnThisItem(strTempPath, booltempItsAFolder)
						End if
					End if
					If booltempItsAFolder then
						If t_used then Call DoTheWorkOnEverythingUnderDirectory(strTempPath)
					End if
				Else
					If booltempItsAFolder then
						If s_Used then
							Call DoTheWorkOnThisItem(strTempPath, booltempItsAFolder)
						End if 
						If t_used then 
							Call DoTheWorkOnEverythingUnderDirectory(strTempPath)
						End if
					Else
						If f_Used then 
							Call DoTheWorkOnThisItem(strTempPath, booltempItsAFolder)
						End if
					End if
				End if
			End if
		next
	Else
		Set f = fso.GetFolder(ThisPath)

		If blnErrorOccurred(" occurred in getting FileSystemObject.GetFolder (Msg#203)") Then Exit Do

		Set fc = f.Files	
		For Each f1 in fc
			If QryBaseNameHasWildcards Or QryExtensionHasWildcards then
				If DoesItMatch(f1.Path) then
					If g_Used  or r_Used or p_Used or d_Used or o_used or i_used then 
						If f_Used then Call DoTheWorkOnThisItem(f1.Path, False)
					Else
						Call DoTheWorkOnThisItem(f1.Path, False)
					End if
				End if
			Else
				If f_Used then Call DoTheWorkOnThisItem(f1.Path, False)
			End if
		Next
		Set fc = Nothing

		Set fc = f.SubFolders	

		For Each f1 in fc
			If QryBaseNameHasWildcards Or QryExtensionHasWildcards then
				If DoesItMatch(f1.Path) then
					If g_Used  or r_Used or p_Used or d_Used or o_used or i_used then 
						If s_Used then Call DoTheWorkOnThisItem(f1.Path, True)
					Else
						Call DoTheWorkOnThisItem(f1.Path, True)
					End if
				End if
				If t_used then Call DoTheWorkOnEverythingUnderDirectory(f1.Path)
			Else
				If s_used then Call DoTheWorkOnThisItem(f1.Path, True)
				If t_used then Call DoTheWorkOnEverythingUnderDirectory(f1.Path)
			End if
		Next
		Set fc = Nothing
	End if

	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set f = Nothing
    Set fc = Nothing
    Set f1 = Nothing
    Set objPath = Nothing
    Set objFileSystemSet = Nothing
    Set objPath2 = Nothing
    Set objFileSystemSet2 = Nothing

    Call blnErrorOccurred(" occurred while in the DoTheWorkOnEverythingUnderDirectory routine. (Msg#204)")
    If debug_on then Call PrintMsg("DoTheWorkOnEverythingUnderDirectory: Exit")
End Sub

'********************************************************************
'*
'* Sub DoTheWorkOnThisItem()
'* Purpose: Work on File/Folder passed to it, and pass to Work routine
'* Input:   ABSPath - Path to File/Folder
'* Output:  TRUE if Successful, FALSE if not
'*
'********************************************************************

Sub DoTheWorkOnThisItem(byval AbsPath, byval IsItAFolder)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("DoTheWorkOnThisItem: Enter")

    Dim DisplayIt

    Do
	DisplayIt = TRUE

	Call PrintMsg("")
	Call PrintMsg("**************************************************************************")
	If IsItAFolder then
		Call PrintMsg("Directory: " & DisplayPathString(AbsPath))
	Else
		Call PrintMsg("File: " & DisplayPathString(AbsPath))
	End if
	'We already checked for existance so we will assume it exists.
	If g_Used  or r_Used or p_Used or d_Used or o_used or i_used then 
		Call SetACLForObject(AbsPath, IsItAFolder)
		DisplayIt = FALSE
	End If
	If DisplayIt then 
		Call DisplayThisACL(AbsPath)
	End if
	Call PrintMsg("**************************************************************************")
	Exit Do
    Loop

    Call blnErrorOccurred(" occurred while in the DoTheWorkOnThisItem routine. (Msg#301)")
    If debug_on then Call PrintMsg("DoTheWorkOnThisItem: Exit")

End Sub

'********************************************************************
'*
'* Sub DisplayThisACL()
'* Purpose: Shows ACL's that are applied to strPath
'* Input:   strPath - string containing path of file or folder, ShowLong - If TRUE, permissions are in long form
'* Output:  prints the acls
'*
'********************************************************************

Sub DisplayThisACL(byval strPath)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("DisplayThisACL: Enter")

    Dim objFileSecSetting, objOutParams, objSecDescriptor, objOwner, objDACL_Member
    Dim objtrustee, numAceFlags, strAceFlags, x, strAceType, numControlFlags, ReturnAceFlags, TempSECString
    ReDim arraystrACLS(0)

    'Put statements in loop to be able to drop out and clear variables
    Do
	set objFileSecSetting = objService.Get("Win32_LogicalFileSecuritySetting.Path=""" & Replace(strPath,"\","\\") & """")
	If blnErrorOccurred(" occurred setting Win32_LogicalFileSecuritySetting object. (Msg#401)") Then 
		If debug_on then 
			Call PrintMsg("DisplayThisACL: objService.Get query that failed:")
			Call PrintMsg("DisplayThisACL:           Win32_LogicalFileSecuritySetting.Path=""" & Replace(strPath,"\","\\") & """")
		end if
		Exit Do
	End if

	Set objOutParams = objFileSecSetting.ExecMethod_("GetSecurityDescriptor")
	If blnErrorOccurred(" occurred when this command was issued: GetSecurityDescriptor. (Msg#402)") Then Exit Do

	set objSecDescriptor = objOutParams.Descriptor
	If blnErrorOccurred(" occurred setting objSecDescriptor = objOutParams.Descriptor. (Msg#403)") Then Exit Do

	numControlFlags = objSecDescriptor.ControlFlags

	If IsArray(objSecDescriptor.DACL) then
		Call PrintMsg("")
		Call PrintMsg("Permissions:")
		Call PrintMsg( strPackString("Type", 9, 1, TRUE) & strPackString("Username", 24, 1, TRUE) & strPackString("Permissions", 22, 1, TRUE) & strPackString("Inheritance", 22, 1, TRUE))
		For Each objDACL_Member in objSecDescriptor.DACL
			TempSECString = ""
			ReturnAceFlags = 0
			Select Case objDACL_Member.AceType
			Case ACCESS_ALLOWED_ACE_TYPE
				strAceType = "Allowed"
			Case ACCESS_DENIED_ACE_TYPE
				strAceType = "Denied"
			Case else
				strAceType = "Unknown"
			End select
			Set objtrustee = objDACL_Member.Trustee
			numAceFlags = objDACL_Member.AceFlags
			strAceFlags = StringAceFlag(numAceFlags, numControlFlags, SE_DACL_AUTO_INHERITED, FALSE, ReturnAceFlags)
			TempSECString = SECString(objDACL_Member.AccessMask,TRUE)
			If ReturnAceFlags = 2 then
				If TempSECString = "Read and Execute" then
					TempSECString = "List Folder Contents"
				End if
			End if
			Call AddStringToArray(arraystrACLS, strPackString(strAceType, 9, 1, TRUE) & strPackString(objtrustee.Domain & "\" & objtrustee.Name, 24, 1, TRUE) & strPackString(TempSECString, 22, 1, TRUE) & strPackString(strAceFlags, 22, 1, TRUE),-1)
			Set objtrustee = Nothing
		Next
		For x = LBound(arraystrACLS) to UBound(arraystrACLS)
			Call PrintMsg(arraystrACLS(x))
		Next 
	Else
		Call PrintMsg("")
		Call PrintMsg("No Permissions set")
	End if
	ReDim arraystrACLS(0)
	If IsArray(objSecDescriptor.SACL) then
		Call PrintMsg("")
		Call PrintMsg("Auditing:")
		Call PrintMsg(strPackString("Type", 9, 1, TRUE) & strPackString("Username", 24, 1, TRUE) & strPackString("Access", 22, 1, TRUE) & strPackString("Inheritance", 22, 1, TRUE))
		For Each objDACL_Member in objSecDescriptor.SACL
			TempSECString = ""
			ReturnAceFlags = 0
			Set objtrustee = objDACL_Member.Trustee
			numAceFlags = objDACL_Member.AceFlags
			strAceType = StringSACLAceFlag(numAceFlags)
			strAceFlags = StringAceFlag(numAceFlags, numControlFlags, SE_SACL_AUTO_INHERITED, FALSE, ReturnAceFlags)
			TempSECString = SECString(objDACL_Member.AccessMask,TRUE)
			If ReturnAceFlags = 2 then
				If TempSECString = "Read and Execute" then
					TempSECString = "List Folder Contents"
				End if
			End if
			Call AddStringToArray(arraystrACLS, strPackString(strAceType, 9, 1, TRUE) & strPackString(objtrustee.Domain & "\" & objtrustee.Name, 24, 1, TRUE) & strPackString(TempSECString, 22, 1, TRUE) & strPackString(strAceFlags, 22, 1, TRUE),-1)
			Set objtrustee = Nothing
		Next
		For x = LBound(arraystrACLS) to UBound(arraystrACLS)
			Call PrintMsg(arraystrACLS(x))
		Next 
	Else
		Call PrintMsg("")
		Call PrintMsg("No Auditing set")
	End if

	Set objOwner = objSecDescriptor.Owner
	If blnErrorOccurred(" occurred setting objOwner = objSecDescriptor.Owner. (Msg#404)") Then Exit Do
	Call PrintMsg("")
	Call PrintMsg("Owner: " & objOwner.Domain & "\" & objOwner.Name)

	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objOwner = Nothing
    Set objSecDescriptor = Nothing
    Set objDACL_Member = Nothing
    Set objtrustee = Nothing
    Set objOutParams = Nothing
    Set objFileSecSetting = Nothing

    Call blnErrorOccurred(" occurred while in the DisplayThisACL routine. (Msg#405)")
    If debug_on then Call PrintMsg("DisplayThisACL: Exit")

End Sub


'********************************************************************
'*
'* Sub GetParentDACLArray()
'* Purpose: Gets the DACL array for the objTempFileSecSetting sent
'* Input:   objTempFileSecSetting
'* Output:  None
'*
'********************************************************************

Sub GetParentDACLArray(ByVal objTempFileSecSetting, byval IsItAFolder)
    ON ERROR RESUME NEXT

    Dim NewobjDescriptor, objmethod, objInParam, RetVal, objOutParams, objSecDescriptor
    Dim i, t, TempParentArray(), boolSkipIt, strCurrentPath

    If debug_on then Call PrintMsg("GetParentDACLArray: Enter")

    'Put statements in loop to be able to drop out and clear variables
    Do
    		strCurrentPath = fso.GetParentFolderName(objTempFileSecSetting.Path)
    		If strOldDaclParentPath <> "" then
    			'If they match then don't redue the parent array
    			If StrComp(strCurrentPath, strOldDaclParentPath, 1) = 0 then
    				'Finally we check to see if the last IsItAFolder is the same as the current one, this allows for this list to be rebuilt if the last value was a folder and the current is a file. 
    				'Possibly would have different Inherited rights if its not the same.
    				If strOldDaclLastIsItAFolderValue = IsItAFolder then Exit Do
    			End if
    		End if
    		strOldDaclLastIsItAFolderValue = IsItAFolder
    		strOldDaclParentPath = strCurrentPath
    		Redim TempParentArray(0)
    		Redim OldDaclParentObj(0)

		If debug_on then Call PrintMsg("GetParentDACLArray: We are getting Inherited ACL's from Parent Directory")

		Set NewobjDescriptor = objService.Get("Win32_SecurityDescriptor").Spawninstance_
		If blnErrorOccurred(" occurred Setting NewobjDescriptor = objService.Get(""Win32_SecurityDescriptor"").Spawninstance_ (Msg#4301)") Then Exit Do

		NewobjDescriptor.ControlFlags =  ALLOW_INHERIT
		If blnErrorOccurred(" occurred setting  NewobjDescriptor.ControlFlags =  ALLOW_INHERIT (Msg#4302)") Then Exit Do

		Set objmethod = objTempFileSecSetting.Methods_("SetSecurityDescriptor")
		If blnErrorOccurred(" occurred setting objmethod = objFileSecSetting.Methods_(""SetSecurityDescriptor"") (Msg#4303)") Then Exit Do

		Set objInParam = objmethod.inParameters.SpawnInstance_()
		If blnErrorOccurred(" occurred Setting objInParam = objmethod.inParameters.SpawnInstance_() (Msg#4304)") Then Exit Do

		objInParam.Properties_.item("Descriptor") = NewobjDescriptor
		If blnErrorOccurred(" occurred setting objInParam.Properties_.item(""Descriptor"") = NewobjDescriptor (Msg#4305)") Then Exit Do

		Set RetVal = objTempFileSecSetting.ExecMethod_("SetSecurityDescriptor", objInParam)	
		If blnErrorOccurred(" occurred setting RetVal = objFileSecSetting.ExecMethod_(""SetSecurityDescriptor"", objInParam) (Msg#4306)") Then Exit Do

		If RetVal.ReturnValue > 0 then
			Call PrintMsg("Error: (Msg#4307) Return Value = " & RetVal.ReturnValue)
			Exit Do
		End if

		'Now we need to get only the Inherited ACE's (Everyone group may be set if DACL array was empty)
		Set objOutParams = objTempFileSecSetting.ExecMethod_("GetSecurityDescriptor")
		If blnErrorOccurred(" occurred Setting objOutParams = objFileSecSetting.ExecMethod_(""GetSecurityDescriptor"") (Msg#4308)") Then Exit Do

		set objSecDescriptor = objOutParams.Descriptor
		If blnErrorOccurred(" occurred setting objSecDescriptor = objOutParams.Descriptor. (Msg#4309)") Then Exit Do

		If r_Used then 	'Revoke switch used so we must use a temparry for the DACL, then check against /R list
			Call GetDaclArray(TempParentArray,objSecDescriptor, TRUE)
			If blnErrorOccurred(" occurred when Calling GetDaclArray(TempParentArray,objSecDescriptor, TRUE) (Msg#4310a)") Then Exit Do
		Else
			Call GetDaclArray(OldDaclParentObj,objSecDescriptor, TRUE)
			If blnErrorOccurred(" occurred when Calling GetDaclArray(OldDaclParentObj,objSecDescriptor, TRUE) (Msg#4310b)") Then Exit Do
		End if

		boolOldDACLParentRevokedUserFound = False
		'What will happen here is we will go through the list of trustee's found from the parent, if any are found match a Revoked User, they are not put into the final list
		'and the boolOldDACLParentRevokedUserFound will be set to true, indicating that a Revoked User was found.


		If Not r_Used then Exit Do 	'Revoke user if present in Inherited Allowed or Denied lists
		
		'Now we have the inherited rights, if one of the revoked users is in the list, then we need to copy the list and turn off inheritance.
		If debug_on then Call PrintMsg("GetParentDACLArray: Looking for Revoke users in Inherited list, if found, Inherited list will be copied to Effective list and inheritance turned off, so we can revoke user")
		
		If UBound(TempParentArray) > 0 then
			For i = 1 to UBound(TempParentArray)
				boolSkipIt = False					
				If blnErrorOccurred(" occurred looping through OldDaclObj. (Msg#4311)") Then Exit Do
				For t = LBound(r_var_User) to UBound(r_var_User)
					If r_Var_User(t) <> "" then
						If TrusteesMatch(ObjTrustee_r_var_User(t), TempParentArray(i).Trustee) then
							'We found a match
							boolOldDACLParentRevokedUserFound = True
							boolSkipIt = True
							Exit For
						End if
					End if
				Next
				If Not boolSkipIt then
					'We need to add it to the array OldDaclParentObj
					Call AddObjectToArray(OldDaclParentObj, TempParentArray(i), -1)
				End if
				'Now we clear up the temporary array
				Set TempParentArray(i) = Nothing
			Next
		End If
		
		Exit Do
	Loop
	
	Set NewobjDescriptor = Nothing
	Set objmethod = Nothing
	Set objInParam = Nothing
	Set RetVal = Nothing
	Set objOutParams = Nothing
	Set objSecDescriptor = Nothing
		
	Call blnErrorOccurred(" occurred while in the GetParentDACLArray routine. (Msg#4312)")				
	If debug_on then Call PrintMsg("GetParentDACLArray: Exit")		
End Sub

'********************************************************************
'*
'* Sub SetACLForObject()
'* Purpose: Set the ACL for the file/folder passed
'* Input:   strPath - string containing path of file or folder, IsItAFolder, 
'* Output:  None
'*
'********************************************************************

Sub SetACLForObject(byval strPath, byval IsItAFolder)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("SetACLForObject: Enter")

    Dim objFileSecSetting, objmethod, objSecDescriptor
    Dim objtrustee, objInParam, objOutParams, objOwner
    Dim objParentFileSecSetting, objParentOutParams, objParentSecDescriptor

    Dim OldAceObj, ObjNewAce, NewobjDescriptor, RetVal, i_Var_Copy_Temp
    Dim OldDaclObj(), NewDaclObj(), ImpDenyDaclObj()
    Dim ImpAllowDaclObj(), ImpDenyObjectDaclObj()

    Dim objTempTrustee, i, t, ThisUserAccess, RightsToGive, NewRights
    Dim intTempAccessMask, boolDoTheUpdate
    Dim strOldAccount, strThisAccount, NewArraySize, NewArrayMember, BoolDoThisOne
    Dim ControlFlagsVar, BoolAllowInherited, BoolGetInherited, BoolInitialInheritRightsPresent, numControlFlags, ReturnAceFlags
    Dim TempAceFlag
    Dim intWhatToDoWithInheritance

    Const LeaveAloneInheritance = 0
    Const EnableInheritance = 1
    Const CopyAndTurnOffInheritance = 2
    Const RemoveAndTurnOffInheritance = 3

    'Put statements in loop to be able to drop out and clear variables
    Do

	'Initialize all of the new ACL objects
    	ReDim OldDaclObj(0)
    	ReDim NewDaclObj(0)
    	ReDim ImpDenyDaclObj(0)
    	ReDim ImpAllowDaclObj(0)
	ReDim InheritedObjectDaclObj(0)
	BoolAllowInherited = FALSE
	BoolGetInherited = TRUE 'Should only get inherited rights if we need to copy them.
	BoolInitialInheritRightsPresent = FALSE
	boolDoTheUpdate = FALSE
	i_Var_Copy_Temp = FALSE 'This is a local value that tells us if we need to copy inheritance locally and turn off.

	intWhatToDoWithInheritance = 0
	Select Case i_var
	Case 1 'Enable
		intWhatToDoWithInheritance = EnableInheritance
	Case 1 'Copy
		intWhatToDoWithInheritance = CopyAndTurnOffInheritance		
	Case 3 'Remove
		intWhatToDoWithInheritance = RemoveAndTurnOffInheritance
	End Select

	If debug_on then Call PrintMsg("SetACLForObject: Working on """ & strPath & """")

	set objFileSecSetting = objService.Get("Win32_LogicalFileSecuritySetting.Path=""" & Replace(strPath,"\","\\") & """")
	If blnErrorOccurred(" occurred setting Win32_LogicalFileSecuritySetting object. (Msg#501)") Then 
		If debug_on then 
			Call PrintMsg("SetACLForObject: objService.Get query that failed:")
			Call PrintMsg("SetACLForObject:           Win32_LogicalFileSecuritySetting.Path=""" & Replace(strPath,"\","\\") & """")
		end if
		Exit Do
	End if

	Set objOutParams = objFileSecSetting.ExecMethod_("GetSecurityDescriptor")
	If blnErrorOccurred(" occurred Setting objOutParams = objFileSecSetting.ExecMethod_(""GetSecurityDescriptor"") (Msg#502)") Then Exit Do

	set objSecDescriptor = objOutParams.Descriptor
	If blnErrorOccurred(" occurred setting objSecDescriptor = objOutParams.Descriptor. (Msg#503)") Then Exit Do

	Set objOwner = objSecDescriptor.Owner
	If blnErrorOccurred(" occurred setting objOwner = objSecDescriptor.Owner. (Msg#504)") Then Exit Do

	numControlFlags = objSecDescriptor.ControlFlags

	If e_Used then
		If debug_on then Call PrintMsg("SetACLForObject: Getting DACL array")

		'If e_Used then the old ACL list is maintained, if not we start fresh.
		Call GetDaclArray(OldDaclObj,objSecDescriptor, FALSE)
		If blnErrorOccurred(" occurred after Calling GetDaclArray(OldDaclObj,objSecDescriptor, FALSE) (Msg#505)") Then Exit Do
	Else
		Set OldDaclObj(0) = CreateObject("AccessControlEntry")
	End if

	If UBound(OldDaclObj) <> 0 then
		For i = 1 to UBound(OldDaclObj)
			If blnErrorOccurred(" occurred looping through OldDaclObj. (Msg#506)") Then Exit Do
			Set OldAceObj = OldDaclObj(i)
			If StringAceFlag(OldAceObj.AceFlags, numControlFlags, SE_DACL_AUTO_INHERITED, TRUE, ReturnAceFlags) = "Inherited" then
				Set OldAceObj = OldDaclObj(i)
				Set objTempTrustee = OldAceObj.Trustee
				BoolInitialInheritRightsPresent = TRUE
				Exit For 
			End if
		Next
	End if

	If (r_Used and i_var<>3) or i_var=2 then 	'We must get the Parent DACL in case one of those users are Revoked, Also if we want to Copy the Inheritance
		'We must call into GetParentDACLArray in order to verify we have the correct Parent Folder's DACL array, and also to verify that a Revoked User isn't in this list.
		Call GetParentDACLArray(objFileSecSetting,IsItAFolder)

		'We only want to copy the inheritance locally if a Revoked User was found...
		If BoolInitialInheritRightsPresent and boolOldDACLParentRevokedUserFound then
				Call PrintMsg("  - One of the Revoked Users is listed under Inherited permissions.")
				Call PrintMsg("     Copying Inherited Permissions and turning off inheritance.")
				intWhatToDoWithInheritance = CopyAndTurnOffInheritance
				i_Var_Copy_Temp = True
		End if
		'If the /I was used, and COPY, then we want to copy it either way...
		If i_var=2 then 
			i_Var_Copy_Temp = True
			intWhatToDoWithInheritance = CopyAndTurnOffInheritance
		End if
	End if

	'If i_Var_Copy_Temp is true, then we need to copy in the Parentarray into the exiting DACL array
	If i_Var_Copy_Temp then
		boolDoTheUpdate = True
		If UBound(OldDaclParentObj) <> 0 then
			For i = 1 to UBound(OldDaclParentObj)
				Set OldAceObj = OldDaclParentObj(i)
				If ((OldAceObj.AceFlags And INHERITED_ACE) = INHERITED_ACE) Then
					OldAceObj.AceFlags = OldAceObj.AceFlags - INHERITED_ACE
				End if
				Call AddObjectToArray(OldDaclObj, OldAceObj, 0)
			Next
		End if
	End If

	If debug_on then Call PrintMsg("SetACLForObject: Sorting DACL array and modifying rights if needed")

	If UBound(OldDaclObj) > 0 then
		For i = 1 to UBound(OldDaclObj)
			BoolDoThisOne = TRUE
			If blnErrorOccurred(" occurred looping through OldDaclObj. (Msg#517)") Then Exit Do
			Set OldAceObj = OldDaclObj(i)
			Set objTempTrustee = OldAceObj.Trustee
			If blnErrorOccurred(" occurred Setting objTempTrustee = OldAceObj.Trustee (Msg#518)") Then Exit Do
			intTempAccessMask = OldAceObj.AccessMask
			If debug_on then Call PrintMsg("SetACLForObject: """ & TrusteesDisplay(objTempTrustee) & """ current rights = " & SECString(OldAceObj.AccessMask,TRUE))

			'We won't track Inherited Rights because we already collected them in the GetParentDACLArray
			If StringAceFlag(OldAceObj.AceFlags, numControlFlags, SE_DACL_AUTO_INHERITED, TRUE, ReturnAceFlags) = "Inherited" then BoolDoThisOne = FALSE

			If BoolDoThisOne then
				If p_Used then 	'Replace user rights if present
					For t = LBound(p_var_User) to UBound(p_var_User)
						If p_Var_User(t) <> "" then
							If TrusteesMatch(ObjTrustee_p_var_User(t), objTempTrustee) then
								'We found a match so skip it.
								BoolDoThisOne = FALSE
								Call PrintMsg("Replacing rights for existing user """ & p_Var_User(t) & """")
							End if
						End if
					Next
				End If
			End if
			If BoolDoThisOne then
				If r_Used then 	'Revoke user if present in Allowed or Denied lists
					For t = LBound(r_var_User) to UBound(r_var_User)
						If r_Var_User(t) <> "" then
							If TrusteesMatch(ObjTrustee_r_var_User(t), objTempTrustee) then
								'We found a match so skip it.
								BoolDoThisOne = FALSE
								Call PrintMsg("Revoking rights for existing user """ & r_Var_User(t) & """")
							End if
						End if
					Next
				End if
			End if
			If BoolDoThisOne then
				Select Case OldAceObj.AceType
       				Case ACCESS_ALLOWED_ACE_TYPE
					Call AddObjectToArray(ImpAllowDaclObj, OldAceObj, -1)
				Case ACCESS_DENIED_ACE_TYPE
					Call AddObjectToArray(ImpDenyDaclObj, OldAceObj, -1)
				Case Else
					Call PrintMsg("Error: Bad ace...." & Hex(OldAceObj.AceType))
				End Select
			End if
		Next
	End If
	
	'Add ACE's that need to be added:

	If g_Used then 	'Grant rights for these users

		If debug_on then Call PrintMsg("SetACLForObject: Granting Rights for Users (that haven't been granted already)")
		Call AccessMask_New(ImpAllowDaclObj, ObjTrustee_g_var_User, g_var_User, g_var_Perm, g_var_Spec, g_var_PermBit, g_var_SpecBit, ACCESS_ALLOWED_ACE_TYPE, "Granting", IsItAFolder)
		If blnErrorOccurred(" occurred when Building Granted (File) Rights array (Msg#519)") Then Exit Do		

	End if
	If p_Used then 	'Grant rights for these users (Replace rights)

		If debug_on then Call PrintMsg("SetACLForObject: Replacing Rights for Users (that haven't been granted already)")

		Call AccessMask_New(ImpAllowDaclObj, ObjTrustee_p_var_User, p_var_User, p_var_Perm, p_var_Spec, p_var_PermBit, p_var_SpecBit, ACCESS_ALLOWED_ACE_TYPE, "Replacing", IsItAFolder)
		If blnErrorOccurred(" occurred when Building Replace (File) Rights array (Msg#520)") Then Exit Do		

	End if

	If d_Used then 	'Deny rights for these users

		If debug_on then Call PrintMsg("SetACLForObject: Denying Rights for Users (that haven't been denied already)")

		Call AccessMask_New(ImpDenyDaclObj, ObjTrustee_d_var_User, d_var_User, d_var_Perm,  d_var_Spec, d_var_PermBit,  d_var_SpecBit, ACCESS_DENIED_ACE_TYPE, "Denying", IsItAFolder)
		If blnErrorOccurred(" occurred when Building Denied (File) Rights array (Msg#521)") Then Exit Do

	End if

	' Combine the ACEs in the proper order
	' Implicit Deny
	' Implicit Allow
	' Inherited Aces

	If debug_on then Call PrintMsg("SetACLForObject: Forming new DACL array")

	ReDim NewDaclObj(0)
	If UBound(ImpDenyDaclObj) > 0 then		'0 member is always blank
		For i = (LBound(ImpDenyDaclObj) + 1) to UBound(ImpDenyDaclObj)
			boolDoTheUpdate = TRUE
			Call AddObjectToArray(NewDaclObj, ImpDenyDaclObj(i), 0)
		Next
		If blnErrorOccurred(" occurred when Building Implicit Deny array (Msg#524)") Then Exit Do
	End if
	If UBound(ImpAllowDaclObj) > 0 then
		For i = (LBound(ImpAllowDaclObj) + 1) to UBound(ImpAllowDaclObj)
			boolDoTheUpdate = TRUE
			Call AddObjectToArray(NewDaclObj, ImpAllowDaclObj(i), 0)
		Next
		If blnErrorOccurred(" occurred when Building Implicit Allow array (Msg#525)") Then Exit Do
	End if
	If UBound(NewDaclObj)=0 and Not boolDoTheUpdate Then
		Set NewDaclObj(0) = objService.Get("Win32_Ace").Spawninstance_
		If blnErrorOccurred(" occurred Setting NewDaclObj(0) = CreateObject(""AccessControlEntry""). (Msg#527)") Then Exit Do
	End if
	
	Select Case intWhatToDoWithInheritance
	Case EnableInheritance
		ControlFlagsVar = SE_DACL_PRESENT + SE_DACL_AUTO_INHERITED + SE_SELF_RELATIVE
	Case CopyAndTurnOffInheritance, RemoveAndTurnOffInheritance
		ControlFlagsVar = SE_DACL_PRESENT + SE_DACL_PROTECTED + SE_SELF_RELATIVE	
	Case Else 	'Same as LeaveAloneInheritance
		If BoolInitialInheritRightsPresent then 	' We want to keep Inherited rights if they were present
			ControlFlagsVar = SE_DACL_PRESENT + SE_DACL_AUTO_INHERITED + SE_SELF_RELATIVE
		Else
			ControlFlagsVar = SE_DACL_PRESENT + SE_DACL_PROTECTED + SE_SELF_RELATIVE
		End if
	End Select

	If debug_on then Call PrintMsg("SetACLForObject: Saving new Descriptor")

	Set NewobjDescriptor = objService.Get("Win32_SecurityDescriptor").Spawninstance_
	If blnErrorOccurred(" occurred Setting NewobjDescriptor = objService.Get(""Win32_SecurityDescriptor"").Spawninstance_ (Msg#528)") Then Exit Do

	NewobjDescriptor.Properties_.item("DACL") = NewDaclObj
	If blnErrorOccurred(" occurred setting NewobjDescriptor.Properties_.item(""DACL"") = NewDaclObj (Msg#529)") Then Exit Do

	If o_Used then 	'Change Ownership
		If debug_on then Call PrintMsg("SetACLForObject: Changing Ownership")
		If o_Var <> "" then
			If Not ObjTrustee_o_var_User Is Nothing then
				If TrusteesMatch(objOwner, ObjTrustee_o_var_User) then
					Call PrintMsg("Ownership not going to be changed, owner is already set to """ & TrusteesDisplay(ObjTrustee_o_var_User) & """")
				Else
					NewobjDescriptor.Properties_.item("Owner") = ObjTrustee_o_var_User
					If blnErrorOccurred(" occurred setting NewobjDescriptor.Properties_.item(""Owner"") = ObjTrustee_o_var_User (Msg#532)") Then Exit Do				
					Call PrintMsg("Changing Ownership to """ & TrusteesDisplay(ObjTrustee_o_var_User) & """")
				End if
			End if
		End if
	End if

	NewobjDescriptor.ControlFlags =  ControlFlagsVar
	If blnErrorOccurred(" occurred setting  NewobjDescriptor.ControlFlags =  ControlFlagsVar (Msg#533)") Then Exit Do

	Set objmethod = objFileSecSetting.Methods_("SetSecurityDescriptor")
	If blnErrorOccurred(" occurred setting objmethod = objFileSecSetting.Methods_(""SetSecurityDescriptor"") (Msg#534)") Then Exit Do

	Set objInParam = objmethod.inParameters.SpawnInstance_()
	If blnErrorOccurred(" occurred Setting objInParam = objmethod.inParameters.SpawnInstance_() (Msg#535)") Then Exit Do

	objInParam.Properties_.item("Descriptor") = NewobjDescriptor
	If blnErrorOccurred(" occurred setting objInParam.Properties_.item(""Descriptor"") = NewobjDescriptor (Msg#536)") Then Exit Do

	Set RetVal = objFileSecSetting.ExecMethod_("SetSecurityDescriptor", objInParam)	
	If blnErrorOccurred(" occurred setting RetVal = objFileSecSetting.ExecMethod_(""SetSecurityDescriptor"", objInParam) (Msg#537)") Then Exit Do

	Select Case RetVal.ReturnValue
		Case 0		'The request was accepted. 
			Call PrintMsg("Completed successfully.")
		Case 1		'The request is not supported. 
			Call PrintMsg("Error: The request is not supported.  (Msg#538)")
		Case 2 		'The user did not have the necessary access. 
			Call PrintMsg("Error: The user did not have the necessary access.  (Msg#539)")
		Case 8 		'Interactive Process. 
			Call PrintMsg("Error: Interactive Process.  (Msg#540)")
		Case 9 		'The name specified was invalid. 
			Call PrintMsg("Error: The name specified was invalid.  (Msg#541)")
		Case 21 		'Invalid parameters have been passed to the service. 
			Call PrintMsg("Error: Invalid parameters have been passed to the service.  (Msg#542)")
		Case 1307
			Call PrintMsg("Error: This security ID may not be assigned as the owner of this object. (Msg#543)")
		Case Else
			Call PrintMsg("Error: Error Code = " & RetVal.ReturnValue & "    (Msg#544)")
	End Select

	Exit Do											'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now

    Set objOwner = Nothing
    Set objFileSecSetting = Nothing
    Set objmethod = Nothing
    Set objSecDescriptor = Nothing
    Set objtrustee = Nothing
    Set objInParam = Nothing
    Set objOutParams = Nothing
    Set OldAceObj = Nothing
    Set ObjNewAce = Nothing
    Set NewobjDescriptor = Nothing
    Set objTempTrustee = Nothing
    Set RetVal = Nothing

    Call blnErrorOccurred(" occurred while in the SetACLForObject routine. (Msg#539)")				
    If debug_on then Call PrintMsg("SetACLForObject: Exit")

End Sub


'********************************************************************
'*
'* Function AccessMask_New()
'* Purpose: Takes a list of users with permissions and adds them to the list
'* Input:   Array_ACLobj	:	DACL Array
'*          Array_Users		:	Array of users
'*          Array_Perm		:	Array of permissions
'*          AceType		:	Type of Permissions (Allow or Deny)
'*          AceFlag		:	Apply to what (Files or Folders)
'*          strAction		:	String saying what the action was (Grant, Replace, or Deny)
'* Output:  Acl Array Object
'*
'********************************************************************

Function AccessMask_New(byref Array_ACLobj, byref Array_ObjTrustee, byval Array_Users, byval Array_Perm, byval Array_Spec, byval Array_PermBit, byval Array_SpecBit,byval AceType, byval strAction, byval IsItAFolder)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("AccessMask_New: Enter")

    Dim t, objNEWACE, RightsToGive, AceTypeString, TempAceFlag, TempAceFlagFile
    Dim TempAceFlagFolder, intWhoCares

    'Put statements in loop to be able to drop out and clear variables

    Do
    
	AccessMask_New = FALSE

	' Find out what this applied to, for the ACEFlag value
	' "This Folder Only"					= 0
	' "This Folder and Files" 				= 1 		or 	OBJECT_INHERIT_ACE
	' "This Folder and Subfolders" 		= 2 		or 	CONTAINER_INHERIT_ACE
	' "This Folder, Subfolders and Files" 	= 3 		or 	OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE
	' "Files Only" 						= 9 		or 	OBJECT_INHERIT_ACE + INHERIT_ONLY_ACE
	' "Subfolders only" 					= 10 	or 	CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE
	' "Subfolders and Files only" 			= 11 	or 	OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE

	For t = LBound(Array_Users) to UBound(Array_Users)
		If blnErrorOccurred(" occurred while " & strAction & " permissions. (Msg#601)") Then Exit Do
		If Array_Users(t) <> "" and (Array_Perm(t) <> "" or Array_Spec(t) <> "") then
			If IsObject(Array_ObjTrustee(t)) then
				TempAceFlag = 0
				'Perm Setting
				If Array_Perm(t) <> "" then
					If IsItAFolder then	
						If spec_used then
							TempAceFlag = spec_var
						Else
							If Array_Spec(t) = "*GUI*" or Array_Spec(t) = Array_Perm(t) then
								If Array_Perm(t) = "L" then
									TempAceFlag = CONTAINER_INHERIT_ACE
								Else						
									TempAceFlag = OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE
								End if
							Else					
								TempAceFlag = OBJECT_INHERIT_ACE + INHERIT_ONLY_ACE
							End if
						End if
					Else				
						TempAceFlag = OBJECT_INHERIT_ACE
					End if
					AceTypeString = StringAceFlag(TempAceFlag, 0, 0, FALSE, intWhoCares)
					
					RightsToGive = Array_PermBit(t)
					If blnErrorOccurred(" occurred getting rights for " & Array_Users(t) & ". (Msg#602a)") Then Exit Do

					Set objNEWACE = SetACE(RightsToGive, TempAceFlag, AceType, Array_ObjTrustee(t))
					If blnErrorOccurred(" occurred creating ""ACE Object"" for " & Array_Users(t) & ". (Msg#603a)") Then Exit Do

					Call AddObjectToArray(Array_ACLobj, objNEWACE, -1)
					If blnErrorOccurred(" occurred adding (" & strAction & ") New ACE object to array. (Msg#604a)") Then Exit Do

					Set objNEWACE = Nothing
					Call PrintMsg(strAction & " NTFS rights (" & Array_Perm(t) & " access for " & AceTypeString & ") for """ & Array_Users(t) & """")
				End if

				TempAceFlag = 0
				'Spec Setting (we only do this if its a folder and perm and spec don't match
				If Array_Spec(t) <> "" and IsItAFolder then
					If Array_Spec(t) <> "*GUI*" and Array_Spec(t) <> Array_Perm(t) then	
						If spec_used then
							TempAceFlag = spec_var
						Else
							If s_Used then TempAceFlag = CONTAINER_INHERIT_ACE
						End if
						
						AceTypeString = StringAceFlag(TempAceFlag, 0, 0, FALSE, intWhoCares)
						
						RightsToGive = Array_SpecBit(t)
						If blnErrorOccurred(" occurred getting rights for " & Array_Users(t) & ". (Msg#602b)") Then Exit Do

						Set objNEWACE = SetACE(RightsToGive, TempAceFlag, AceType, Array_ObjTrustee(t))
						If blnErrorOccurred(" occurred creating ""ACE Object"" for " & Array_Users(t) & ". (Msg#603b)") Then Exit Do

						Call AddObjectToArray(Array_ACLobj, objNEWACE, -1)
						If blnErrorOccurred(" occurred adding (" & strAction & ") New ACE object to array. (Msg#604b)") Then Exit Do
						Set objNEWACE = Nothing
						Call PrintMsg(strAction & " NTFS rights (" & Array_Spec(t) & " access for " & AceTypeString & ") for """ & Array_Users(t) & """")
					End if				
				End if
			Else
				Call PrintMsg("Failed " & strAction & " NTFS rights for """ & Array_Users(t) & """")
			End if
		End if
	Next

	AccessMask_New = TRUE

	Exit Do		'We really didn't want to loop
    Loop

    Set objNEWACE = Nothing

    If debug_on then Call PrintMsg("AccessMask_New: Return = " & AccessMask_New)

    Call blnErrorOccurred(" occurred while in the AccessMask_New routine. (Msg#605)")
    If debug_on then Call PrintMsg("AccessMask_New: Exit")

End Function


'********************************************************************
'*
'* Sub TrusteesMatch()
'* Purpose: Checks the Trustee to the Name and Domain and returns boolean TRUE if they match
'* Input:   objTrusteeA, objTrusteeB
'* Output:  Boolean
'* Notes:   This is a nice way to check if one trustee matches another, the procedure can change
'*          and compare different values and only needs to be changed here, not in the rest of the code.
'*
'********************************************************************

Function TrusteesMatch(ByRef objTrusteeA, ByRef objTrusteeB)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("TrusteesMatch: Enter")

    'Put statements in loop to be able to drop out and clear variables

    Do
	TrusteesMatch = FALSE
	If debug_on then Call PrintMsg("TrusteesMatch: Checking Users to see if they match")	

	Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#701)")
	If NOT IsObject(objTrusteeA) then 
		Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#702)")
		Exit Do
	End if

	If NOT IsObject(objTrusteeB) then 
		Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#703)")
		Exit Do
	End if
	If objTrusteeA.SIDString = objTrusteeB.SIDString then
		TrusteesMatch = TRUE
		If debug_on then Call PrintMsg("TrusteesMatch: Sid's match")
		Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#706)")
		Exit Do
	End if
	If Global_bool_SID_Used then
		If debug_on then Call PrintMsg("TrusteesMatch: Will not check for Name match because a SID# was specified")
		Exit Do
	End if
	If UCase(objTrusteeA.Domain) = UCase(objTrusteeB.Domain) then
		Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#707)")
		If UCase(objTrusteeA.Name) = UCase(objTrusteeB.Name) then
			TrusteesMatch = True
			If debug_on then Call PrintMsg("TrusteesMatch: Names match")
			Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#708)")
			Exit Do
		End if
	End if
	
	If debug_on then Call PrintMsg("TrusteesMatch: No Match")

	Exit Do		'We really didn't want to loop
    Loop

    Call blnErrorOccurred(" occurred while in the TrusteesMatch routine. (Msg#709)")
    If debug_on then Call PrintMsg("TrusteesMatch: Exit")

End Function

'********************************************************************
'*
'* Sub TrusteesDisplay()
'* Purpose: Returns a Display ready string of trustee passed in
'* Input:   objTrustee
'* Output:  String
'* Notes:   This makes the display of a trustee a standard throughout the code
'*
'********************************************************************

Function TrusteesDisplay(ByRef objTrustee)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("TrusteesDisplay: Enter")

    'Put statements in loop to be able to drop out and clear variables

    Do
	TrusteesDisplay = ""

	If NOT IsObject(objTrustee) then 
		Exit Do
	End if

	If objTrustee.Domain = "" then
		TrusteesDisplay = objTrustee.Name
	Else
		TrusteesDisplay = objTrustee.Domain & "\" & objTrustee.Name
	End if

	Exit Do		'We really didn't want to loop
    Loop

    Call blnErrorOccurred(" occurred while in the TrusteesDisplay routine. (Msg#801)")
    If debug_on then Call PrintMsg("TrusteesDisplay: Exit")

End Function

'********************************************************************
'*
'* Sub CheckTrustees()
'* Purpose: Checks the list of entered users and makes them valid, run only once
'* Input:   Global Variables only
'* Output:  True if we successfully found one trustee
'* Notes:   This function is called only one time in the code to get a trustee object for
'*          every user entered, and if we can't find one then the user is deleted from the list.
'*
'********************************************************************

Function CheckTrustees()
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("CheckTrustees: Enter")

    'Put statements in loop to be able to drop out and clear variables

    Do
	CheckTrustees = False
	If debug_on then Call PrintMsg("CheckTrustees: Checking Users to make sure they are proper Trustee's")

	Call GetDefaultNames()
	Call GetDefaultDomainSid()

	If g_Used then 	'Add to users
		If debug_on then Call PrintMsg("CheckTrustees: Checking /G users")
		If FixListOfTrustees(g_Var_User, ObjTrustee_g_var_User, "/G") then 
			CheckTrustees = True
		Else
			g_Used = False
		End if
	End if
	If p_Used then 	'Replace users
		If debug_on then Call PrintMsg("CheckTrustees: Checking /P users")
		If FixListOfTrustees(p_Var_User, ObjTrustee_p_var_User, "/P")  then  
			CheckTrustees = True
		Else		
			p_Used = False
		End if
	End if
	If d_Used then 	'Change users
		If debug_on then Call PrintMsg("CheckTrustees: Checking /D users")
		If FixListOfTrustees(d_Var_User, ObjTrustee_d_var_User, "/D") then 
			CheckTrustees = True
		Else
			d_Used = False
		End if
	End if
	If r_Used then 	'Revoke users
		If debug_on then Call PrintMsg("CheckTrustees: Checking /R users")
		If FixListOfTrustees(r_Var_User, ObjTrustee_r_var_User, "/R") then  
			CheckTrustees = True
		Else
			r_Used = False
		End if
	End if	
	If o_used then
		If debug_on then Call PrintMsg("CheckTrustees: Checking /O user")
		If FixThisTrustee(o_Var, ObjTrustee_o_var_User,"/O")  then  
			CheckTrustees = True
		Else
			o_used = False
		End if
	End if

	Exit Do		'We really didn't want to loop
    Loop

    If debug_on then Call PrintMsg("CheckTrustees: Return = " & CheckTrustees)

    Call blnErrorOccurred(" occurred while in the CheckTrustees routine. (Msg#901)")
    If debug_on then Call PrintMsg("CheckTrustees: Exit")

End Function


'********************************************************************
'*
'* Function FixListOfTrustees()
'* Purpose: Takes a list of users and changes to a valid trustee if found, else sets string to ""
'* Input:   Array_Users, strAction
'* Output:  Dacl Array Object
'*
'********************************************************************

Function FixListOfTrustees(byref Array_Users, byref Array_ObjTrustee, byval strAction)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("FixListOfTrustees: Enter")

    Dim t, objTempTrustee, strTempName

    'Put statements in loop to be able to drop out and clear variables

    Do
	FixListOfTrustees = FALSE
	For t = LBound(Array_Users) to UBound(Array_Users)
		strTempName = ""
		If Array_Users(t) <> "" then
			If FixThisTrustee(Array_Users(t), objTempTrustee,strAction) then
				FixListOfTrustees = TRUE		'We return True if we find at least one trustee
				Call AddObjectToArray(Array_ObjTrustee, objTempTrustee, t)
				Set objTempTrustee = Nothing
			End if
		End if
	Next


	Exit Do		'We really didn't want to loop
    Loop

    Set objTempTrustee = Nothing
    If debug_on then Call PrintMsg("FixListOfTrustees: Return = " & FixListOfTrustees)

    Call blnErrorOccurred(" occurred while in the FixListOfTrustees routine. (Msg#1001)")
    If debug_on then Call PrintMsg("FixListOfTrustees: Exit")

End Function

'********************************************************************
'*
'* Function FixThisTrustee()
'* Purpose: Takes one user and changes to a valid trustee if found, else sets string to ""
'* Input:   User String, strAction
'* Output:  Dacl Object
'*
'********************************************************************

Function FixThisTrustee(byref strThisTrusteeName, byref ObjTrustee, strAction)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("FixThisTrustee: Enter")

    Dim strTempName

    'Put statements in loop to be able to drop out and clear variables

    Do
	FixThisTrustee = FALSE
	strTempName = strThisTrusteeName

	'Replace #machine# with actual machine name if its in the string
	strTempName = Replace(ucase(strTempName),"#MACHINE#", strDefaultDomain)

	Set ObjTrustee = SetTrustee(strTempName)
	If blnErrorOccurred(" occurred Setting objTempTrustee = SetTrustee(strTempName) (Msg#1051)") Then Exit Do

	If ObjTrustee Is Nothing then
		Call PrintMsg("Could not find " & strAction & " user/group: """ & strThisTrusteeName & """ removing from list.")
		strThisTrusteeName = ""
	Else
		strTempName = TrusteesDisplay(ObjTrustee)
		If IsNull(ObjTrustee.Domain) then ObjTrustee.Domain = ""
		If UCASE(strThisTrusteeName) <> UCASE(strTempName) then
			Call PrintMsg(" - Changing " & strAction & " user/group: """ & strThisTrusteeName & """ to """ & strTempName & """")
		End if
		strThisTrusteeName = strTempName
		FixThisTrustee = TRUE	'Means we successfully found the user without error
	End if

	Exit Do		'We really didn't want to loop
    Loop

    If debug_on then Call PrintMsg("FixThisTrustee: Return = " & FixThisTrustee)

    Call blnErrorOccurred(" occurred while in the FixThisTrustee routine. (Msg#1052)")
    If debug_on then Call PrintMsg("FixThisTrustee: Exit")

End Function


'********************************************************************
'*
'* Sub GetDaclArray()
'* Purpose: Return Dacl Array object if found
'* Input:   objArrayPassedIn, objSecDescriptor, getonlyInherited
'* Output:  Dacl Array Object
'*
'********************************************************************

Sub GetDaclArray(DACL_Array, objSecDescriptor, getonlyInherited)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetDaclArray: Enter")

    Dim TempDACL_Array, objDACL_Member, numControlFlags, ReturnAceFlags

    'Put statements in loop to be able to drop out and clear variables

    Do
	numControlFlags = objSecDescriptor.ControlFlags
	If blnErrorOccurred(" occurred getting ControlFlags. (Msg#1101)") Then Exit Do


	TempDACL_Array = objSecDescriptor.DACL
	If blnErrorOccurred(" occurred getting Temporary DACL array. (Msg#1102)") Then Exit Do

	If IsArray(TempDACL_Array) then
		For Each objDACL_Member in TempDACL_Array
			If blnErrorOccurred(" occurred while looping through TempDACL_Array. (Msg#1103)") Then Exit Do
			If getonlyInherited then
				If StringAceFlag(objDACL_Member.AceFlags, numControlFlags, SE_DACL_AUTO_INHERITED, TRUE, ReturnAceFlags) = "Inherited" then Call AddObjectToArray(DACL_Array, objDACL_Member, -1)
			Else
				Call AddObjectToArray(DACL_Array, objDACL_Member, -1)
			End If
		Next
	End if
	If (UBound(DACL_Array) = 0) Then
		Set DACL_Array(0) = CreateObject("AccessControlEntry")
		If blnErrorOccurred(" occurred Setting DACL_Array(0) = CreateObject(""AccessControlEntry""). (Msg#1104)") Then Exit Do
	End if
	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objDACL_Member = Nothing

    Call blnErrorOccurred(" occurred while in the GetDaclArray routine. (Msg#1105)")
    If debug_on then Call PrintMsg("GetDaclArray: Exit")

End Sub


'********************************************************************
'* Function SetTrustee()
'* Purpose: Returns Win32_Trustee object for User/group or Nothing if not found
'* Input:   strFullName
'* Output:  Win32_Trustee object is returned, Nothing if not found
'********************************************************************
Function SetTrustee(byval strFullName)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("SetTrustee: Enter")

    Dim objTrustee, objAccount, objSID, strSid, strDomain, strName

    'Put statements in loop to be able to drop out and clear variables
    Do
	Set SetTrustee = Nothing
	strSid = ""

	Set objTrustee = objService.Get("Win32_Trustee").Spawninstance_

	If blnErrorOccurred(" occurred in getting objService.Get(Win32_Trustee).Spawninstance_ (Msg#1201)") Then Exit Do
	
	If UCASE(Left(strFullName,4))="SID#" then
		Global_bool_SID_Used = True
		strSid = UCASE(Mid(strFullName, 5))
	End if

	If strSid = "" then
		'GetStandardSid will parse the strFullname into strDomain and strName
		strSid = GetStandardSid(strFullName, strDomain, strName)
		
		If strSid = "" then
			Set objAccount = GetAccountObj(strDomain, strName)
			If blnErrorOccurred(" occurred getting Account Object. (Msg#1202)") Then 
				Exit Do
			End if

			If objAccount Is Nothing then 
				Call PrintMsg("Can't find Account: """ & strFullName & """")
				Exit Do
			Else
				strSid = objAccount.Sid
	    		End If
	    	End if
	End If

	If strSid = "" then 'This means it doesn't exist
		Call PrintMsg("Can't find Sid for: """ & strFullName & """")
		Exit Do
   	End if

	set objSID = objService.Get("Win32_SID.SID=""" & strSid &"""")
	If blnErrorOccurred(" occurred getting Win32_SID.SID Object. (Msg#1203)") Then 
		Exit Do
	End if

	objTrustee.Domain = objSID.ReferencedDomainName
	objTrustee.Name = objSID.AccountName
	objTrustee.Properties_.item("SID") = objSID.BinaryRepresentation
	objTrustee.Properties_.item("SidLength") = objSID.SidLength
	objTrustee.Properties_.item("SIDString") = objSID.Sid

	set objSID = nothing
	Set objAccount = Nothing

	set SetTrustee = objTrustee
	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objTrustee = Nothing
    Set objAccount = Nothing
    Set objSID = Nothing

    If debug_on then 
	If SetTrustee is Nothing then
		Call PrintMsg("SetTrustee: Return = " & "Nothing")
	Else
		Call PrintMsg("SetTrustee: Return = " & "Win32_Trustee object")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the SetTrustee routine. (Msg#1204)")
    If debug_on then Call PrintMsg("SetTrustee: Exit")

End Function

'********************************************************************
'* Function GetStandardSid()
'* Purpose: Returns Sid if the account is a special account
'* Input:   strFullName
'* Output:  String Value of Sid
'********************************************************************
Function GetStandardSid(ByRef strFullName, ByRef strDomain, ByRef strName)
    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetStandardSid: Enter")
    Dim strSpecialDomain, strtemp

    'Put statements in loop to be able to drop out and clear variables
    Do
	strDomain = ""
	strName = ""
	'These next 3 lines will remove any leading \\ from the username (they shouldn't be there)
	'This helps prevent the split of domain and username from failing
	While Left(strFullName,1) = "\"
		strFullName = Mid(strFullName, 2)
	Wend
	If InStr(1, strFullName, "\",1) > 0 then
		strDomain = Left(strFullName, InStr(1, strFullName, "\",1) - 1)
		strName = Mid(strFullName, InStr(1, strFullName, "\",1) + 1)
	Else
		If InStr(1, strFullName, "/",1) > 0 then
			strDomain = Left(strFullName, InStr(1, strFullName, "/",1) - 1)
			strName = Mid(strFullName, InStr(1, strFullName, "/",1) + 1)
		Else
			strName = strFullName
		End if
	End if
	strSpecialDomain = ""

	If strDomain <> "" then
		strtemp = ucase(strDomain)
		If strtemp <> "NT AUTHORITY"  and strtemp <> "BUILTIN" and strtemp <> UCase(strSystemDomainName) then Exit Do
	End if
	
	'List comes primarily from http://support.microsoft.com/support/kb/articles/q243/3/30.asp
	
	Select Case UCase(strName)
	'**************************************************
	'All of these are [strSpecialDomain = "NT AUTHORITY"] so we don't set it every time, we just make that the default.
	Case "EVERYONE"
		GetStandardSid = "S-1-1-0"
		strSpecialDomain = "NT AUTHORITY"
	Case "CREATOR GROUP"
		GetStandardSid = "S-1-3-1"
		strSpecialDomain = "NT AUTHORITY"
	Case "CREATOR OWNER"
		GetStandardSid = "S-1-3-0"
		strSpecialDomain = "NT AUTHORITY"
	Case "DIALUP"
		GetStandardSid = "S-1-5-1"
		strSpecialDomain = "NT AUTHORITY"
	Case "NETWORK"
		GetStandardSid = "S-1-5-2"
		strSpecialDomain = "NT AUTHORITY"
	Case "BATCH"
		GetStandardSid = "S-1-5-3"
		strSpecialDomain = "NT AUTHORITY"
	Case "INTERACTIVE"
		GetStandardSid = "S-1-5-4"
		strSpecialDomain = "NT AUTHORITY"
	Case "SERVICE"
		GetStandardSid = "S-1-5-6"
		strSpecialDomain = "NT AUTHORITY"
	Case "ANONYMOUS LOGON"
		GetStandardSid = "S-1-5-7"
		strSpecialDomain = "NT AUTHORITY"
	Case "PROXY"
		GetStandardSid = "S-1-5-8"
		strSpecialDomain = "NT AUTHORITY"
	Case "ENTERPRISE DOMAIN CONTROLLERS", "ENTERPRISE DOMAIN"
		GetStandardSid = "S-1-5-9"
		strSpecialDomain = "NT AUTHORITY"
	Case "PRINCIPAL SELF", "SELF"
		GetStandardSid = "S-1-5-10"
		strSpecialDomain = "NT AUTHORITY"
	Case "AUTHENTICATED USERS"
		GetStandardSid = "S-1-5-11"
		strSpecialDomain = "NT AUTHORITY"
	Case "RESTRICTED"
		GetStandardSid = "S-1-5-12"
		strSpecialDomain = "NT AUTHORITY"
	Case "TERMINAL SERVER USERS"
		GetStandardSid = "S-1-5-13"
		strSpecialDomain = "NT AUTHORITY"
	Case "LOCAL SYSTEM", "SYSTEM"
		GetStandardSid = "S-1-5-18"
		strSpecialDomain = "NT AUTHORITY"
	Case "LOCAL SERVICE"
		GetStandardSid = "S-1-5-19"
		strSpecialDomain = "NT AUTHORITY"
	Case "NETWORK SERVICE"
		GetStandardSid = "S-1-5-20"		
		strSpecialDomain = "NT AUTHORITY"
	'**************************************************		
	Case "ADMINISTRATORS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-544"
	Case "USERS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-545"
	Case "GUESTS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-546"
	Case "POWER USERS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-547"
	Case "ACCOUNT OPERATORS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-548"
	Case "SERVER OPERATORS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-549"
	Case "PRINT OPERATORS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-550"
	Case "BACKUP OPERATORS"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-551"
	Case "REPLICATOR"
		strSpecialDomain = "BUILTIN"
		GetStandardSid = "S-1-5-32-552"
	Case "ADMINISTRATOR"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-500"
			strSpecialDomain = strSystemDomainName
		End if
	Case "GUEST"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-501"
			strSpecialDomain = strSystemDomainName
		End if
	Case "KRBTGT"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-502"
			strSpecialDomain = strSystemDomainName
		End if
	Case "DOMAIN ADMINS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-512"
			strSpecialDomain = strSystemDomainName
		End if
	Case "DOMAIN USERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-513"
			strSpecialDomain = strSystemDomainName
		End if
	Case "GUESTS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-514"
			strSpecialDomain = strSystemDomainName
		End if
	Case "DOMAIN COMPUTERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-515"
			strSpecialDomain = strSystemDomainName
		End if
	Case "DOMAIN CONTROLLERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-516"
			strSpecialDomain = strSystemDomainName
		End if
	Case "CERT PUBLISHERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-517"
			strSpecialDomain = strSystemDomainName
		End if
	Case "SCHEMA ADMINS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-518"
			strSpecialDomain = strSystemDomainName
		End if
	Case "GROUP POLICY CREATOR OWNERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-520"
			strSpecialDomain = strSystemDomainName
		End if
	Case "RAS AND IAS SERVERS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-533"
			strSpecialDomain = strSystemDomainName
		End if
	Case "GUESTS"
		if strSystemDomainSid <> "" then 
			GetStandardSid = "S-1-5-" & strSystemDomainSid & "-514"
			strSpecialDomain = strSystemDomainName
		End if
	Case Else
		GetStandardSid = ""
	End Select
	If strSpecialDomain <> "" then
		If ucase(strSpecialDomain) <> ucase(strDomain) then
			If debug_on then Call PrintMsg(" - Changing entered user/group: """ & strFullName & """ to """ & strSpecialDomain & "\" & strName & """")
			strFullName = strSpecialDomain & "\" & strName
			strDomain = strSpecialDomain
		End if
	End if
	If strDomain = "" then
		If strDefaultDomain <> "" then
			If debug_on then Call PrintMsg(" - Changing entered user/group: """ & strFullName & """ to """ & strDefaultDomain & "\" & strName & """")
			strFullName = strDefaultDomain & "\" & strName
			strDomain = strDefaultDomain
		End if
	End if
	Exit Do		'We really didn't want to loop
    Loop

    If debug_on then 
	If GetStandardSid <> "" then
		Call PrintMsg("GetStandardSid: Return = " & GetStandardSid)
	Else
		Call PrintMsg("GetStandardSid: Return = NOTHING")
	End if
    End if
    Call blnErrorOccurred(" occurred while in the GetStandardSid routine. (Msg#1301)")
    If debug_on then Call PrintMsg("GetStandardSid: Exit")

End Function

'********************************************************************
'*
'* Function SetACE()
'*
'* Purpose: Returns Win32_Ace object for objTrustee or Nothing if not found
'*
'* Input:   AccessMask, AceFlags, AceType, objTrustee
'*
'* Output:  Win32_Ace object is returned, Nothing if not found
'*
'********************************************************************
Function SetACE(byval AccessMask, byval AceFlags, byval AceType, byval objTrustee)
    ON ERROR RESUME NEXT

    Dim objACE

    If debug_on then Call PrintMsg("SetACE: Enter")

    Do		'For Error Control

	set objACE = objService.Get("Win32_Ace").Spawninstance_
	If blnErrorOccurred(" occurred while getting Win32_Ace.Spawninstance object (Msg#1401)") Then Exit Do

	objACE.Properties_.item("AccessMask") = AccessMask
	objACE.Properties_.item("AceFlags") = AceFlags
	objACE.Properties_.item("AceType") = AceType
	objACE.Properties_.item("Trustee") = objTrustee

	set SetACE = objACE
	Exit Do
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objACE = Nothing

    If debug_on then 
	If SetACE is Nothing then
		Call PrintMsg("SetACE: Return = " & "Nothing")
	Else
		Call PrintMsg("SetACE: Return = " & "Win32_Ace object")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the SetACE routine. (Msg#1402)")
    If debug_on then Call PrintMsg("SetACE: Exit")

End Function

'********************************************************************
'*
'* Sub GetDefaultNames()
'* Purpose: Return a Domain name and Computer Name
'* Input:   None
'* Output:  Sets Global Vars for Domain Name and Computer Name
'*
'********************************************************************

Sub GetDefaultNames()

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetDefaultNames: Enter")

    Dim objSystemSet, objSystem, intRole


    'Put statements in loop to be able to drop out and clear variables
    Do
        Set objSystemSet = objService.ExecQuery("Select Name, Domain, DomainRole from Win32_ComputerSystem",,0)
	If blnErrorOccurred(" occurred setting objSystemSet = objService.ExecQuery(""Select Name, Domain, DomainRole from Win32_ComputerSystem"",,0). (Msg#1501)") Then Exit Do

	strDefaultDomain = ""
	strSystemDomainName = ""

    	for each objSystem in objSystemSet
		If objSystem.Name <> "" then
			If objSystem.Domain <> "" then
				strSystemDomainName = objSystem.Domain
			Else
				strSystemDomainName = objSystem.Name
			End if 
			intRole = objSystem.DomainRole
			Select Case intRole
			Case 0 		'Standalone Workstation
				strDefaultDomain = objSystem.Name
			Case 1		'Member Workstation
				If CONST_USE_LOCAL_FOR_NON_DCs then
					strDefaultDomain = objSystem.Name
				Else
					strDefaultDomain = objSystem.Domain
				End if
			Case 2		'Standalone Server
				strDefaultDomain = objSystem.Name
			Case 3		'Member Server
				If CONST_USE_LOCAL_FOR_NON_DCs then
					strDefaultDomain = objSystem.Name
				Else
					strDefaultDomain = objSystem.Domain
				End if
			Case 4		'Backup Domain Controller
				strDefaultDomain = objSystem.Domain
			Case 5		'Primary Domain Controller
				strDefaultDomain = objSystem.Domain
			Case Else	'Don't know this one...so do nothing
				strDefaultDomain = ""
			End select
			Exit For
		End if
    	next
	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objSystem = Nothing
    Set objSystemSet = Nothing

    Call blnErrorOccurred(" occurred while in the GetDefaultNames routine. (Msg#1502)")
    If debug_on then Call PrintMsg("GetDefaultNames: Exit")

End Sub

'********************************************************************
'*
'* Sub GetDefaultDomainSid()
'* Purpose: Gets the Domain Sid by getting the Administrator account and extracting the Domain Sid portion of the sid
'* Input:   None
'* Output:  Sets Global var strSystemDomainSid to the Domain Sid found
'*
'********************************************************************

Sub GetDefaultDomainSid()

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetDefaultDomainSid: Enter")

    Dim objSystemSet, objSystem, strQuery


    'Put statements in loop to be able to drop out and clear variables
    Do
	strSystemDomainSid = ""
	strQuery = "Select Sid from Win32_Group WHERE Domain=""" & strSystemDomainName & """ and Name=""Domain Admins"""

        Set objSystemSet = objService.ExecQuery(strQuery,,0)
	If blnErrorOccurred(" occurred setting objSystemSet = objService.ExecQuery(" & strQuery & ",,0). (Msg#1601)") Then Exit Do

    	for each objSystem in objSystemSet
		If objSystem.Sid <> "" then
			If Left(objSystem.Sid,6)="S-1-5-" and Right(objSystem.Sid,4) = "-512" then
				'This is the right account to look at
				strSystemDomainSid = Mid(objSystem.Sid, 7)
				strSystemDomainSid = Left(strSystemDomainSid, Len(strSystemDomainSid)-4)
				Exit For
			End if
		End if
    	next
	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objSystem = Nothing
    Set objSystemSet = Nothing

    Call blnErrorOccurred(" occurred while in the GetDefaultDomainSid routine. (Msg#1602)")
    If debug_on then Call PrintMsg("GetDefaultDomainSid: Exit")

End Sub


'********************************************************************
'* Function GetAccountObj()
'* Purpose: Returns User/group Object or Nothing if not found
'* Input:   strDomain, strName
'* Output:  Account Object is returned
'********************************************************************
Function GetAccountObj(byval strDomain, byval strName)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetAccountObj: Enter")

    Do
	Set GetAccountObj = Nothing

    	Set GetAccountObj = GetUserObj(objService, strDomain, strName)
	If GetAccountObj is Nothing then
		Set GetAccountObj = GetGroupObj(objService, strDomain, strName)
    	End if

	If GetAccountObj is Nothing then
		If debug_on then Call PrintMsg("GetAccountObj: Couldn't find a User or Group object, but will try a second time")
    		Set GetAccountObj = GetUserObj(objLocalService, strDomain, strName)
		If GetAccountObj is Nothing then
			Set GetAccountObj = GetGroupObj(objLocalService, strDomain, strName)
	    	End if
	End if

	Exit Do
    Loop

    If debug_on then 
	If GetAccountObj is Nothing then
		Call PrintMsg("GetAccountObj: Return = " & "Nothing")
	Else
		Call PrintMsg("GetAccountObj: Return = " & "Win32_UserAccount or Win32_Group object")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the GetAccountObj routine. (Msg#1701)")
    If debug_on then Call PrintMsg("GetAccountObj: Exit")

End Function

'********************************************************************
'* Function GetUserObj()
'* Purpose: Returns User's Object
'* Input:   objService, strDomain, strName
'* Output:  UserObject is returned, Nothing if not found
'********************************************************************
Function GetUserObj(ByRef objTempService, byval strDomain, byval strName)

    ON ERROR RESUME NEXT

    If debug_on then 
	Call PrintMsg("GetUserObj: Enter")
	Call PrintMsg("GetUserObj: strDomain = " & strDomain)
	Call PrintMsg("GetUserObj: strName = " & strName)
    End if

    Dim objSet, objUserAccount
    Dim strWBEMClass, I
    Dim strQuery
    Set GetUserObj = Nothing

    'Put statements in loop to be able to drop out and clear variables
    Do
	strWBEMClass = "Win32_UserAccount"
  
	'Get the first instance
	If strName <> ""  then
		If strDomain <> "" then
	        	strQuery = "Domain = """ & strDomain & """ and Name = """ & strName & """"
		Else 
	        	strQuery = "Name = """ & strName & """"
		End if
        	strQuery = "Select Sid, Domain, Name from " & strWBEMClass & " Where " & strQuery & " and SIDType = 1" 'This means just users
        	Set objSet = objTempService.ExecQuery(strQuery,,0)
        	If blnErrorOccurred (" obtaining the " & strWBEMClass & "  (Msg#1801)") Then Exit Do
	Else
        	Call PrintMsg("Error: UserName required when obtaining the Win32_UserAccount")
		Exit Do 
	End If

	If objSet.Count = 0 then Exit Do      	'Username not found

	For Each objUserAccount In objSet
		Set GetUserObj = objUserAccount
    	Next

	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objSet = Nothing
    Set objUserAccount = Nothing

    If debug_on then 
	If GetUserObj is Nothing then
		Call PrintMsg("GetUserObj: Return = " & "Nothing")
	Else
		Call PrintMsg("GetUserObj: Return = " & "Win32_UserAccount object")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the GetUserObj routine. (Msg#1802)")
    If debug_on then Call PrintMsg("GetUserObj: Exit")

End Function

'********************************************************************
'* Function GetGroupobj()
'* Purpose: Returns Groups's Object
'* Input:   strDomain, strName
'* Output:  GroupObject is returned, Nothing if not found
'********************************************************************
Function GetGroupobj(ByRef objTempService, byval strDomain, byval strName)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("GetGroupobj: Enter")

    Dim objSet, objUserAccount
    Dim strWBEMClass, I
    Dim strQuery
    Set GetGroupobj = Nothing

    'Put statements in loop to be able to drop out and clear variables
    Do
	If debug_on then Call PrintMsg("GetGroupobj: strDomain = " & strDomain)
	If debug_on then Call PrintMsg("GetGroupobj: strName   = " & strName)
	If strName <> ""  then
		If strDomain <> "" then
			strQuery = "Domain=""" & strDomain & """,Name=""" & strName & """"
		Else 
	        	strQuery = "Name=""" & strName & """"
		End if
    	Else
        	Call PrintMsg("Error: GroupName required when obtaining the Win32_Group object")
		Exit Do 
	End If

	set objUserAccount = objTempService.Get("Win32_Group." & strQuery)
	If Err.Number = -2147217406 then 
		Err.Clear
		Exit Do
	End if
	If blnErrorOccurred (" setting objUserAccount = objTempService.Get(""Win32_Group." & strQuery & ")") Then Exit Do

	If Not objUserAccount is Nothing then
		Set GetGroupobj = objUserAccount 
	End if

	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objSet = Nothing
    Set objUserAccount = Nothing

    If debug_on then 
	If GetGroupobj is Nothing then
		Call PrintMsg("GetGroupobj: Return = " & "Nothing")
	Else
		Call PrintMsg("GetGroupobj: Return = " & "Win32_Group object")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the GetGroupobj routine. (Msg#1901)")
    If debug_on then Call PrintMsg("GetGroupobj: Exit")

End Function


'********************************************************************
'*
'* Function SECString()
'* Purpose: Converts SEC bitmask to a string
'* Input:   intBitmask - integer and ReturnLong - Boolean
'* Output:  String Array
'*
'********************************************************************

Function SECString(byval intBitmask, byval ReturnLong)

    On Error Resume Next
    Dim LongName, X

    If debug_on then Call PrintMsg("SECString: Enter")

    SECString = ""

    Do
	If debug_on then Call PrintMsg("SECString: intBitmask = " & intBitmask)
		
	For X = LBound(Perms_LStr) to UBound(Perms_LStr)
    		If ((intBitmask And Perms_Const(X)) = Perms_Const(X)) then
			If Perms_SStr(X) <> "" then
				SECString = SECString & Perms_SStr(X)
			End if
    		End if
	Next

	If debug_on then Call PrintMsg("SECString: SECString = " & SECString)
	Select Case SECString
	Case "DCBA987654321", "EDCBA987654321"
		SECString = "F"								'Full control
		LongName = "Full Control"	
	Case "BA98654321", "EBA98654321"
		SECString = "M"								'Modify
		LongName = "Modify"
	Case "B98654321", "EB98654321"
		SECString = "XW"								'Read, Write and Execute
		LongName = "Read, Write and Execute"
	Case "B9854321", "EB9854321"
		SECString = "RW"								'Read and Write
		LongName = "Read and Write"
	Case "B8641", "EB8641"
		SECString = "X"								'Read and Execute
		LongName = "Read and Execute"
	Case "B841", "EB841"
		SECString = "R"								'Read
		LongName = "Read"
	Case "9532", "E9532"
		SECString = "W"								'Write
		LongName = "Write"
	Case Else
		If SECString = "" then
			LongName = "Special (Unknown)"
			If debug_on then
				LongName = "Unknown (" & intBitmask & ")"
			End if
		Else
			If LEN(SECString) = 1 then
				For X = LBound(Perms_SStr) to UBound(Perms_SStr)
					If StrComp(SECString,Perms_SStr(X),1) = 0 Then
						LongName = "Advanced (" & Perms_LStr(X) & ")"
						Exit For
					End if
				Next
			Else
				LongName = "Special (" & SECString & ")"
			End if
		End if
	End Select

	Exit Do
    Loop

    If ReturnLong Then SECString = LongName

    If debug_on then Call PrintMsg("SECString: Return = " & SECString)

    Call blnErrorOccurred(" occurred while in the SECString routine. (Msg#2001)")
    If debug_on then Call PrintMsg("SECString: Exit")

End Function

'********************************************************************
'*
'* Function SECBitMask()
'* Purpose: Converts string to a SEC bitmask
'* Input:   string similiar to RWX
'* Output:  bitmask integer
'*
'********************************************************************

Function SECBitMask(byval strsec)

    On Error Resume Next

    If debug_on then Call PrintMsg("SECBitMask: Enter")

    SECBitMask = 0 'No Access

    Dim x, i, thischar, OurPermsSelected()

    Do
	SECBitMask = 0
	If strsec = "" then Exit Do

	Redim OurPermsSelected(0)

	'Now lets fill OurPermsSelected with the bitmask for the code found (duplicates will be ignored)
	For x = 1 to Len(strsec)
		thischar = Mid(strsec, x, 1)
		'Lets not do the array if this character isn't a known character
		If IsPermCompatible(thischar) then
			For i = LBound(Perms_LStr) to UBound(Perms_LStr)
				If StrComp(thischar,Perms_SStr(i),1) = 0 Then
					Call AddStringToArray(OurPermsSelected, Perms_Const(i), -1)
				End if
			Next
		Else
			Call PrintMsg("PERM Code: """ & thischar & """ not found and will be ignored.")
		End if
	Next
	'We now add up the OurPermsSelected
	SECBitMask = Perms_Const(0)
	For i = LBound(OurPermsSelected) to UBound(OurPermsSelected)
		SECBitMask = SECBitMask + OurPermsSelected(i)
	Next
	Exit Do
    Loop

    If debug_on then Call PrintMsg("SECBitMask: Return = " & SECBitMask)

    Call blnErrorOccurred(" occurred while in the SECBitMask routine. (Msg#2101)")
    If debug_on then Call PrintMsg("SECBitMask: Exit")

End Function

'********************************************************************
'*
'* Function ConvertToOurCodes()
'* Purpose: Changes the known ACL codes into our codes
'* Input:   String of known codes
'* Output:  String of our codes
'*
'********************************************************************

Function ConvertToOurCodes(byval strKnownCodes)

	On Error Resume Next

	Dim TempString

	If debug_on then Call PrintMsg("ConvertToOurCodes: Enter")

	'Lets turn the special codes into our own codes
	TempString = Replace(strKnownCodes, "F", "DCBA987654321", 1, -1, 1)	'Full control
	TempString = Replace(TempString, "M", "BA98654321", 1, -1, 1)		'Modify
	TempString = Replace(TempString, "X", "B8641", 1, -1, 1)			'Read and Execute
	TempString = Replace(TempString, "L", "B8641", 1, -1, 1)			'List Folder Contents (same as Read and Execute for This Folder and Subfolders only)
	TempString = Replace(TempString, "R", "B841", 1, -1, 1)				'Read
	ConvertToOurCodes = Replace(TempString, "W", "9532", 1, -1, 1)		'Write

	If debug_on then Call PrintMsg("ConvertToOurCodes: Return = " & ConvertToOurCodes)

	Call blnErrorOccurred(" occurred while in the ConvertToOurCodes routine. (Msg#2201)")
	If debug_on then Call PrintMsg("ConvertToOurCodes: Exit")

End Function

'********************************************************************
'*
'* Function StringAceFlag()
'* Purpose: Changes the AceFlag into a string
'* Input:   numAceFlag =      This items ACEFlag
'*          numControlFlags = This Descriptors AceFlag
'*          FlagToCheck =     This lists Auto_Inherited bit to check for
'*          ReturnShort =     If True then we will return a short version
'*          ReturnAceFlags =  Final numAceFlags value after changes (leaves real one alone
'* Output:  String of our codes
'*
'********************************************************************

Function StringAceFlag(ByVal numAceFlags, ByVal numControlFlags, ByVal FlagToCheck, ByVal ReturnShort, ByRef ReturnAceFlags)

    On Error Resume Next

    If debug_on then Call PrintMsg("StringAceFlag: Enter")

    Dim TempShort, TempLong

    Do
	If numAceFlags = 0 then 
		TempShort = "Implicit"
		TempLong = "This Folder Only"
		Exit Do
	End if
	If numAceFlags > FAILED_ACCESS_ACE_FLAG then
		numAceFlags = numAceFlags - FAILED_ACCESS_ACE_FLAG
	End if
	If numAceFlags > SUCCESSFUL_ACCESS_ACE_FLAG then
		numAceFlags = numAceFlags - SUCCESSFUL_ACCESS_ACE_FLAG
	End if
	If ((numAceFlags And INHERITED_ACE) = INHERITED_ACE) then
		TempShort = "Inherited"
		numAceFlags = numAceFlags - INHERITED_ACE
		TempLong = "Inherited"
	Else
		TempShort = "Implicit"
		TempLong = "Implicit"
	End If

	ReturnAceFlags = numAceFlags 

	If numControlFlags > DENY_INHERIT then
		numControlFlags = numControlFlags - DENY_INHERIT
	End if
	If numControlFlags > ALLOW_INHERIT then
		numControlFlags = numControlFlags - ALLOW_INHERIT
	End if

	Select Case numAceFlags 
	Case 0
		TempLong = "This Folder Only"
	Case 1							'OBJECT_INHERIT_ACE
		TempLong = "This Folder and Files"
	Case 2							'CONTAINER_INHERIT_ACE
		TempLong = "This Folder and Subfolders"
	Case 3
		TempLong = "This Folder, Subfolders and Files"
	Case 9
		TempLong = "Files Only"
	Case 10
		TempLong = "Subfolders only"
	Case 11
		TempLong = "Subfolders and Files only"
	Case Else
		If ((numControlFlags And FlagToCheck) = FlagToCheck) then
			TempShort = "Inherited"
			TempLong = "Inherited"
		End if
	End Select
	Exit Do
    Loop

    If ReturnShort then
	StringAceFlag = TempShort
    Else
	StringAceFlag = TempLong
    End if

    If debug_on then Call PrintMsg("StringAceFlag: Return = " & StringAceFlag)

    Call blnErrorOccurred(" occurred while in the StringAceFlag routine. (Msg#2301)")
    If debug_on then Call PrintMsg("StringAceFlag: Exit")

End Function

'********************************************************************
'*
'* Function StringSACLAceFlag()
'* Purpose: Changes the AceFlag into a string
'* Input:   numAceFlag
'* Output:  String of our codes
'*
'********************************************************************

Function StringSACLAceFlag(byval numAceFlags)

    On Error Resume Next

    If debug_on then Call PrintMsg("StringSACLAceFlag: Enter")

    Do
	If ((numAceFlags And (SUCCESSFUL_ACCESS_ACE_FLAG + FAILED_ACCESS_ACE_FLAG)) = (SUCCESSFUL_ACCESS_ACE_FLAG + FAILED_ACCESS_ACE_FLAG)) then 
		StringSACLAceFlag = "All"
		Exit Do
	End if
	If ((numAceFlags And SUCCESSFUL_ACCESS_ACE_FLAG) = SUCCESSFUL_ACCESS_ACE_FLAG) then 
		StringSACLAceFlag = "Success"
		Exit Do
	End If
	If ((numAceFlags And FAILED_ACCESS_ACE_FLAG) = FAILED_ACCESS_ACE_FLAG) then 
		StringSACLAceFlag = "Fail"
		Exit Do
	End if
	StringSACLAceFlag = "Unknown"
	Exit Do
    Loop


    If debug_on then Call PrintMsg("StringSACLAceFlag: Return = " & StringSACLAceFlag)

    Call blnErrorOccurred(" occurred while in the StringSACLAceFlag routine. (Msg#2401)")
    If debug_on then Call PrintMsg("StringSACLAceFlag: Exit")

End Function



'********************************************************************
'*
'* Function IsPermCompatible()
'* Purpose: Makes sure the string is Perm access right compatible
'* Input:   Perm string
'* Output:  TRUE if it is compatible, FALSE if it isn't
'*
'********************************************************************

Function IsPermCompatible(byval thePerm)

    On Error Resume Next

    If debug_on then Call PrintMsg("IsPermCompatible: Enter")

    Do

	IsPermCompatible = FALSE	'Assumed FALSE
	Dim i, CurrentChar, PermList

	if thePerm = "" then Exit Do

	IsPermCompatible = TRUE	'We set it to TRUE so any character that is not a Perm will change it to FALSE
	PermList = Join(Perms_SStr,"")

	For i = 1 to Len(thePerm)
		CurrentChar = Mid(thePerm,i,1)
		If InStr(1, PermList, CurrentChar, 1) = 0 Then 
			IsPermCompatible = FALSE
			Call PrintMsg("")
			Call PrintMsg("Error: Permission Code not recognized: " & CurrentChar)
			Call PrintMsg("")
		End if
	Next
	Exit Do
    Loop

    If debug_on then Call PrintMsg("IsPermCompatible: Return = " & IsPermCompatible)

    Call blnErrorOccurred(" occurred while in the IsPermCompatible routine. (Msg#2501)")
    If debug_on then Call PrintMsg("IsPermCompatible: Exit")

End Function


'********************************************************************
'*
'* Function IsPermGUI()
'* Purpose: Checks Perm to see if its just GUI letters or if it contains special codes.
'* Input:   thePerm as String
'* Output:  Returns Boolean
'*
'********************************************************************

Function IsPermGUI(byval thePerm)

    On Error Resume Next

    If debug_on then WScript.Echo "IsPermGUI: Enter"

    Do

	IsPermGUI = FALSE	'Assumed FALSE

	Dim GUIPermList, i, CurrentChar

	if thePerm = "" then Exit Do

	GUIPermList = "FMXLRW"

	For i = 1 to Len(thePerm)
		CurrentChar = UCASE(Mid(thePerm,i,1))
		If InStr(1, GUIPermList, CurrentChar, 1) = 0 Then 
			If debug_on then WScript.Echo "IsPermGUI: " & CurrentChar & " is not a GUI perm"
			Exit Do
		End if
	Next

	IsPermGUI = TRUE 'If we get here then all the characters are GUI perms so we return True

	Exit Do

    Loop

    If debug_on then WScript.Echo "IsPermGUI: Return = " & IsPermGUI

    Call blnErrorOccurred(" occurred while in the IsPermGUI routine. (Msg#2601)")
    If debug_on then WScript.Echo "IsPermGUI: Exit"

End Function


'********************************************************************
'*
'* Function HasWildcardCharacters()
'* Purpose: Tells us if the file inputed has wildcard characters
'* Input:   Filename
'* Output:  TRUE if it has wildcard characters, FALSE if it doesn't
'*
'********************************************************************

Function HasWildcardCharacters(byval theFilename)

    On Error Resume Next

    If debug_on then Call PrintMsg("HasWildcardCharacters: Enter")

    HasWildcardCharacters = FALSE

    If InStr(1, theFilename, "*",1) > 0 Then 
	HasWildcardCharacters = TRUE
    End if
    If InStr(1, theFilename, "?",1) > 0 Then 
	HasWildcardCharacters = TRUE
    End if

    If debug_on then 
	If HasWildcardCharacters then
		Call PrintMsg("HasWildcardCharacters: Return = TRUE")
	Else
		Call PrintMsg("HasWildcardCharacters: Return = FALSE")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the HasWildcardCharacters routine. (Msg#2701)")
    If debug_on then Call PrintMsg("HasWildcardCharacters: Exit")

End Function

'********************************************************************
'*
'* Function DoesItMatch()
'* Purpose: To see if the path/file matches the query
'*          We want to do the same query functions as the DOS command DIR.
'* Input:   ThisPath - Path to check
'* Output:  TRUE if thispath matches the Global filename_var variable
'*
'********************************************************************

Function DoesItMatch(byval ThisPath)

    On Error Resume Next

    If debug_on then Call PrintMsg("DoesItMatch: Enter")

    Dim qryBaseName, qryFileExtension
    Dim thisPathBaseName, thisPathFileExtension
    Dim BaseNameMatches, ExtensionMatches
    Dim x, i, ThisChar

    Do
	'Assume FALSE
	DoesItMatch = FALSE
	BaseNameMatches = FALSE
	ExtensionMatches = FALSE

	qryBaseName = fso.GetBaseName(filename_var)
	qryFileExtension = fso.GetExtensionName(filename_var)
	thisPathBaseName = fso.GetBaseName(ThisPath)
	thisPathFileExtension = fso.GetExtensionName(ThisPath)

	If QryBaseNameHasWildcards then 
		BaseNameMatches = DoesThisOneMatch(thisPathBaseName, qryBaseName)
	Else
		BaseNameMatches = (StrComp(thisPathBaseName, qryBaseName,1) = 0)
	End if

	If NOT BaseNameMatches then Exit Do 	'Why continue testing, if it failed already

	If InStr(1, filename_var, ".",1) > 0 Then 
		If qryFileExtension <> "" then
			If QryExtensionHasWildcards then 
				ExtensionMatches = DoesThisOneMatch(thisPathFileExtension, qryFileExtension)
			Else
				ExtensionMatches = (StrComp(thisPathFileExtension, qryFileExtension,1) = 0)
			End if  
		Else
			'If queryFileExtensions is blank then we want only directories.
			If thisPathFileExtension = "" then ExtensionMatches = TRUE
		End if
	Else
		'Consider no . meaning they want all files and directories regardless of extension.
		ExtensionMatches = TRUE
	End if
	DoesItMatch = BaseNameMatches AND ExtensionMatches
	Exit Do
    Loop

    If debug_on then Call PrintMsg("DoesItMatch: Return = " & DoesItMatch)

    Call blnErrorOccurred(" occurred while in the DoesItMatch routine. (Msg#2801)")
    If debug_on then Call PrintMsg("DoesItMatch: Exit")

End Function

'********************************************************************
'*
'* Function DoesThisOneMatch()
'* Purpose: To see if the string matches the query
'* Input:   ThisString - String to check, ThisQuery - Query with wildcard characters
'* Output:  TRUE if thispath matches querypath
'*
'********************************************************************

Function DoesThisOneMatch(byval ThisString, byval ThisQuery)

    On Error Resume Next

    If debug_on then Call PrintMsg("DoesThisOneMatch: Enter")

    Dim x, i, QueryArray, TempString, LastOneWasAStar

    Do
	DoesThisOneMatch = FALSE 					'Assume it doesn't match

	If ThisQuery = "" then
		If ThisString = "" then DoesThisOneMatch = TRUE
		Exit Do
	End if
	'Object is to break out the Query string into an array, where each member will either be *, ? or a string of characters
	ThisQuery = Replace(ThisQuery, "*", CHR(130) & "*" & CHR(130), 1, -1, 1) 	'Lets deliminate the string
	ThisQuery = Replace(ThisQuery, "?", CHR(130) & "?" & CHR(130), 1, -1, 1) 	'Now ThisQuery is deliminated
	ThisQuery = Replace(ThisQuery, CHR(130) & CHR(130), CHR(130), 1, -1, 1) 	'Removes double CHR(130)'s in the middle
	If Left(ThisQuery, 1)= CHR(130) then
		ThisQuery = Mid(ThisQuery, 2)				'Removes First CHR(130)
	End if
	If Right(ThisQuery, 1) = CHR(130) then
		ThisQuery = Left(ThisQuery, Len(ThisQuery) - 1)		'Removes Last CHR(130)
	End if
	QueryArray = Split(ThisQuery, CHR(130), -1, 1) 		'Now we have an array with no blank members
	TempString = ThisString
	LastOneWasAStar = FALSE
	For x = LBound(QueryArray) to UBound(QueryArray)
		Select Case QueryArray(x)
		Case "*"						'Do Nothing because * means 0 to any length characters
			LastOneWasAStar = TRUE
		Case "?"						'Reduce String by 1 when ? used
			If Len(TempString) > 0 then
				TempString = Mid(TempString, 2)
			Else
				Exit Do				'We can't do the ? because there are no characters left, so it doesn't match
			End if
			LastOneWasAStar = FALSE
		Case Else						'Find First Occurance of QueryArray(x) in remaining string
			i = InStr(1, TempString, QueryArray(x),1)
			If i > 0 Then 
				If Not LastOneWasAStar and i > 1 then 	'If found, and lastchar wasn't a * then make sure its in position 1
					Exit Do
				End if
				TempString = Mid(TempString, i + Len(QueryArray(x)))
			Else						'Didn't find a match so we exit
				Exit Do
			End if
			LastOneWasAStar = FALSE
		End Select
	Next
	If Len(TempString) > 0 And Not LastOneWasAStar then		'There were more characters in string but the last Query character wasn't a *, so this is not a match.
		Exit Do
	End If
	DoesThisOneMatch = TRUE					'If we got here, either the last query char was * or String is empty now, so its a match
	Exit Do
    Loop

    If debug_on then Call PrintMsg("DoesThisOneMatch: Return = " & DoesThisOneMatch)

    Call blnErrorOccurred(" occurred while in the DoesThisOneMatch routine. (Msg#2901)")
    If debug_on then Call PrintMsg("DoesThisOneMatch: Exit")
End Function

'********************************************************************
'*
'* Function AddObjectToArray()
'* Purpose: Adds an Object to an array
'* Input:   objArray and objMember
'* Output:  Updates global arrays
'*
'********************************************************************

Function AddObjectToArray(ByRef objArray, ByRef objMember, byval intUseIndex)

    Dim intUBound, UseThisNumber
    On Error Resume Next

    If debug_on then Call PrintMsg("AddObjectToArray: Enter")

    intUBound = 0
    AddObjectToArray = FALSE
    Do	'For error control
	If NOT IsObject(objMember) then 
		Exit Do
	End if
	If objMember is Nothing then 
		Exit Do
	End if
	If NOT IsArray(objArray) then 
		Exit Do
	End if

	intUBound = UBound(objArray, 1)
	If blnErrorOccurred (" obtaining the UBound(objArray) (Msg#3001)") Then Exit Do

	Select case intUseIndex 
	Case -1
		'We will always increment by 1 so the first member is 0 or blank
		intUBound = intUBound + 1
		UseThisNumber = intUBound
	Case 0
		If intUBound = 0 then
			If NOT IsObject(objArray(0)) then 
				UseThisNumber = 0
			Else
				intUBound = intUBound + 1
				UseThisNumber = intUBound
			End if
			If blnErrorOccurred (" when checking objArray(0) (Msg#3002)") Then Exit Do
		Else
			intUBound = intUBound + 1
			UseThisNumber = intUBound
		End if
	Case Else
		If intUseIndex > intUBound then
			intUBound = intUseIndex 
		End if
		UseThisNumber = intUseIndex
	End select

	ReDim Preserve objArray(intUBound)
	If blnErrorOccurred (" when doing command ReDim Preserve objArray(" & intUBound & ") (Msg#3003)") Then Exit Do

	Set objArray(UseThisNumber) = objMember
	If blnErrorOccurred (" when Setting objArray(UseThisNumber) = objMember (Msg#3004)") Then Exit Do
	AddObjectToArray = TRUE
	Exit Do
    Loop

    If debug_on then Call PrintMsg("AddObjectToArray: Return = " & AddObjectToArray)

    Call blnErrorOccurred(" occurred while in the AddObjectToArray routine. (Msg#3005)")
    If debug_on then Call PrintMsg("AddObjectToArray: Exit")

End Function

'********************************************************************
'*
'* Function ClearObjectArray()
'* Purpose: Clears an Object array
'* Input:   objArray
'* Output:  Updates global arrays
'*
'********************************************************************

Function ClearObjectArray(ByRef objArray)

    Dim intLBound, intUBound, x
    On Error Resume Next

    If debug_on then Call PrintMsg("ClearObjectArray: Enter")

    intLBound = 0
    intUBound = 0
    ClearObjectArray = FALSE
    Do	'For error control
	If NOT IsArray(objArray) then 
		Exit Do
	End if

	intLBound = LBound(objArray, 1)
	If blnErrorOccurred (" obtaining the LBound(objArray) (Msg#3101)") Then Exit Do

	intUBound = UBound(objArray, 1)
	If blnErrorOccurred (" obtaining the UBound(objArray) (Msg#3102)") Then Exit Do


	For X = intLBound to intUBound
		Set objArray(x) = Nothing
	Next

	ReDim objArray(0)
	If blnErrorOccurred (" when doing command ReDim objArray(0) (Msg#3103)") Then Exit Do

	ClearObjectArray = TRUE
	Exit Do
    Loop

    If debug_on then 
	If ClearObjectArray then
		Call PrintMsg("ClearObjectArray: Return = TRUE")
	Else
		Call PrintMsg("ClearObjectArray: Return = FALSE")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the ClearObjectArray routine. (Msg#3104)")
    If debug_on then Call PrintMsg("ClearObjectArray: Exit")

End Function

'********************************************************************
'*
'* Function AddStringToArray()
'* Purpose: Adds a string to an array (allowing duplicates) and allows for a member index number
'* Input:   Array and Member
'* Output:  Returns Index Number
'* Notes:   If intUseIndex is -1 then we just want to ReDim the array to be 1 larger and use the
'*          last member. If its any other number than we want to use that number if available.
'*
'********************************************************************

Function AddStringToArray(ByRef theArray, byval theMember, byval intUseIndex)

    On Error Resume Next

    Dim UseThisNumber

    If debug_on then Call PrintMsg("AddStringToArray: Enter")

    Do

	AddStringToArray = UBound(theArray)

	If intUseIndex <> -1 then
		If intUseIndex > AddStringToArray then
			AddStringToArray = intUseIndex 
		End if
		UseThisNumber = intUseIndex
	Else
		'We will always increment by 1 so the first member is 0 or blank
		AddStringToArray = AddStringToArray + 1
		UseThisNumber = AddStringToArray
	End if

	ReDim Preserve theArray(AddStringToArray)

	theArray(UseThisNumber) = theMember
	If blnErrorOccurred (" when Setting theArray(" & UseThisNumber & ") = """ & theMember & """ (Msg#3201)") Then Exit Do

	Exit Do
    Loop


    If debug_on then Call PrintMsg("AddStringToArray: Return = " & AddStringToArray)

    Call blnErrorOccurred(" occurred while in the AddStringToArray routine. (Msg#3202)")
    If debug_on then Call PrintMsg("AddStringToArray: Exit")

End Function


'********************************************************************
'*
'* Function SetMainVars()
'* Purpose: Checks a FilePath for existance and sets Global Var's
'* Input:   Nothing
'* Output:  Boolean TRUE if worked, FALSE if failed
'* Notes:   None
'*
'********************************************************************

Function SetMainVars()

    On Error Resume Next

    Dim strTempServer, strTempShare, objFileShare

    If debug_on then Call PrintMsg("SetMainVars: Enter")

    Do

	SetMainVars = FALSE
	strTempServer = ""
	strTempShare = ""
	If NOT GetServerNameString(filename_var, strTempServer, strTempShare) then 
		If strTempServer <> "" then
			'We shouldn't have gotten a server name, if we did then the first two characters were \\
			Call PrintMsg("Error, FileName looks like a UNC path without a ShareName.")
			Call PrintMsg("Script can not continue.")
			Exit Do
		End if
	Else
		strRemoteServerName = strTempServer
		strRemoteShareName = strTempShare
		RemoteServer_Used = TRUE
	End if

	'Create Locator object to connect to remote CIM object manager
	Set objLocator = CreateObject("WbemScripting.SWbemLocator")
	If blnErrorOccurred(" occurred in creating a locator object. (Msg#3301)") Then Exit Do

	Set objLocalService = objLocator.ConnectServer ("", "root/cimv2")
	If blnErrorOccurred(" occurred in connecting to Local WMI server. (Msg#3302)") Then Exit Do

	'Connect to the namespace which is either local or remote
	If RemoteServer_Used then
		If RemoteUserName_Used then
			Set objService = objLocator.ConnectServer (strRemoteServerName, "root/cimv2", strRemoteUserName, strRemotePassword)
		Else
			Set objService = objLocator.ConnectServer (strRemoteServerName, "root/cimv2")
		End if
		If blnErrorOccurred(" occurred in connecting to server. (Msg#3203)") Then Exit Do
	Else
		Set objService = objLocator.ConnectServer ("", "root/cimv2")
		If blnErrorOccurred(" occurred in connecting to server. (Msg#3204)") Then Exit Do
	End if

	objLocalService.Security_.impersonationlevel = 3

	objLocalService.Security_.Privileges.AddAsString "SeSecurityPrivilege", TRUE
	If blnErrorOccurred(" occurred setting SeSecurityPrivilege. (Msg#3205)") Then Exit Do

	ObjService.Security_.impersonationlevel = 3

	objService.Security_.Privileges.AddAsString "SeSecurityPrivilege", TRUE
	If blnErrorOccurred(" occurred setting SeSecurityPrivilege. (Msg#3206)") Then Exit Do


	If fso.GetBaseName(filename_var) <> "" then
		QryBaseNameHasWildcards = HasWildcardCharacters(fso.GetBaseName(filename_var))
	Else
		QryBaseNameHasWildcards = FALSE
	End if
	If fso.GetExtensionName(filename_var) <> "" then
		QryExtensionHasWildcards = HasWildcardCharacters(fso.GetExtensionName(filename_var))
	Else
		QryExtensionHasWildcards = FALSE
	End if

	If strRemoteShareName <> "" Then
		set objFileShare = objService.Get("Win32_Share.Name=""" & strRemoteShareName & """")
		If blnErrorOccurred(" occurred getting Win32_Share """ & strRemoteShareName & """. (Msg#3207)") Then Exit Do
		If objFileShare.Path <> "" then
			ActualDirPath = objFileShare.Path
			DisplayDirPath = "\\" & strRemoteServerName & "\" & strRemoteShareName
		Else
			Call PrintMsg("Error, Share """ & strRemoteShareName & """ does not have a Path set.")
			Call PrintMsg("Script can not continue.")
			Exit Do
		End if
		InitialfilenameAbsPath = fso.GetAbsolutePathName(Replace(filename_var, DisplayDirPath, ActualDirPath, 1, 1, 1))
	Else
		InitialfilenameAbsPath = fso.GetAbsolutePathName(filename_var)
	End if

	SetMainVars = TRUE
	Exit Do
    Loop

    'ClearObjects that could be set and aren't needed now
    Set objFileShare = Nothing

    If debug_on then 
	If SetMainVars then
		Call PrintMsg("SetMainVars: Return = TRUE")
	Else
		Call PrintMsg("SetMainVars: Return = FALSE")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the SetMainVars routine. (Msg#3208)")
    If debug_on then Call PrintMsg("SetMainVars: Exit")

End Function

'********************************************************************
'*
'* Function DisplayPathString()
'* Purpose: Changes path from actual path to Display path (shows UNC path if needed)
'* Input:   File path string
'* Output:  Display Path string
'*
'********************************************************************

Function DisplayPathString(byval strFilePath)

    On Error Resume Next

    Dim strTempServer, intShareStart, intShareEnd

    If debug_on then Call PrintMsg("DisplayPathString: Enter")

    Do
	If strFilePath = "" then Exit Do

	If strRemoteShareName <> "" Then
		DisplayPathString = Replace(strFilePath, ActualDirPath, DisplayDirPath, 1, 1, 1)
	Else
		DisplayPathString = strFilePath
	End if


	Exit Do
    Loop

    If debug_on then 
	Call PrintMsg("DisplayPathString: Return = TRUE")
    End if

    Call blnErrorOccurred(" occurred while in the GetServerNameString routine. (Msg#3301)")
    If debug_on then Call PrintMsg("GetServerNameString: Exit")

End Function

'********************************************************************
'*
'* Function GetServerNameString()
'* Purpose: Gets a servername from the file path if exists
'* Input:   File path string, ServerName and ShareName
'* Output:  Boolean
'* Notes:   This will only work if \\ is the first two characters of filepath
'*
'********************************************************************

Function GetServerNameString(byval strFilePath, byref strServerName, byref strShareName)

    On Error Resume Next

    Dim strTempServer, intShareStart, intShareEnd

    If debug_on then Call PrintMsg("GetServerNameString: Enter")

    Do

	GetServerNameString = FALSE
	If strFilePath = "" then Exit Do
	If Left(strFilePath,2) <> "\\" then Exit Do
	If LEN(strFilePath) < 3 then Exit Do

	strTempServer = Mid(strFilePath,3)
	intShareStart = InStr(1, strTempServer, "\",1)
	If intShareStart > 0 then
		strServerName = Left(strTempServer,intShareStart-1)
		strShareName = Mid(strTempServer,intShareStart + 1)
		intShareEnd = InStr(1, strShareName, "\",1)
		If intShareEnd > 0 then
			strShareName = Left(strShareName,intShareEnd-1)
		End if
	Else
		strServerName = strTempServer
		Exit Do
	End if 

	GetServerNameString = TRUE
	Exit Do
    Loop

    If debug_on then 
	If GetServerNameString then
		Call PrintMsg("GetServerNameString: Return = TRUE")
	Else
		Call PrintMsg("GetServerNameString: Return = FALSE")
	End if
    End if

    Call blnErrorOccurred(" occurred while in the GetServerNameString routine. (Msg#3401)")
    If debug_on then Call PrintMsg("GetServerNameString: Exit")

End Function

'********************************************************************
'*
'* Function IsOSSupported()
'* Purpose: This function is responsible for determining which operating system we are
'*          running on and if its Windows 2000
'* Input:   None
'* Output:  Boolean (True means its Windows 2000)
'*
'********************************************************************

Function IsOSSupported()

    On Error Resume Next

    If debug_on then Call PrintMsg("IsOSSupported: Enter")

    Dim objShell, OSVer
	
    IsOSSupported = False

    Do
	Set objShell = CreateObject("Wscript.Shell")
	OSVer = objShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\CurrentVersion")
	If blnErrorOccurred(" occurred reading the OS version information from the registry! (Msg#3501)") Then Exit Do

	If debug_on then 
		Call PrintMsg("IsOSSupported: OSVer = " & OSVer)
	End if

	Select Case OSVer
	Case "5.0", "5.1", "5.2"
		IsOSSupported = True
	Case Else
		Call PrintMsg("")
		Call PrintMsg("************************************************")
		Call PrintMsg("* Script not tested on this version of Windows *")
		Call PrintMsg("************************************************")
		Call PrintMsg("")
		Call PrintMsg("This script hasn't been tested on version """ & OSVer & """ of Windows.")
		Call PrintMsg("")
		Call PrintMsg("Currently, the script has been tested on the following:")
		Call PrintMsg("            Win2000, WinXP, Win2003")
		Call PrintMsg("")
		Call PrintMsg("Previous versions of Windows NT can use:")
		Call PrintMsg("""XCACLS.EXE"" from the NT 4.0 Resource Kit.")
		Call PrintMsg("")
		Call PrintMsg("For more recent versions, there may be an update to this script.")
		Call PrintMsg("Please contact David Burrell (dburrell@microsoft.com)")
		Call PrintMsg("")
		Call PrintMsg("Note: WMI must be installed for this script to function.")
		Call PrintMsg("If you need to run this script on the current OS,")
		Call PrintMsg("and you verified WMI is installed, do the following:")
		Call PrintMsg("            open this script in Notepad")
		Call PrintMsg("            search for Function IsOSSupported()")
		Call PrintMsg("            change this line:")
		Call PrintMsg("                        Case ""5.0"", ""5.1"", ""5.2""")
		Call PrintMsg("            to:")
		Call PrintMsg("                        Case ""5.0"", ""5.1"", ""5.2"", """ & OSVer & """")
		Call PrintMsg("            Save the script.")
		Call PrintMsg("")
		Call PrintMsg("Exiting script now.")
	End Select

	Exit Do
    Loop
    Set objShell = Nothing

    If debug_on then 
	Call PrintMsg("IsOSSupported: Return = " & IsOSSupported)
    End if

    Call blnErrorOccurred(" occurred while in the IsOSSupported routine. (Msg#3502)")
    If debug_on then Call PrintMsg("IsOSSupported: Exit")

End Function


'********************************************************************
'*
'* Function DoesPathNameExist()
'* Purpose: Checks a FilePath for existance and what it is (file/folder)
'* Input:   File path string
'* Output:  Integer (0 for doesn't exist, 1 for Folder, 2 for File)
'* Notes:   None
'*
'********************************************************************

Function DoesPathNameExist(byVal strFilePath)

    On Error Resume Next

    Dim objFileSystemSet, objPath, strQuery

    If debug_on then Call PrintMsg("DoesPathNameExist: Enter")

    Do
	DoesPathNameExist = 0
	If strFilePath = "" then Exit Do

	If RemoteServer_Used then
		strQuery = "Select Name, FileType from Cim_LogicalFile Where Name=""" & Replace(strFilePath,"\","\\") & """"
        	Set objFileSystemSet = objService.ExecQuery(strQuery,,0)
		If blnErrorOccurred(" occurred setting objFileSystemSet = objService.ExecQuery(" & strQuery & ",,0). (Msg#3601)") Then Exit Do

	    	for each objPath in objFileSystemSet
			If objPath.Name <> "" then
				Select Case UCase(objPath.FileType)
				Case "FILE FOLDER"
					DoesPathNameExist = 1
				Case Else
					DoesPathNameExist = 2
				End select
				Exit For
			End if
	    	next
	Else
		If fso.FolderExists(strFilePath) Then
			DoesPathNameExist = 1
		Else
			If fso.FileExists(strFilePath) Then
				DoesPathNameExist = 2
			End if
		End If
	End if
	Exit Do		'We really didn't want to loop
    Loop
    'ClearObjects that could be set and aren't needed now
    Set objPath = Nothing
    Set objFileSystemSet = Nothing

    If debug_on then 
	Call PrintMsg("DoesPathNameExist: Return = " & DoesPathNameExist)
    End if

    Call blnErrorOccurred(" occurred while in the DoesPathNameExist routine. (Msg#3602)")
    If debug_on then Call PrintMsg("DoesPathNameExist: Exit")

End Function


'********************************************************************
'*
'* Sub PrintArguments()
'* Purpose: To show what arguments were entered
'* Input:   None
'* Output:  Prints arguments
'*
'********************************************************************

Sub PrintArguments()

    ON ERROR RESUME NEXT
    Dim i, TempString

    If debug_on then Call PrintMsg("PrintArguments: Enter")

    'Lets show what arguments were entered:
    Call PrintMsg("")
    Call PrintMsg("Startup directory:")
    Call PrintMsg("""" & fso.GetParentFolderName(fso.GetAbsolutePathName("Test.txt")) & """")
    Call PrintMsg("")
    Call PrintMsg("Arguments Used:")
    If filename_var <> "" Then
	    Call PrintMsg(vbtab & "Filename = """ & DisplayPathString(filename_var) & """")
    Else
	    Call PrintMsg(vbtab & "Filename is required and was not passed as an argument.")
    End if
    If f_Used then Call PrintMsg(vbtab & "/F (All Files under current directory)")
    If s_Used then Call PrintMsg(vbtab & "/S (All Sub Directories under current directory)")
    If t_Used then Call PrintMsg(vbtab & "/T (Traverse Directories)")
    If e_Used then Call PrintMsg(vbtab & "/E (Edit ACL leaving other users intact)")
    If g_Used then 
	Call PrintMsg(vbtab & "/G (Grant rights)")
	For i = LBound(g_var_User) to UBound(g_var_User)
		If g_var_User(i) <> "" then
			TempString = ""
			If g_Var_Spec(i) <> "" then 
				If g_Var_Spec(i) <> "*GUI*" then TempString = ";" & g_Var_Spec(i)
			End if
			Call PrintMsg(vbtab & vbtab & g_var_User(i) & ":" & g_Var_Perm(i) & TempString)
		End if
	Next
    End if
    If r_Used then 
	Call PrintMsg(vbtab & "/R (Revoke rights)")
	For i = LBound(r_var_User) to UBound(r_var_User)
		If r_var_User(i) <> "" then
			Call PrintMsg(vbtab & vbtab & r_var_User(i))
		End if
	Next
    End if
    If p_Used then 
	Call PrintMsg(vbtab & "/P (Replace rights)")
	For i = LBound(p_var_User) to UBound(p_var_User)
		If p_var_User(i) <> "" then
			TempString = ""
			If p_Var_Spec(i) <> "" then 
				If p_Var_Spec(i) <> "*GUI*" then TempString = ";" & p_Var_Spec(i)
			End if			
			Call PrintMsg(vbtab & vbtab & p_var_User(i) & ":" & p_Var_Perm(i) & TempString)
		End if
	Next
    End if
    If d_Used then 
	Call PrintMsg(vbtab & "/D (Deny rights)")
	For i = LBound(d_var_User) to UBound(d_var_User)
		If d_var_User(i) <> "" then
			TempString = ""
			If d_Var_Spec(i) <> "" then 
				If d_Var_Spec(i) <> "*GUI*" then TempString = ";" & d_Var_Spec(i)
			End if			
			Call PrintMsg(vbtab & vbtab & d_var_User(i) & ":" & d_Var_Perm(i) & TempString)
		End if
	Next
    End if
    If o_Used then 
	Call PrintMsg(vbtab & "/O (Change Ownership)")
	Call PrintMsg(vbtab & vbtab & o_var)
    End if
    If i_Used then 
	Call PrintMsg(vbtab & "/I (Inheritance)")
	Select Case i_Var
	Case 1
		Call PrintMsg(vbtab & vbtab & "ENABLE")
	Case 2
		Call PrintMsg(vbtab & vbtab & "COPY")
	Case 3
		Call PrintMsg(vbtab & vbtab & "REMOVE")
	Case Else
		Call PrintMsg(vbtab & vbtab & "N/A")
	End Select
    End if
    If spec_used then
    	Call PrintMsg(vbtab & "/SPEC (Special Folder Permission)")
	Select Case spec_Var
	Case 0		
		Call PrintMsg(vbtab & vbtab & "A - This Folder Only")
	Case OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE
		Call PrintMsg(vbtab & vbtab & "B - This Folder, Subfolders and Files")
	Case CONTAINER_INHERIT_ACE
		Call PrintMsg(vbtab & vbtab & "C - This Folder and Subfolders")
	Case OBJECT_INHERIT_ACE
		Call PrintMsg(vbtab & vbtab & "D - This Folder and Files")
	Case OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE
		Call PrintMsg(vbtab & vbtab & "E - Subfolders and Files only")
	Case CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE					
		Call PrintMsg(vbtab & vbtab & "F - Subfolders only")
	Case OBJECT_INHERIT_ACE + INHERIT_ONLY_ACE
		Call PrintMsg(vbtab & vbtab & "G - Files Only")
	Case Else
		Call PrintMsg(vbtab & vbtab & "N/A")
	End Select
    End if
    If l_Used then 
	Call PrintMsg(vbtab & "/L (File: """ &  strOutputFile & """)")
    End if
    If q_Used then 
	Call PrintMsg(vbtab & "/Q (Quiet mode)")
    End if
    If debug_Used then 
	Call PrintMsg(vbtab & "/DEBUG")
    End if
    If timewmi_Used then 
	Call PrintMsg(vbtab & "/TIMEWMI")
    End if    
    If RemoteServer_Used then 
	Call PrintMsg(vbtab & "/SERVER (For Remote Connections)")
	Call PrintMsg(vbtab & vbtab & strRemoteServerName)
    End if
    If RemoteUserName_Used then 
	Call PrintMsg(vbtab & "/USER")
	Call PrintMsg(vbtab & vbtab & strRemoteUserName)
	Call PrintMsg(vbtab & "/PASS")
	Call PrintMsg(vbtab & vbtab & "******** (Not displaying for security reasons)")
    End if

    Call PrintMsg("")

    Call blnErrorOccurred(" occurred while in the PrintArguments routine. (Msg#3701)")
    If debug_on then Call PrintMsg("PrintArguments: Exit")
End Sub

'********************************************************************
'*
'* Function intParseCmdLine()
'* Purpose: Parses the command line.
'* Input:   Nothing
'* Output:  Messages are sent to the screen and intParseCmdLine returns Success or Failure
'*
'********************************************************************

Function intParseCmdLine()

    ON ERROR RESUME NEXT

    Dim strFlag, i, intState, ValidParGiven, strCurrentArgument, strTempArgument
    Dim TempString, ParsingErrorText, x, j

    Do

	ParsingErrorText = ""
	intParseCmdLine = CONST_SHOW_USAGE
	ValidParGiven = FALSE

	If Wscript.arguments.count = 0 then                'No arguments have been received
        	Exit Do
	End If

	i = 0
	strCurrentArgument = GetThisArg(i)
	While strCurrentArgument <> ""
		TempString = ""
		Select Case UCASE(strCurrentArgument)
		Case "/A", "-A" 'Changes ACLs of files and sub directories in the current directory only.
			ValidParGiven = FALSE
			ParsingErrorText = "Error: /A is no longer used." & vbcrlf & "Please refer to new /F and /S switches."
			intParseCmdLine = ParsingErrorText
			Exit Do

		Case "/F", "-F" 'Changes ACLs of files in the current directory only.
			ValidParGiven = TRUE
			f_Used = TRUE

		Case "/S", "-S" 'Changes ACLs of files in the current directory only.
			ValidParGiven = TRUE
			s_Used = TRUE

		Case "/T", "-T" 'Changes ACLs of specified files in the current directory and all subdirectories.
			ValidParGiven = TRUE
			t_Used = TRUE

		Case "/E", "-E" 'Edit ACL instead of replacing it.
			ValidParGiven = TRUE
			e_Used = TRUE

		Case "/G", "-G" 'user:perm;spec  Grant specified user access rights.
			If i < (Wscript.arguments.count - 1) then
				If GetPermArg(i, g_var_User, g_Var_Perm, g_Var_Spec, g_Var_PermBit, g_Var_SpecBit, TempString, "/G", TRUE) then
					ValidParGiven = TRUE
					g_Used = TRUE
				Else
					ValidParGiven = FALSE
					ParsingErrorText = TempString
					Exit Do
				End if
			End if

		Case "/R", "-R" 'Revoke specified user's access rights.
			If i < (Wscript.arguments.count - 1) then
				If GetPermArg(i, r_var_User, r_var_User, r_var_User, r_var_User, r_var_User, TempString, "/R", FALSE) then
					ValidParGiven = TRUE
					r_Used = TRUE
				Else
					ValidParGiven = FALSE
					ParsingErrorText = TempString
					Exit Do
				End if
			End if

		Case "/P", "-P" 'Replace specified user's access rights. For access right specification see /G option
			If i < (Wscript.arguments.count - 1) then
				If GetPermArg(i, p_var_User, p_Var_Perm, p_Var_Spec, p_Var_PermBit, p_Var_SpecBit, TempString, "/P", TRUE) then
					ValidParGiven = TRUE
					p_Used = TRUE
				Else
					ValidParGiven = FALSE
					ParsingErrorText = TempString
					Exit Do
				End if
			End if

		Case "/D", "-D" 'Deny specified user access.
			If i < (Wscript.arguments.count - 1) then
				If GetPermArg(i, d_var_User, d_Var_Perm, d_Var_Spec, d_Var_PermBit, d_Var_SpecBit, TempString, "/D", TRUE) then
					ValidParGiven = TRUE
					d_Used = TRUE
				Else
					ValidParGiven = FALSE
					ParsingErrorText = TempString
					Exit Do
				End if
			End if

		Case "/O", "-O" 'Change the Owner.
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					o_Used = TRUE
					ValidParGiven = TRUE
					o_var = strTempArgument
					i = x
				End if
			End if

		Case "/I", "-I" 'Inheritance Flag
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					Select Case UCASE(strTempArgument)
					Case "ENABLE"
						j = 1
					Case "COPY"
						j = 2
					Case "REMOVE"
						j = 3
					Case Else
						j = 0
					End Select
					If j > 0 then
						i_var = j
						i = x
						ValidParGiven = TRUE
						i_Used = TRUE
					End if
				End if
			End if

		Case "/SPEC", "-SPEC" 'Special Folder permissions
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					Select Case ucase(strTempArgument)
					Case "A"		' "This Folder Only"
						spec_var = 0
					Case "B"		' "This Folder, Subfolders and Files"
						spec_var = OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE			
					Case "C"		' "This Folder and Subfolders"
						spec_var = CONTAINER_INHERIT_ACE						
					Case "D"		' "This Folder and Files"
						spec_var = OBJECT_INHERIT_ACE
					Case "E"		' "Subfolders and Files only"
						spec_var = OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE
					Case "F"		' "Subfolders only"
						spec_var = CONTAINER_INHERIT_ACE + INHERIT_ONLY_ACE						
					Case "G"		' "Files Only"
						spec_var = OBJECT_INHERIT_ACE + INHERIT_ONLY_ACE						
					Case Else
						ValidParGiven = FALSE
						ParsingErrorText = "/SPEC switch """ & strTempArgument & """ is invalid" & vbcrlf
						Exit Do
					End Select	
					spec_Used = TRUE
					ValidParGiven = TRUE
					i = x
				End if
			End if			

		Case "/H","HELP","\H","-H","H","?","-?","/?","\?"
			Exit Function

		Case "/L", "-L"
			'If the filename is not present, then the user simply wants to turn on logging.
			l_used = TRUE
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					strOutputFile = strTempArgument
					i = x
				End if
			End if

		Case "/Q", "-Q"
			blnQuiet = TRUE
			q_used = TRUE

		Case "/DEBUG"
			debug_on = TRUE
			debug_used = TRUE

		Case "/TIMEWMI"
			timewmi_on = TRUE
			timewmi_used = TRUE			

		Case "/SERVER", "-SERVER" 'Remote Server.
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					RemoteServer_Used = TRUE
					ValidParGiven = TRUE
					strRemoteServerName = strTempArgument
					i = x
				End if
			End if

		Case "/USER", "-USER" 'UserName.
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					If strRemotePassword <> "" then RemoteUserName_Used = TRUE
					ValidParGiven = TRUE
					strRemoteUserName = strTempArgument
					i = x
				End if
			End if

		Case "/PASS", "-PASS" 'Password.
			If i < (Wscript.arguments.count - 1) then
				x = i + 1 'The very next parameter should be for this switch
				strTempArgument = GetThisArg(x)
				If Left(strTempArgument,1) <> "/" AND Left(strTempArgument,1) <> "-" then
					If strRemoteUserName <> "" then RemoteUserName_Used = TRUE
					ValidParGiven = TRUE
					strRemotePassword = strTempArgument
					i = x
				End if
			End if

		Case else
			If i = 0 then
				ValidParGiven = TRUE
				filename_var = strCurrentArgument
			Else
				ParsingErrorText = "Error: Invalid flag " & strCurrentArgument & "." & vbcrlf
				ParsingErrorText = ParsingErrorText & "Please check the input and try again." & vbcrlf
				intParseCmdLine = ParsingErrorText
				Exit Do
			End if
		End Select
		i = i + 1
		strCurrentArgument = GetThisArg(i)
	    Wend

	intParseCmdLine = CONST_PROCEED

	Exit Do
    Loop
    If Not ValidParGiven Then
        intParseCmdLine = ParsingErrorText
    End if
    If filename_var = "" then
	ParsingErrorText = ParsingErrorText & "Error: Required Filename missing." & vbcrlf
	ParsingErrorText = ParsingErrorText & "" & vbcrlf
	ParsingErrorText = ParsingErrorText & "Run XCACLS with /? for usage instructions" & vbcrlf
	intParseCmdLine = ParsingErrorText
    End If

End Function

'********************************************************************
'*
'* Function GetThisArg()
'* Purpose: Gets the next argument, returns TRUE if there were no errors
'* Input:   ArgNumber of next argument
'* Output:  Returns String of next argument or blank if there was none, updates argnumber
'*
'********************************************************************

Function GetThisArg(ByRef intArgNumber)

    On Error Resume Next

    Dim BoolComplete, intLeftCharHex

    Do
	GetThisArg = ""
	If Wscript.arguments.count = 0 then                		'No arguments have been received
        	Exit Do
	End If

	If intArgNumber = (Wscript.arguments.count) then 		'No more to get
        	Exit Do
	End If

	BoolComplete = FALSE

	intLeftCharHex = ASC(Left(Wscript.arguments.Item(intArgNumber),1))
	GetThisArg = Wscript.arguments.Item(intArgNumber)
	Select Case intLeftCharHex
	Case 34, 145, 146, 147, 148	'Quotation marks (different kinds)
		If InStr(2, Wscript.arguments.Item(intArgNumber), Chr(intLeftCharHex),1) > 0 then
			'Then we know that the quotes is closed in the same argument.
		Else
			If intArgNumber < Wscript.arguments.count - 1 then
				While BoolComplete = FALSE
					intArgNumber = intArgNumber + 1
					GetThisArg = GetThisArg & " " & Wscript.arguments.Item(intArgNumber)
					If InStr(1, Wscript.arguments.Item(intArgNumber), Chr(intLeftCharHex),1) > 0 then
						'Then we found the quote pair, lets end it.
						BoolComplete = TRUE
					End if
				Wend 
			End if
		End if
	End Select

	Exit Do
	
    Loop

    Call blnErrorOccurred(" occurred while in the GetThisArg routine. (Msg#3801)")

End Function


'********************************************************************
'*
'* Function GetPermArg()
'* Purpose: Gets the next Perm type argument, returns TRUE if there were no errors
'* Input:   ArguNumber, UserArray, PermArray, SpecArray
'* Output:  Returns Boolean
'*
'********************************************************************

Function GetPermArg(ByRef intI, ByRef Array_User, ByRef Array_Perm, ByRef Array_Spec, ByRef Array_PermBit, ByRef Array_SpecBit, ByRef strErrorText, byval strSwitch, byval boolNeedsColon)

    On Error Resume Next

    Dim x, colonplace, semicolonplace, CurrentIndex, CurrentArgument, strFirstChar, strFullUser, strPerm, strSpec, Lplace, strWithoutL 
    Dim temp_strPerm, temp_strSpec

    Do
	GetPermArg = FALSE
	colonplace = 0
	semicolonplace = 0
	Lplace = 0
	strWithoutL = ""

	x = intI + 1 'The very next parameter should be for this switch

	CurrentArgument = GetThisArg(x)
	While CurrentArgument <> ""
		strFirstChar = Left(CurrentArgument, 1)
		If strFirstChar <> "/" And strFirstChar <> "-" then
			colonplace = InStr(1, CurrentArgument, ":",1)
			semicolonplace = InStr(1, CurrentArgument, ";",1)
			If boolNeedsColon then
				If colonplace > 0 then
					strFullUser = Left(CurrentArgument, colonplace - 1)
					If semicolonplace > 0 then
						strPerm = UCASE(Mid(CurrentArgument, colonplace + 1, semicolonplace - colonplace - 1))
						strSpec = UCASE(Mid(CurrentArgument, semicolonplace + 1))
					Else
						strPerm = UCASE(Mid(CurrentArgument, colonplace + 1))
						strSpec = ""
					End if
					If strPerm <> "" then
						temp_strPerm = ConvertToOurCodes(strPerm)
						If Not IsPermCompatible(temp_strPerm) then 
							strErrorText = strErrorText & "Error: Perm entered with " & strSwitch & " not valid, ending script." & vbcrlf
							strPerm = ""
						End if
					End if
					If strSpec <> "" then
						temp_strSpec = ConvertToOurCodes(strSpec)
						If Not IsPermCompatible(temp_strSpec) then 
							strErrorText = strErrorText & "Error: Spec entered with " & strSwitch & " not valid, ending script." & vbcrlf
							strSpec = ""
						End if
					End if
					If strPerm <> "" and strSpec = "" then
						If IsPermGUI(strPerm) then
							strSpec = "*GUI*"
						End if
					End if

					If strPerm = "" and strSpec = "" then 
						strErrorText = strErrorText & "Error: Valid Perm or Spec required when using " & strSwitch & ", ending script." & vbcrlf
						Exit Do
					Else
						CurrentIndex = AddStringToArray(Array_User, strFullUser, -1)
						Call AddStringToArray(Array_Perm, strPerm, CurrentIndex)
						Call AddStringToArray(Array_Spec, strSpec, CurrentIndex)
						Call AddStringToArray(Array_PermBit, SECBitMask(temp_strPerm), CurrentIndex)
						Call AddStringToArray(Array_SpecBit, SECBitMask(temp_strSpec), CurrentIndex)
						GetPermArg = TRUE
					End if
				End if
			Else
				If colonplace > 0 then
					strErrorText = strErrorText & "Error: User argument should not have a colon in it when using " & strSwitch & ", ending script." & vbcrlf
				end if
				If semicolonplace > 0 then
					strErrorText = strErrorText & "Error: User argument should not have a semicolon in it when using " & strSwitch & ", ending script." & vbcrlf
				end if
				If colonplace = 0 and semicolonplace = 0 then
					Call AddStringToArray(Array_User, CurrentArgument, -1)
					GetPermArg = TRUE
				End if
			End if
			intI = x
			x = x + 1
			CurrentArgument = GetThisArg(x)
		Else 
			strErrorText = strErrorText & "Error: Username entered with " & strSwitch & " not valid, ending script." & vbcrlf
			CurrentArgument = ""
		End if
	Wend
	
	Exit Do
	
    Loop

    Call blnErrorOccurred(" occurred while in the GetPermArg routine. (Msg#3901)")

End Function


'********************************************************************
'*
'* Sub ShowUsage()
'* Purpose: Shows the correct usage to the user.
'* Input:   None
'* Output:  Help messages are displayed on screen.
'*
'********************************************************************

Sub ShowUsage()

	If debug_on then Call PrintMsg("ShowUsage: Enter")

	Dim X

	Call PrintMsg("")
	Call PrintMsg("------------------------------------------------------------------")
	Call PrintMsg("---------------------------- Usage -------------------------------")
	Call PrintMsg("------------------------------------------------------------------")
	Call PrintMsg("Displays or modifies access control lists (ACLs) of files & directories")
	Call PrintMsg("")
	Call PrintMsg("XCACLS filename [/E] [/G user:perm;spec] [...] [/R user [...]]")
	Call PrintMsg("                [/F] [/S] [/T]")
	Call PrintMsg("                [/P user:perm;spec [...]] [/D user:perm;spec] [...]")
	Call PrintMsg("                [/O user] [/I ENABLE/COPY/REMOVE] [/N")
	Call PrintMsg("                [/L filename] [/Q] [/DEBUG]")
	Call PrintMsg("")
	Call PrintMsg("   filename            [Required] If used alone, it Displays ACLs.")
	Call PrintMsg("                       (Filename can be a filename, directory name or")
	Call PrintMsg("                       wildcard characters and can include the entire")
	Call PrintMsg("                       path. If path is missing, its assumed to be")
	Call PrintMsg("                       under the current directory.")
	Call PrintMsg("                       Notes:")
	Call PrintMsg("                       - Put filename in quotes if it has spaces or")
	Call PrintMsg("                       special characters such as &, $, #, etc.")
	Call PrintMsg("                       - If Filename is a directory, all files and")
	Call PrintMsg("                       sub directories under it will NOT be changed")
	Call PrintMsg("                       unless the /F or /S is present.")
	Call PrintMsg("")
	Call PrintMsg("   /F                  [Used with Directory or Wildcard] This will change all")
	Call PrintMsg("                       files under the inputed directory but will NOT")
	Call PrintMsg("                       traverse sub directories unless /T is also present.")
	Call PrintMsg("                       If filename is a directory, and /F is not used, no")
	Call PrintMsg("                       files will be touched.")
	Call PrintMsg("")
	Call PrintMsg("   /S                  [Used with Directory or Wildcard] This will change all")
	Call PrintMsg("                       sub folders under the inputed directory but will NOT")
	Call PrintMsg("                       traverse sub directories unless /T is also present.")
	Call PrintMsg("                       If filename is a directory, and /S is not used, no")
	Call PrintMsg("                       sub directories will be touched.")
	Call PrintMsg("")
	Call PrintMsg("   /T                  [Used only with a Directory] Traverses each")
	Call PrintMsg("                       subdirectory and makes the same changes.")
	Call PrintMsg("                       This switch will traverse directories only if the")
	Call PrintMsg("                       filename is a directory or is using wildcards.")
	Call PrintMsg("   /E                  Edit ACL instead of replacing it.")
	Call PrintMsg("")
	Call PrintMsg("   /G user:GUI         Grant security permissions similar to Windows GUI")
	Call PrintMsg("                       standard (non-advanced) choices.")
	Call PrintMsg("   /G user:Perm;Spec   Grant specified user access rights.")
	Call PrintMsg("                       (/G adds to existing rights for user)")
	Call PrintMsg("") 
	Call PrintMsg("                       User: If User has spaces in it, surround it in Quotes")
	Call PrintMsg("                             If User contains #machine#, it will replace")
	Call PrintMsg("                             #machine# with the actual machine name if its a")
	Call PrintMsg("                             non-domain controller, and replace it with the")
	Call PrintMsg("                             actual domain name if it is a domain controller.")
	Call PrintMsg("")
	Call PrintMsg("                             New to 3.0: User can be a string representing")
	Call PrintMsg("                             the actual SID, but MUST be lead by SID#")
	Call PrintMsg("                             Example: SID#S-1-5-21-2127521184-160...")
	Call PrintMsg("                                      (SID string shown has been shortened)")
	Call PrintMsg("                                      (If any user has SID# then globaly all") 
	Call PrintMsg("                                       matches must match the SID (not name)") 
	Call PrintMsg("                                       so if your intention is to apply changes") 
	Call PrintMsg("                                       to all accounts that match Domain\User") 
	Call PrintMsg("                                       then do not specify SID# as one of the") 
	Call PrintMsg("                                       users)") 
	Call PrintMsg("") 
	Call PrintMsg("                       GUI: Is for standard rights and can be:")
	Call PrintMsg("                             Permissions...")
	Call PrintMsg("                                    F  Full control")
	Call PrintMsg("                                    M  Modify")
	Call PrintMsg("                                    X  read & eXecute")
	Call PrintMsg("                                    L  List folder contents")
	Call PrintMsg("                                    R  Read")
	Call PrintMsg("                                    W  Write")
	Call PrintMsg("                             Note: If a ; is present, this will be considered") 
	Call PrintMsg("                             a Perm;Spec parameter pair") 
	Call PrintMsg("") 
	Call PrintMsg("                       Perm: Is for ""Files Only"" and can be:")
	Call PrintMsg("                             Permissions...")
	Call PrintMsg("                                    F  Full control")
	Call PrintMsg("                                    M  Modify")
	Call PrintMsg("                                    X  read & eXecute")
	Call PrintMsg("                                    R  Read")
	Call PrintMsg("                                    W  Write")
	Call PrintMsg("                             Advanced...")
	
		For X = LBound(Perms_LStr) to UBound(Perms_LStr)
	    		If Perms_SStr(X) <> "" then
		    		If debug_on then 
					Call PrintMsg("                                    " & Perms_SStr(X) & " " & Perms_LStr(X) & " (bitmask=" & Perms_Const(X))
		    		Else
			    		Call PrintMsg("                                    " & Perms_SStr(X) & " " & Perms_LStr(X) )
			    	End if
	    		End if
		Next
		
	Call PrintMsg("                       Spec is for ""Folder and Subfolders only"" and has the")
	Call PrintMsg("                       same choices as Perm.")
	Call PrintMsg("")
	Call PrintMsg("   /R user             Revoke specified user's access rights.")
	Call PrintMsg("                       (Will remove any Allowed or Denied ACL's for user)")
	Call PrintMsg("")
	Call PrintMsg("   /P user:GUI         Replace security permissions similiar to standard choices")
	Call PrintMsg("   /P user:perm;spec   Replace specified user's access rights.")
	Call PrintMsg("                       For access right specification see /G option")
	Call PrintMsg("                       (/P acts like /G if there are no rights set for user)")
	Call PrintMsg("")
	Call PrintMsg("   /D user:GUI         Deny security permissions similiar to standard choices.")
	Call PrintMsg("   /D user:perm;spec   Deny specified user access rights.")
	Call PrintMsg("                       For access right specification see /G option")
	Call PrintMsg("                       (/D adds to existing rights for user)")
	Call PrintMsg("")
	Call PrintMsg("   /O user             Change the Ownership to this user or group.")
	Call PrintMsg("")
	Call PrintMsg("   /I switch           Inheritance flag, if omitted default is to not touch")
	Call PrintMsg("                       Inherited ACL's. Switch can be:")
	Call PrintMsg("                          ENABLE - This will turn on the Inheritance Flag if")
	Call PrintMsg("                                   its not on already.")
	Call PrintMsg("                          COPY   - This will turn off the Inheritance flag and")
	Call PrintMsg("                                   copy the Inherited ACL's")
	Call PrintMsg("                                   into Effecive ACL's")
	Call PrintMsg("                          REMOVE - This will turn off the Inheritance flag and")
	Call PrintMsg("                                   will not copy the Inherited")
	Call PrintMsg("                                   ACL's, this is the opposite of ENABLE")
	Call PrintMsg("                          If switch is not present, /I will be ignored and")
	Call PrintMsg("                          Inherited ACL's will remain untouched.")
	Call PrintMsg("")
	Call PrintMsg("   /SPEC switch        Special Permission for Folder and Subfolders only")
	Call PrintMsg("                       If this switch is used, and the object is a folder, then")
	Call PrintMsg("                       one of the switches below would be used instead of the")
	Call PrintMsg("                       default.")
	Call PrintMsg("                          A - This Folder Only")
	Call PrintMsg("                          B - This Folder, Subfolders and Files (Default)")
	Call PrintMsg("                          C - This Folder and Subfolders")
	Call PrintMsg("                          D - This Folder and Files")
	Call PrintMsg("                          E - Subfolders and Files Only")
	Call PrintMsg("                          F - Subfolders Only")
	Call PrintMsg("                          G - Files Only")
	Call PrintMsg("")
	Call PrintMsg("   /L filename         Filename for Logging. This can include a path name")
	Call PrintMsg("                       if the file isn't under the current directory.")
	Call PrintMsg("                       File will be appended to, or created if it doesn't")
	Call PrintMsg("                       exit. Must be Text file if it exists or error will occur.")
	Call PrintMsg("                       If filename is obmitted the default name of XCACLS will")
	Call PrintMsg("                       be used.")
	Call PrintMsg("")
	Call PrintMsg("   /Q                  Turn on Quiet mode, its off by default.")
	Call PrintMsg("                       If its turned on, there will be no display to the screen.")
	Call PrintMsg("")
	Call PrintMsg("   /DEBUG              Turn on Debug mode, its off by default.")
	Call PrintMsg("                       If its turned on, there will be more information")
	Call PrintMsg("                       displayed and/or logged. Information will show")
	Call PrintMsg("                       Sub/Function Enterand Exit as well as other important")
	Call PrintMsg("                       information.")
	Call PrintMsg("")
	Call PrintMsg("   /TIMEWMI            Turn on to Time WMI use, only shows up in Debug Mode.")	
	Call PrintMsg("")
	Call PrintMsg("   /SERVER servername  Enter a remote server to run script against.")
	Call PrintMsg("")
	Call PrintMsg("   /USER username      Enter Username to impersonate for Remote Connections")
	Call PrintMsg("                            (Requires PASS switch)")
	Call PrintMsg("                            - Will be ignored if its for a Local Connection.")
	Call PrintMsg("")
	Call PrintMsg("   /PASS password      Enter Password to go with USER switch")
	Call PrintMsg("                            (Requires USER switch)")
	Call PrintMsg("")
	Call PrintMsg("")
	Call PrintMsg("Wildcards can be used to specify more than one file in a command.")
	Call PrintMsg("Such as:")
	Call PrintMsg("				*  	Any string of zero or more characters")
	Call PrintMsg("				?  	Any single character")
	Call PrintMsg("")
	Call PrintMsg("You can specify more than one user in a command.")
	Call PrintMsg("You can combine access rights.")
	Call PrintMsg("")

	Call blnErrorOccurred(" occurred while in the ShowUsage routine. (4001)")

	If debug_on then Call PrintMsg("ShowUsage: Exit")

End Sub

'********************************************************************
'*
'* Function IsEngineCScript()
'* Purpose: Determines which program is used to run this script.
'* Input:   None
'* Output:  True if using CScript.
'*
'********************************************************************

Function IsEngineCScript()

    ON ERROR RESUME NEXT

    Dim strFullName, strCommand, i, j

    IsEngineCScript = False
    'strFullName should be something like C:\WINDOWS\COMMAND\CSCRIPT.EXE
    strFullName = WScript.FullName
    If Err.Number Then
	Wscript.Echo "Error while checking scripting engine." & vbcrlf & "Error " & Err.Number & ": " & Err.Description
	WScript.Quit
    End if

    i = InStr(1, strFullName, ".exe", 1)
    If i = 0 Then
        Exit Function
    Else
        j = InStrRev(strFullName, "\", i, 1)
        If j = 0 Then
            Exit Function
        Else
            strCommand = Mid(strFullName, j+1, i-j-1)
            Select Case LCase(strCommand)
                Case "cscript"
			IsEngineCScript = True
                Case "wscript"
			'IsEngineCScript is already False so we don't do anything
                Case Else       'should never happen
			wscript.Echo "An unexpected program is used to run this script." & vbcrlf & "Only CScript.Exe or WScript.Exe can be used to run this script."
			WScript.Quit
            End Select
        End If
    End If

    If Not IsEngineCScript Then
	Wscript.Echo "You are not using CScript for the scripting engine." & vbcrlf & "Screen messages will be surpressed."
    End if

End Function

'********************************************************************
'*
'* Function strPackString()
'* Purpose: Attaches spaces to a string to increase the length to intWidth.
'* Input:   strString   a string
'*          intWidth   the intended length of the string
'*          blnAfter    specifies whether to add spaces after or before the string
'*          blnTruncate specifies whether to truncate the string or not if
'*                      the string length is longer than intWidth
'* Output:  strPackString is returned as the packed string.
'*
'********************************************************************

Function strPackString(byval strString, ByVal intWidth, byval blnAfter, byval blnTruncate)

    ON ERROR RESUME NEXT

    If debug_on then Call PrintMsg("strPackString: Enter")

    Do

	intWidth = CInt(intWidth)
	blnAfter = CBool(blnAfter)
	blnTruncate = CBool(blnTruncate)
	If blnErrorOccurred(" Argument type is incorrect for strPackString function. (Msg#4101)") Then 
		Exit Do 
	End if

	If IsNull(strString) Then
        	strPackString = "null" & Space(intWidth-4)
        	Exit Do
	End If

	strString = CStr(strString)
	If blnErrorOccurred(" Argument type is incorrect for strPackString function. (Msg#4102)") Then 
		Exit Do 
	End if

	If intWidth > Len(strString) Then
        	If blnAfter Then
			strPackString = strString & Space(intWidth-Len(strString))
        	Else
			strPackString = Space(intWidth-Len(strString)) & strString & " "
        	End If
	Else
		If blnTruncate Then
			strPackString = Left(strString, intWidth-1) & " "
        	Else
			strPackString = strString & " "
		End If
	End If
	Exit Do
    Loop

    If debug_on then Call PrintMsg("strPackString: Return = " & strPackString)

    Call blnErrorOccurred(" occurred while in the strPackString routine. (Msg#4103)")
    If debug_on then Call PrintMsg("strPackString: Exit")

End Function


'********************************************************************
'*
'* Sub OpenOutputFile()
'* Purpose: Opens the output file, or sets the object to "" if its not needed
'* Input:   Nothing
'* Output:  Nothing
'*
'********************************************************************

Sub OpenOutputFile()

    Dim objFileSystem, MyFile, strAbsoluteFile

    If Not blnQuiet then
	If debug_on then Wscript.Echo "OpenOutputFile: Enter"
    End if

    Do
	If strOutputFile = "" then Exit Do
	set objFileSystem = CreateObject("Scripting.FileSystemObject")
	If blnErrorOccurred(" opening a filesystem object. (Msg#4201)") Then 
		strOutputFile = ""
		Exit Do
	End if
	'Open the file for output
	strAbsoluteFile = objFileSystem.GetAbsolutePathName(strOutputFile)
	If Not objFileSystem.FileExists(strAbsoluteFile) Then 
		'If it doesn't exist we try to create it.
		Set MyFile = objFileSystem.CreateTextFile(strAbsoluteFile, TRUE)

		If blnErrorOccurred(" occurred in getting objFileSystem.CreateTextFile(strAbsoluteFile, TRUE) (Msg#4202)") Then Exit Do

		MyFile.Close
	End If
	set objOutputFile = objFileSystem.OpenTextFile(strAbsoluteFile, 8, TRUE)
	If blnErrorOccurred(" opening file " & strAbsoluteFile & ". (Msg#4203)") Then 
		strOutputFile = ""
		Exit Do
	End if
	Exit Do
    Loop

    Call blnErrorOccurred(" occurred while in the OpenOutputFile routine. (Msg#4204)")
    If Not blnQuiet then
	If debug_on then Wscript.Echo "OpenOutputFile: Exit"
    End if

End Sub


'********************************************************************
'*
'* Function blnErrorOccurred()
'* Purpose: Reports error with a string saying what the error occurred in.
'* Input:   strIn		string saying what the error occurred in.
'* Output:  displayed on screen 
'*
'********************************************************************
Function blnErrorOccurred (byval strIn)
    Dim TempNum, TempDescript

    If Err.Number Then
        TempNum = Err.Number
        TempDescript = Err.Description
        Err.Clear
        Call PrintMsg( "Error " & TempNum & ": " & strIn)
        If TempDescript <> "" Then
            Call PrintMsg( "Error description: " & TempDescript)
        End If
        blnErrorOccurred = TRUE
    Else
        blnErrorOccurred = FALSE
    End If

End Function

'********************************************************************
'*
'* Sub PrintMsg()
'* Purpose: Prints a message on screen if blnQuiet = FALSE.
'* Input:   strMessage      the string to print
'* Output:  strMessage is printed on screen if blnQuiet = FALSE.
'*
'********************************************************************

Sub PrintMsg(byval strMessage)
    If Not blnQuiet then
	If BoolUsingCScript then Wscript.Echo  strMessage
    End If

    If l_Used then
	If strOutputFile <> "" Then
		If IsObject(objOutputFile) then
	        	objOutputFile.WriteLine strMessage
	        	If Err.Number Then
		             Wscript.Echo "Error " & Err.Number & ": Writing to Logfile" & vbcrlf & "Error description: " & Err.Description
        		     Err.Clear
	        	End If
		Else
			Wscript.Echo "Error: Logfile object missing"
		End if
	    End if
    End if
End Sub


'********************************************************************
'*                                                                  *
'*                           End of File                            *
'*                                                                  *
'********************************************************************
