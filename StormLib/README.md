# StormLib

This is official repository for the StomLib library, an open-source project that can work with Blizzard MPQ archives.

## Installation and basic usage
### Linux
1. Download latest release
2. Install StormLib:
```
$ cd <path-to-StormLib>
$ cmake CMakeLists.txt
$ make
$ make install
```
3. Include StormLib in your project: `#include <StormLib.h>`
4. Make sure you compile your project with `-lstorm -lz -lbz2`

### Windows (Visual Studio 2008)
1. Download the latest release of StormLib
2. Open the solution file `StormLib_vs08.sln` in Visual Studio 2008
3. Choose "Build / Batch Build" and select every build of "StormLib"
4. Choose "Rebuild"
5. The result libraries are in `.\bin\Win32` and `.\bin\x64`

### Windows (Visual Studio 2017 or 2019)
0. Make sure you have SDK 10.0.17134.0 installed
1. Download the latest release of StormLib
2. Open the solution file `StormLib_vs19.sln` in Visual Studio 2017/2019
3. Choose "Build / Batch Build" and select every build of "StormLib"
4. Choose "Rebuild"
5. The result libraries are in `.\bin\Win32` and `.\bin\x64`

### Windows (Test Project)
1. Include the main StormLib header: `#include <StormLib.h>`
2. Set the correct library directory for StormLibXYZ.lib:
   * X: D = Debug, R = Release
   * Y: A = ANSI build, U = Unicode build
   * Z: S = Using static CRT library, D = Using Dynamic CRT library
3. Rebuild
