Windows
=======

libpng, zlib and bzip2 will be used from VCPKG.
My vcpkg is located at d:/VCPKG
I also created a new triplets to be used.

For 64 bit:
~~~
x64-windows-v142:
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_PLATFORM_TOOLSET v142)
~~~
same for 32 bit (only required for classical build):
~~~
x86-windows-v142:
set(VCPKG_TARGET_ARCHITECTURE x86)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_PLATFORM_TOOLSET v142)
~~~
prepare libs:
~~~
D:\vcpkg\vcpkg.exe install libpng --triple=x86-windows-v142 (only required for classical build)
D:\vcpkg\vcpkg.exe install libpng --triple=x64-windows-v142
D:\vcpkg\vcpkg.exe install zlib   --triple=x86-windows-v142 (only required for classical build)
D:\vcpkg\vcpkg.exe install zlib   --triple=x64-windows-v142
D:\vcpkg\vcpkg.exe install bzip2  --triple=x86-windows-v142 (only required for classical build)
D:\vcpkg\vcpkg.exe install bzip2  --triple=x64-windows-v142
~~~

~~~
git clone https://github.com/th-2021/Shipwright-cmake.git
~~~

All-in-One with packaging (64 bit only)
---------------------------------------
copy your ROM into Shipwright-cmake/OTRExporter
~~~
cd Shipwright-cmake
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64  "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --config Release
cd  .\build\x64
& 'C:\Program Files\CMake\bin\cpack.exe' -G ZIP
~~~

Classical build (soh 32 bit)
----------------------------
~~~
#
# build SoH
#
cd shipwright-cmake
cd OTRExporter
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64  "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --config Release
# copy ROM for asset extraction
.\extract_assets.py
cd ../soh
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/Win32" -G "Visual Studio 17 2022" -T v142 -A Win32 "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x86-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\Win32 --config Release
cd ../OTRGui
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64 "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --config Release
~~~

Linux (Ubuntu 22.04)
====================

All-in-One with packaging support (only 64 bit):
------------------------------------------------

execute at top-level:
~~~
cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake
~~~

now in build-cmake:
~~~
cpack -G DEB
cpack -G ZIP
cpack -G External	(does appimage - experimental)
~~~

Classical build (soh 32 bit)
----------------------------

~~~
cd ShipWright
~~~

(will be built with below cmake commands) - no extra build needed

~~~
cd OTRExporter


cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake 

./extract_assets_cmake.py
  
cd ../soh
cmake -H. -Bbuild-cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=CMake/Linux32bit-toolchain.cmake
cmake --build build-cmake 

cd ../OTRGui
cmake -H. -Bbuild-cmake -GNinja -DOpenGL_GL_PREFERENCE="GLVND" -DCMAKE_BUILD_TYPE="Release"
cmake --build build-cmake  
cp build-cmake/ZAPD/ZAPD.out build-cmake/assets/extractor

~~~

macOS (M1)
==========

using native build (not docker)
(will be built with below cmake commands) - no extra build needed

~~~
cd OTRExporter

cmake -S . -B build-cmake -DCMAKE_TOOLCHAIN_FILE=../soh/CMake/Mac-gnu-toolchain.cmake
cmake --build build-cmake

./extract_assets_cmake.py
  
cd ../soh
cmake -S . -B build-cmake -DCMAKE_TOOLCHAIN_FILE=CMake/Mac-gnu-toolchain.cmake
cmake --build build-cmake 
~~~
