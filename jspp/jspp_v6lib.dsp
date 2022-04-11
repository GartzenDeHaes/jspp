# Microsoft Developer Studio Project File - Name="jspp_v6lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jspp_v6lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jspp_v6lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jspp_v6lib.mak" CFG="jspp_v6lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jspp_v6lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jspp_v6lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jspp_v6lib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "\sources\spl" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\jspp.lib"

!ELSEIF  "$(CFG)" == "jspp_v6lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\sources\spl" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MTLIB" /D "DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\jsppd.lib"

!ENDIF 

# Begin Target

# Name "jspp_v6lib - Win32 Release"
# Name "jspp_v6lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\interp.c
# End Source File
# Begin Source File

SOURCE=.\src\interpinner.c
# End Source File
# Begin Source File

SOURCE=.\src\intrinsics.c
# End Source File
# Begin Source File

SOURCE=.\src\jdes.c
# End Source File
# Begin Source File

SOURCE=.\src\jhttp_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\jsclass.c
# End Source File
# Begin Source File

SOURCE=.\src\jsocket.c
# End Source File
# Begin Source File

SOURCE=.\src\lex.c
# End Source File
# Begin Source File

SOURCE=.\src\libcommon.c
# End Source File
# Begin Source File

SOURCE=.\src\native.c
# End Source File
# Begin Source File

SOURCE=.\src\parse.c
# End Source File
# Begin Source File

SOURCE=.\src\program.c
# End Source File
# Begin Source File

SOURCE=.\src\programdump.c
# End Source File
# Begin Source File

SOURCE=.\src\variant.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\jspp\config.h
# End Source File
# Begin Source File

SOURCE=.\jspp\interp.h
# End Source File
# Begin Source File

SOURCE=.\jspp\interperrors.h
# End Source File
# Begin Source File

SOURCE=.\jspp\intrinsics.h
# End Source File
# Begin Source File

SOURCE=.\jspp\jdes.h
# End Source File
# Begin Source File

SOURCE=.\jspp\jhttp_lib.h
# End Source File
# Begin Source File

SOURCE=.\jspp\jsclass.h
# End Source File
# Begin Source File

SOURCE=.\jspp\jsocket.h
# End Source File
# Begin Source File

SOURCE=.\jspp\lex.h
# End Source File
# Begin Source File

SOURCE=.\jspp\native.h
# End Source File
# Begin Source File

SOURCE=.\jspp\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\jspp\parse.h
# End Source File
# Begin Source File

SOURCE=.\jspp\parseerrors.h
# End Source File
# Begin Source File

SOURCE=.\jspp\program.h
# End Source File
# Begin Source File

SOURCE=.\jspp\variant.h
# End Source File
# Begin Source File

SOURCE=.\jspp\y.tab.h
# End Source File
# End Group
# End Target
# End Project
