@echo off
setlocal enabledelayedexpansion

rem Optional: specify build configuration (Debug, Release, RelWithDebInfo, MinSizeRel).
set "BUILD_CONFIG=%~1"
if "%BUILD_CONFIG%"=="" (
    set "BUILD_CONFIG=RelWithDebInfo"
)

set "BUILD_DIR=build\x64"
set "CMAKE_GENERATOR=Visual Studio 17 2022"
set "CMAKE_ARCH=x64"
set "CMAKE_TOOLSET=v143"

set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%..\.." >nul 2>&1 || goto :fail

call :locate_cmake
if not defined CMAKE_EXE goto :fail

echo [INFO] Configuring project with %CMAKE_GENERATOR% (%CMAKE_ARCH%)...
"%CMAKE_EXE%" -S . -B "%BUILD_DIR%" -G "%CMAKE_GENERATOR%" -T "%CMAKE_TOOLSET%" -A "%CMAKE_ARCH%"
if errorlevel 1 goto :fail

echo [INFO] Generating soh.otr assets...
"%CMAKE_EXE%" --build "%BUILD_DIR%" --target GenerateSohOtr --config "%BUILD_CONFIG%"
if errorlevel 1 goto :fail

echo [INFO] Building Ship of Harkinian (%BUILD_CONFIG%)...
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config "%BUILD_CONFIG%"
if errorlevel 1 goto :fail

echo [SUCCESS] Build completed. Output available in "%BUILD_DIR%\%BUILD_CONFIG%".
set "ERROR_CODE=0"
goto :cleanup

:locate_cmake
if defined CMAKE_EXE goto :eof

if defined CMAKE_PATH (
    if exist "%CMAKE_PATH%" (
        set "CMAKE_EXE=%CMAKE_PATH%"
        goto :eof
    )
)

for /f "delims=" %%i in ('where cmake 2^>nul') do (
    set "CMAKE_EXE=%%i"
    goto :eof
)

if not defined CMAKE_EXE (
    if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
        set "CMAKE_EXE=%ProgramFiles%\CMake\bin\cmake.exe"
    ) else if exist "%ProgramFiles(x86)%\CMake\bin\cmake.exe" (
        set "CMAKE_EXE=%ProgramFiles(x86)%\CMake\bin\cmake.exe"
    ) else (
        echo [ERROR] Unable to locate cmake.exe. Install CMake or set the CMAKE_PATH environment variable.
    )
)

goto :eof

:fail
set "ERROR_CODE=%errorlevel%"
if "%ERROR_CODE%"=="0" set "ERROR_CODE=1"
echo [ERROR] Build failed with error code %ERROR_CODE%.

:cleanup
popd >nul 2>&1
endlocal & exit /b %ERROR_CODE%
