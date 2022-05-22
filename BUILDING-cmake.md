libpng and zlib will be used from VCPKG.
My vcpkg is located at d:/VCPKG
I also created a new triplet to be used.

~~~
x64-windows-v142:
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_PLATFORM_TOOLSET v142)
~~~

prepare libpng:
~~~
D:\vcpkg\vcpkg.exe install libpng --triple=x86-windows-v142
~~~

~~~
git clone https://github.com/th-2021/Shipwright-cmake.git
#
# build SoH
#
cd shipwright-cmake
cd OTRExporter
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64  "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --config Release
# copy ROM for asset extraction
#.\extract_assets.py
cd ../soh
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/Win32" -G "Visual Studio 17 2022" -T v142 -A Win32 "-DCMAKE_TOOLCHAIN_FILE=D:\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x86-windows-v142"
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\Win32 --config Release
~~~

Linux (Ubuntu 22.04):

using native build (not docker)

~~~
cd ShipWright
git clone https://github.com/ladislav-zezula/StormLib external/StormLib
git clone https://github.com/Perlmint/glew-cmake.git external/glew
~~~

(will be built with below cmake commands) - no extra build needed

~~~
cd OTRExporter

# change the path to ZAPD in extract_assets.py
sed "s%../ZAPDTR/ZAPD.out%build-cmake/ZAPD/ZAPD.out%" < extract_assets.py > extract_assets_cmake.py
chmod +x extract_assets_cmake.py

cmake -S . -B build-cmake 
cmake --build build-cmake 

./extract_assets_cmake.py
  
cd ../soh
cmake -S . -B build-cmake -DCMAKE_TOOLCHAIN_FILE=CMake/Linux32bit-toolchain.cmake
cmake --build build-cmake 
~~~
