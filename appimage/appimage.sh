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
cp -r Release/* AppDir/usr/bin

chmod +x AppDir/usr/bin/{soh.elf,OTRGui,soh.sh}
 
cd AppDir && ln -s ./usr/bin/soh.sh ./AppRun && cd ..

export UPD_INFO="gh-releases-zsync|HarbourMasters|Shipwright-linux|zelda64|SOH-Linux.AppImage.zsync"
./linuxdeploy-x86_64.AppImage                   \
  --appdir="$GITHUB_WORKSPACE"/AppDir/          \
  -d "$GITHUB_WORKSPACE"/AppDir/soh.desktop     \
  -i "$GITHUB_WORKSPACE"/AppDir/soh.png         \
  -e "$GITHUB_WORKSPACE"/AppDir/usr/bin/soh.elf \
  -e "$GITHUB_WORKSPACE"/AppDir/usr/bin/assets/extractor/ZAPD.out
 
mv "$GITHUB_WORKSPACE"/AppDir/usr/bin/ZAPD.out "$GITHUB_WORKSPACE"/AppDir/usr/bin/assets/extractor/ZAPD.out
cd "$GITHUB_WORKSPACE"/AppDir/usr/bin/assets/extractor/
patchelf --set-rpath "\$ORIGIN/../../../lib" ZAPD.out
cd "$GITHUB_WORKSPACE"
 
./appimagetool-x86_64.AppImage "$GITHUB_WORKSPACE"/AppDir "SOH-Linux.AppImage" -u "gh-releases-zsync|HarbourMasters|Shipwright-linux|zelda64|SOH-Linux.AppImage.zsync"

mkdir -p "$GITHUB_WORKSPACE"/artifacts/ 
mv SOH-Linux.AppImage* "$GITHUB_WORKSPACE"/artifacts/
ls -al artifacts
