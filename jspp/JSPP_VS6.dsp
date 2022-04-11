# Microsoft Developer Studio Project File - Name="JSPP_VS6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=JSPP_VS6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "JSPP_VS6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "JSPP_VS6.mak" CFG="JSPP_VS6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "JSPP_VS6 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "JSPP_VS6 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "JSPP_VS6 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\sources\spl" /I "\sources\jspp" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "CONSOLE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib spl.lib /nologo /subsystem:console /incremental:yes /machine:I386 /libpath:"\sources\spl\release"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "JSPP_VS6 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\sources\spl" /I "\sources\jspp" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "CONSOLE" /D "DEBUG" /D "_WIN32" /D "MTLIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib spld.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"\sources\spl\debug"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "JSPP_VS6 - Win32 Release"
# Name "JSPP_VS6 - Win32 Debug"
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

SOURCE=.\cli\mainishere.c
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Docs"

# PROP Default_Filter "*.doc"
# Begin Group "Web"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Docs\web\index.html
# End Source File
# Begin Source File

SOURCE=.\Docs\web\JohnGarrison2005.html
# End Source File
# Begin Source File

SOURCE=.\Docs\web\Reference.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\Docs\_todo.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\bnf.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\classoverview.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\compilernotes.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\files.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\interpnotes.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\JsppDiagrams.vsd
# End Source File
# Begin Source File

SOURCE=.\Docs\langnotes.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\README.txt
# End Source File
# Begin Source File

SOURCE=.\Docs\Reference.html
# End Source File
# End Group
# Begin Group "TestJSPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\testsrc\console.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\factorial.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\Factorial2.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\fib.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\httpfetch.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\life.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\NeuralNetwork.jspp
# End Source File
# Begin Source File

SOURCE=.\testsrc\static.jspp
# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test\jtestlib.c
# End Source File
# Begin Source File

SOURCE=.\test\libtestharness.c
# End Source File
# Begin Source File

SOURCE=.\test\testcompile.c
# End Source File
# Begin Source File

SOURCE=.\test\testembedded.c
# End Source File
# Begin Source File

SOURCE=.\test\testinstrinsic.c
# End Source File
# Begin Source File

SOURCE=.\test\testinterp.c
# End Source File
# Begin Source File

SOURCE=.\test\testlex.c
# End Source File
# Begin Source File

SOURCE=.\test\testparse.c
# End Source File
# Begin Source File

SOURCE=.\test\testprogram.c
# End Source File
# Begin Source File

SOURCE=.\test\testvariant.c
# End Source File
# Begin Source File

SOURCE=.\test\unittest.c
# End Source File
# End Group
# End Target
# End Project
