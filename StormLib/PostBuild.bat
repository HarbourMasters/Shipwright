@echo off
rem Post-build batch for StormLib project
rem Called as PostBuild.bat $(ProjectName) $(PlatformName) $(ConfigurationName) [vs2008]
rem Example: PostBuild.bat StormLib_dll x64 Debug vs2008

rem Select build type
if "%1" == "StormLib_dll" goto PostBuild_DLL
if "%1" == "StormLib"     goto PostBuild_LIB
goto:eof

:PostBuild_DLL

rem Build steps for the DLL. On 32-bit Release version, increment the build number
if not "x%2" == "xWin32" goto:eof
if not "x%3" == "xRelease" goto:eof
PostBuild.exe .\src\DllMain.rc
goto:eof

:PostBuild_LIB

rem Set target folders
if "x%2" == "xWin32"  set TARGET_DIR_LEVEL2=lib32
if "x%2" == "xx64"    set TARGET_DIR_LEVEL2=lib64
if "x%4" == "xvs2008" set TARGET_DIR_LEVEL3=vs2008

rem Check & create target folder structure
if not exist ..\aaa goto:eof
if not exist ..\aaa\%TARGET_DIR_LEVEL2% md ..\aaa\%TARGET_DIR_LEVEL2%
if not exist ..\aaa\%TARGET_DIR_LEVEL2%\%TARGET_DIR_LEVEL3% md ..\aaa\%TARGET_DIR_LEVEL2%\%TARGET_DIR_LEVEL3%

rem Copy include and LIB files to the target folder
copy /Y .\src\StormLib.h  ..\aaa\inc >nul
copy /Y .\src\StormPort.h ..\aaa\inc >nul
copy /Y .\bin\%1\%2\%3\StormLib???.lib ..\aaa\%TARGET_DIR_LEVEL2%\%TARGET_DIR_LEVEL3% >nul
