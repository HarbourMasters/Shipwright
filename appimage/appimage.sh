#!/bin/bash
  
curl -sSfLO "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod a+x linuxdeploy*.AppImage
curl -sSfLO "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage" 
chmod a+x appimagetool*.AppImage
 
mkdir -p AppDir/usr/bin
cp appimage/{soh.desktop,soh.png,soh.sh} AppDir/
curl -sSfL https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt -o AppDir/usr/bin/gamecontrollerdb.txt

mkdir -p AppDir/usr/share/applications 
mkdir -p AppDir/usr/share/icons/hicolor/scalable/apps
mkdir -p AppDir/usr/lib

mv AppDir/soh.sh AppDir/usr/bin
cp -r build/* AppDir/usr/bin

chmod +x AppDir/usr/bin/{soh.elf,OTRGui,soh.sh}
 
cd AppDir && ln -s ./usr/bin/soh.sh ./AppRun && cd ..

export UPD_INFO="gh-releases-zsync|HarbourMasters|Shipwright-linux|zelda64|SOH-Linux.AppImage.zsync"
./linuxdeploy-x86_64.AppImage                   \
  --appdir=./AppDir/          \
  -d ./AppDir/soh.desktop     \
  -i ./AppDir/soh.png         \
  -e ./AppDir/usr/bin/soh.elf \
  -e ./AppDir/usr/bin/assets/extractor/ZAPD.out
 
mv ./AppDir/usr/bin/ZAPD.out ./AppDir/usr/bin/assets/extractor/ZAPD.out
cd ./AppDir/usr/bin/assets/extractor/
patchelf --set-rpath "\$ORIGIN/../../../lib" ZAPD.out
cd /soh
 
./appimagetool-x86_64.AppImage ./AppDir "SOH-Linux.AppImage" -u "gh-releases-zsync|HarbourMasters|Shipwright-linux|zelda64|SOH-Linux.AppImage.zsync"
