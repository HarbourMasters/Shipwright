# Building Ship of Harkinian

## Windows

1. Requires Visual Studio 2022 Community Edition && `python3, cmake, git` (can be installed via chocolatey or manually)
2. Clone the Ship of Harkinian repository
3. Place one or more [compatible](#compatible-roms) roms in the `OTRExporter` directory with namings of your choice

_Note: Instructions assume using powershell_
```powershell
# Navigate to the Shipwright repo within powershell. ie: cd "C:\yourpath\Shipwright"
cd Shipwright

# Setup cmake project
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64 # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --target ExtractAssets # --config Release (if you're packaging)
# Compile project
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 # --config Release (if you're packaging)

# Now you can run the executable in .\build\x64

# If you need to clean the project you can run
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64 --target clean
```

### Developing SoH
With the cmake build system you have two options for woriking on the project:

#### Visual Studio
To develop using Visual Studio you only need to use cmake to generate the solution file:
```powershell
# Generates Ship.sln at the root directory
& 'C:\Program Files\CMake\bin\cmake' -S . -G "Visual Studio 17 2022" -T v142 -A x64
```

#### Visual Studio Code or another editor
To develop using Visual Studio Code or another editor you only need to open the repository in it.
To build you'll need to follow the instructions from the building section.
_Experimental: You can also use another build system entirely rathen than MSVC like [Ninja](https://ninja-build.org/) for better performance and compatibility with Visual Studio Code and its CMake plugin._


### Generating the distributable
After compiling the project you can generate the distributable by running:
```powershell
# Go to build folder
cd "build/x64"
# Generate
& 'C:\Program Files\CMake\bin\cpack.exe' -G ZIP
```

## Linux
1. Requires `gcc, x11, curl, python3, sdl2, libpng, glew, ninja, cmake`

**Important: For maximum performance make sure you have ninja build tools installed!**

_Note: If you're using Visual Studio Code, the [cpack plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) makes it very easy to just press run and debug._

```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd Shipwright
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
1. Requires Xcode (or xcode-tools) && `sdl2, libpng, glew, ninja, cmake` (can be installed via homebrew, macports, etc)

**Important: For maximum performance make sure you have ninja build tools installed!**

_Note: If you're using Visual Studio Code, the [cpack plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) makes it very easy to just press run and debug._

```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd ShipWright
# Copy the baserom to the OTRExporter folder
cp <path to your ROM> OTRExporter
# Generate Ninja project
cmake -H. -Bbuild-cmake -GNinja # -DCMAKE_BUILD_TYPE:STRING=Release (if you're packaging)
# Extract assets & generate OTR (run this anytime you need to regenerate OTR)
cmake --build build-cmake --target ExtractAssets
# Compile the project
cmake --build build-cmake # --config Release (if you're packaging)

# Now you can run the executable in ./build-cmake/soh/soh-macos
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

