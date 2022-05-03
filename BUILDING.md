# Building Ship of Harkinian

## Windows

 1. Install [Python](https://www.python.org/ftp/python/3.10.2/python-3.10.2-amd64.exe)
 2. Install [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/community/)
 2b. In the Visual Studio Installer, install `MSVC v142 - VS 2019 C++`.
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
git clone git@github.com:HarbourMasters/ShipWright.git
cd ShipWright
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
make setup -j$(nproc)
# Compile the code
make -j $(nproc)
```

# Compatible Roms
```
OOT_PAL_GC      checksum 0x09465AC3
OOT_PAL_GC_DBG1 checksum 0x871E1C92 (debug non-master quest)
```
