# Building Ship of Harkinian

## Windows

Requires:
  * At least 8GB of RAM (machines with 4GB have seen complier failures)
  * Visual Studio 2022 Community Edition with the C++ feature set
  * One of the Windows SDKs that comes with Visual Studio, for example the current Windows 10 version 10.0.19041.0
  * The `MSVC v142 - VS 2019 C++ build tools` component of Visual Studio
  * Python 3 (can be installed manually or as part of Visual Studio)
  * Git (can be installed manually or as part of Visual Studio)
  * Cmake (can be installed via chocolatey or manually)

During installation, check the "Desktop development with C++" feature set:

![image](https://user-images.githubusercontent.com/30329717/183511274-d11aceea-7900-46ec-acb6-3f2cc110021a.png)
Doing so should also check one of the Windows SDKs by default.  Then, in the installation details in the right-hand column, make sure you also check the v142 toolset. 

You can also find the v142 toolset by searching through the individual components tab:  

![image](https://user-images.githubusercontent.com/30329717/183521169-ead6a73b-a1bf-4e99-aab8-441746d8f08e.png)
While you're there, you can also install Python 3 and Git if needed.

1. Clone the Ship of Harkinian repository
2. Place one or more [compatible](#compatible-roms) roms in the `OTRExporter` directory with namings of your choice

_Note: Instructions assume using powershell_
```powershell
# Navigate to the Shipwright repo within powershell. ie: cd "C:\yourpath\Shipwright"
cd Shipwright

# Setup cmake project
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64 # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# or for VS2019
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 16 2019" -T v142 -A x64
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --target ExtractAssets # --config Release (if you're packaging)
# Compile project
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 # --config Release (if you're packaging)

# Now you can run the executable in .\build\x64

# If you need to clean the project you can run
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --target clean
```

### Developing SoH
With the cmake build system you have two options for working on the project:

#### Visual Studio
To develop using Visual Studio you only need to use cmake to generate the solution file:
```powershell
# Generates Ship.sln at `build/x64` for Visual Studio 2022
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64
# or for Visual Studio 2019
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 16 2019" -T v142 -A x64
```

#### Visual Studio Code or another editor
To develop using Visual Studio Code or another editor you only need to open the repository in it.
To build you'll need to follow the instructions from the building section.

_Note: If you're using Visual Studio Code, the [cpack plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) makes it very easy to just press run and debug._

_Experimental: You can also use another build system entirely rather than MSVC like [Ninja](https://ninja-build.org/) for possibly better performance._


### Generating the distributable
After compiling the project you can generate the distributable by running:
```powershell
# Go to build folder
cd "build/x64"
# Generate
& 'C:\Program Files\CMake\bin\cpack.exe' -G ZIP
```

## Linux
Requires `gcc >= 10, x11, curl, python3, sdl2 >= 2.0.22, libpng, glew >= 2.2, ninja, cmake, lld`

**Important: For maximum performance make sure you have ninja build tools installed!**

_Note: If you're using Visual Studio Code, the [cpack plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) makes it very easy to just press run and debug._

```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd Shipwright
# Clone the submodule libultraship
git submodule update --init
# Copy the baserom to the OTRExporter folder
cp <path to your ROM> OTRExporter
# Generate Ninja project
cmake -H. -Bbuild-cmake -GNinja # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
cmake --build build-cmake --target ExtractAssets
# Compile the project
cmake --build build-cmake # --config Release (if you're packaging)

# Now you can run the executable in ./build-cmake/soh/soh.elf
# To develop the project open the repository in VSCode (or your preferred editor)

# If you need to clean the project you can run
cmake --build build-cmake --target clean
```

### Generating a distributable
After compiling the project you can generate a distributable by running of the following:
```bash
# Go to build folder
cd build-cmake
# Generate
cpack -G DEB
cpack -G ZIP
cpack -G External (creates appimage)
```

## macOS
Requires Xcode (or xcode-tools) && `sdl2, libpng, glew, ninja, cmake` (can be installed via homebrew, macports, etc)

**Important: For maximum performance make sure you have ninja build tools installed!**

_Note: If you're using Visual Studio Code, the [cpack plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) makes it very easy to just press run and debug._

```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd ShipWright
# Clone the submodule libultraship
git submodule update --init
# Copy the baserom to the OTRExporter folder
cp <path to your ROM> OTRExporter
# Generate Ninja project
cmake -H. -Bbuild-cmake -GNinja # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
cmake --build build-cmake --target ExtractAssets
# Compile the project
cmake --build build-cmake # --config Release (if you're packaging)

# Copy oot.otr into the Application Support directory
cp build-cmake/soh/oot.otr ~/Library/Application\ Support/com.shipofharkinian.soh/

# Now you can run the executable file:
./build-cmake/soh/soh-macos
# To develop the project open the repository in VSCode (or your preferred editor)

# If you need to clean the project you can run
cmake --build build-cmake --target clean
```

### Generating a distributable
After compiling the project you can generate a distributable by running of the following:
```bash
# Go to build folder
cd build-cmake
# Generate
cpack
```

## Switch
1. Requires that your build machine is setup with the tools necessary for your platform above
2. Requires that you have the switch build tools installed 
3. Clone the Ship of Harkinian repository
4. Place one or more [compatible](#compatible-roms) roms in the `OTRExporter` directory with namings of your choice

```bash
cd Shipwright
# Setup cmake project for your host machine
cmake -H. -Bbuild-cmake -GNinja
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
cmake --build build-cmake --target ExtractAssets
# Setup cmake project for building for Switch
cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
# Build project and generate nro
cmake --build build-switch --target soh_nro

# Now you can run the executable in ./build-switch/soh/soh.nro
# To develop the project open the repository in VSCode (or your preferred editor)
```

## Wii U
1. Requires that your build machine is setup with the tools necessary for your platform above
2. Requires that you have the Wii U build tools installed 
3. Clone the Ship of Harkinian repository
4. Place one or more [compatible](#compatible-roms) roms in the `OTRExporter` directory with namings of your choice

```bash
cd Shipwright
# Setup cmake project for your host machine
cmake -H. -Bbuild-cmake -GNinja
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
cmake --build build-cmake --target ExtractAssets
# Setup cmake project for building for Wii U
cmake -H. -Bbuild-wiiu -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/WiiU.cmake # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# Build project and generate rpx
cmake --build build-wiiu --target soh # --target soh_wuhb (for building .wuhb) 

# Now you can run the executable in ./build-wiiu/soh/soh.rpx or the Wii U Homebrew Bundle in ./build-wiiu/soh/soh.wuhb
# To develop the project open the repository in VSCode (or your preferred editor)
```

# Compatible Roms
```
OOT_PAL_GC      checksum 0x09465AC3
OOT_PAL_GC_DBG1 checksum 0x871E1C92 (debug non-master quest)
```

# OTRExporter Usage

The OTRExporter exports an `oot.otr` archive file which Ship of Harkinian requires to play.

Use the `extract_assets.py` script file to run the exporter using any of the following methods:
1) Double click on the script after placing one or more roms in the directory.
2) Drag & Drop a rom onto the script.
3) In a terminal run `python3 extract_assets.py` after placing one or more roms in the directory.
4) In a terminal run `python3 extract_assets.py <path_to_rom>`

If the script finds multiple roms the user is prompted which to use. Selection is done using the number keys and then pressing the carriage return key.
