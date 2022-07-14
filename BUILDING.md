# Building Ship of Harkinian

## Windows

 1. Requires [Python](https://www.python.org/downloads/) >= 3.6.
 2. Install [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/community/)
 3. In the Visual Studio Installer, install `MSVC v142 - VS 2019 C++`.
 4. Clone the Ship of Harkinian repository.
 5. Place one or more [compatible](#compatible-roms) roms in the `OTRExporter` directory with namings of your choice.
 6. Run `OTRExporter/OTRExporter.sln`.
 7. Switch the solution to `Release x64`.
 8. Build the solution.
 9. Launching `OTRExporter/extract_assets.py` will generate an `oot.otr` archive file in `OTRExporter/oot.otr`.
 10. Run `soh/soh.sln`
 11. Switch the solution to `Release x86`.
 12. Build the solution.
 13. Copy the `OTRExporter/oot.otr` archive file to `soh/Release`.
 14. Launch `soh.exe`.

## Linux

```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd Shipwright
# Copy the baserom to the OTRExporter folder
cp <path to your ROM> OTRExporter
# Build the docker image
sudo docker build . -t soh
# Run the docker image with the working directory mounted to /soh
sudo docker run --rm -it -v $(pwd):/soh soh /bin/bash
```
Inside the Docker container:
```bash
# Clone and build StormLib
git clone https://github.com/ladislav-zezula/StormLib external/StormLib
cmake -B external/StormLib/build -S external/StormLib
cmake --build external/StormLib/build
cp external/StormLib/build/libstorm.a external
cp /usr/local/lib/libGLEW.a external

cd soh
# Extract the assets/Compile the exporter/Run the exporter
make setup -j$(nproc) OPTFLAGS=-O2 DEBUG=0
# Compile the code
make -j $(nproc) OPTFLAGS=-O2 DEBUG=0
```

## macOS

1. Requires Xcode (or xcode-tools) && `sdl2, libpng, glew, dylibbundler` (can be installed via brew, etc)
```bash
# Clone the repo
git clone https://github.com/HarbourMasters/Shipwright.git
cd ShipWright
# Copy the baserom to the OTRExporter folder
cp <path to your ROM> OTRExporter

cd soh
# Extract the assets/Compile the exporter/Run the exporter
# -jX defines number of cores to use for compilation - lower or remove entirely if having issues
make setup -j8 DEBUG=0
# Compile the code (watch the -j parameter as above)
make -j8 DEBUG=0
# Create macOS app bundle
make appbundle
```
9. Copy your OTR file to ~/Library/Application\ Support/com.shipofharkinian.soh
10. Launch soh app in the soh folder!

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

