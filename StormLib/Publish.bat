@echo off
rem This BAT file updates the ZIP file that is to be uploaded to web
rem Only use when both 32-bit and 64-bit are properly compiled

set STORMLIB_NAME=stormlib-9.00

echo Creating %STORMLIB_NAME%.zip ...
cd \Ladik\Appdir
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\doc\*
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\src\*
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\storm_dll\*
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\StormLib.xcodeproj\*
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\stormlib_dll\*
zip.exe -ur9 ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\test\*
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\CMakeLists.txt
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\makefile.*
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\Info.plist
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\*.bat
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\*.sln
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\*.vcproj
zip.exe -u9  ..\WWW\web\download\%STORMLIB_NAME%.zip StormLib\*.vcxproj
echo.

echo Press any key to exit ...
pause >nul
