#!/bin/bash
  
curl -sSfLO "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod a+x linuxdeploy*.AppImage
curl -sSfL https://github.com$(curl https://github.com/probonopd/go-appimage/releases | grep "mkappimage-.*-x86_64.AppImage" | head -n 1 | cut -d '"' -f 2) -o mkappimage.AppImage
chmod a+x mkappimage.AppImage
 
mkdir -p AppDir/usr/bin
cp appimage/{soh.desktop,soh.sh} AppDir/
cp soh/macosx/sohIcon.png AppDir/soh.png
curl -sSfL https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt -o AppDir/usr/bin/gamecontrollerdb.txt

mkdir -p AppDir/usr/share/applications 
mkdir -p AppDir/usr/share/icons/hicolor/scalable/apps
mkdir -p AppDir/usr/lib

mv AppDir/soh.sh AppDir/usr/bin
cp -r build/* AppDir/usr/bin

chmod +x AppDir/usr/bin/{soh.elf,OTRGui,soh.sh}
 
cd AppDir && ln -s ./usr/bin/soh.sh ./AppRun && cd ..

export UPD_INFO="gh-releases-zsync|HarbourMasters|Shipwright-linux|develop|SOH-Linux.AppImage.zsync"
./linuxdeploy-x86_64.AppImage --appimage-extract-and-run  \
  --appdir=./AppDir/          \
  -d ./AppDir/soh.desktop     \
  -i ./AppDir/soh.png         \
  -e ./AppDir/usr/bin/soh.elf

cd /soh
 
VERSION=Linux ./mkappimage.AppImage --appimage-extract-and-run ./AppDir # "SOH-Linux-x86_64.AppImage"
mv SOH-Linux-x86_64.AppImage SOH-Linux.AppImage # Keep Original Name
