:: Build file for Visual Studio 2008 and 2017
@echo off

:: Save the values of INCLUDE, LIB and PATH
set SAVE_INCLUDE=%INCLUDE%
set SAVE_LIB=%LIB%
set SAVE_PATH=%PATH%
set LIB_NAME=StormLib

:: Determine where the program files are, both for 64-bit and 32-bit Windows
if exist "%ProgramFiles%"      set PROGRAM_FILES_DIR=%ProgramFiles%
if exist "%ProgramFiles(x86)%" set PROGRAM_FILES_DIR=%ProgramFiles(x86)%

:: Determine the installed version of Visual Studio (Prioritize Enterprise over Professional)
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"                               set VCVARS_2008=%PROGRAM_FILES_DIR%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"    set VCVARS_2017=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" set VCVARS_2017=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"   set VCVARS_2017=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"    set VCVARS_2019=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" set VCVARS_2019=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat
if exist "%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"   set VCVARS_2019=%PROGRAM_FILES_DIR%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat

::Build all libraries using Visual Studio 2008 and 2017
call :BuildLibs "%VCVARS_2008%" x86 %LIB_NAME%_vs08.sln
call :BuildLibs "%VCVARS_2008%" x64 %LIB_NAME%_vs08.sln
call :BuildLibs "%VCVARS_2019%" x86 %LIB_NAME%_vs19.sln
call :BuildLibs "%VCVARS_2019%" x64 %LIB_NAME%_vs19.sln
goto:eof

::-----------------------------------------------------------------------------
:: Build all 8 configurations of the library
::
:: Parameters:
::
::   %1     Full path to the VCVARS.BAT file
::   %2     Target build platform (x86 or x64)
::   %3     Plain name of the /sln solution file
::

:BuildLibs
::set VSCMD_DEBUG=1
call %1 %2
if "%2" == "x86" set SLN_TRG=Win32
if "%2" == "x64" set SLN_TRG=x64
devenv.com %3 /project "%LIB_NAME%" /rebuild "DebugAD|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "DebugAS|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "DebugUD|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "DebugUS|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "ReleaseAD|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "ReleaseAS|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "ReleaseUD|%SLN_TRG%"
devenv.com %3 /project "%LIB_NAME%" /rebuild "ReleaseUS|%SLN_TRG%"

:: Restore environment variables to the old level
set INCLUDE=%SAVE_INCLUDE%
set LIB=%SAVE_LIB%
set PATH=%SAVE_PATH%
set VSINSTALLDIR=
set VCINSTALLDIR=
set DevEnvDir=
