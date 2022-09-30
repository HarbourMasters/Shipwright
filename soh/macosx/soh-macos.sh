#!/bin/bash

SNAME="$(dirname $0)"
export DATA_SHARE="$HOME/Library/Application Support/com.shipofharkinian.soh"
export SNAME
export RESPATH="${SNAME%/MacOS*}/Resources"
export LIBPATH="${SNAME%/MacOS*}/Frameworks"
export DYLD_FALLBACK_LIBRARY_PATH="$LIBPATH"

while [ ! -e "$DATA_SHARE/oot.otr" ]; do
	ASSETDIR="$(mktemp -d /tmp/assets-XXXXX)"
	export ASSETDIR
	cp -r "$RESPATH/assets" "$ASSETDIR"
	mkdir -p "$ASSETDIR"/tmp
	mkdir -p "$ASSETDIR"/Extract/assets
	DROPROM="$(osascript -ss - "$ASSETDIR" <<-EOF
	set romFile to choose file of type {"b64","n64","v64","z64"} with prompt "Please select your ROM:"
	set destinationFolder to POSIX file "$ASSETDIR"
	tell application "Finder"
	duplicate romFile to destinationFolder
	end tell
	EOF
	)"
	"$DROPROM"
	for rom in "$ASSETDIR"/*.*64
	do
		if [ ! -e "$rom" ]; then
			echo "no ROM"
			osascript -e 'display dialog "Select ROM to generate OTR" giving up after 5'
			rm -r "$ASSETDIR"
			exit
		fi
	done
	cp "$ASSETDIR"/*.*64 "$ASSETDIR"/tmp/rom.z64
	cp -r "$ASSETDIR"/assets/game/ship_of_harkinian "$ASSETDIR"/Extract/assets/
	cd "$ASSETDIR" || return
	ROMHASH="$(shasum "$ASSETDIR"/tmp/rom.z64 | awk '{ print $1 }')"
	case "$ROMHASH" in
	cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
		export ROM=GC_NMQ_D;;
	0227d7c0074f2d0ac935631990da8ec5914597b4)
		export ROM=GC_NMQ_PAL_F;;
	*)
		WRONGHASH="$(osascript -ss - "$ROMHASH" <<-EOF
		display dialog "Incompatible ROM hash $ROMHASH" \
		with title "Incompatible ROM hash" \
		with icon caution \
		giving up after 5
		EOF
		)"
		"$WRONGHASH"
		rm -r "$ASSETDIR"
		exit;;
	esac
	echo "$ROM"

	osascript -e 'display notification "Processing OTR..." with title "SOH: Generating OTR"'
	assets/extractor/ZAPD.out ed -i assets/extractor/xmls/"${ROM}" -b tmp/rom.z64 -fl assets/extractor/filelists -o placeholder -osf placeholder -gsf 1 -rconf assets/extractor/Config_"${ROM}".xml -se OTR
	if [ -e "$PWD"/oot.otr ]; then
		osascript -e 'display notification "OTR Successfully Generated" with title "SOH: Generating OTR"'
		if [ ! -e "$DATA_SHARE" ]; then	mkdir "$DATA_SHARE"; fi
			cp "$ASSETDIR"/oot.otr "$DATA_SHARE"
			rm -r "$ASSETDIR"
		fi
		break
done

arch_name="$(uname -m)"
launch_arch="arm64"
if [ "${arch_name}" = "x86_64" ] && [ "$(sysctl -in sysctl.proc_translated)" != "1" ]; then
	launch_arch="x86_64"
fi

arch -${launch_arch} "$RESPATH"/soh-macos
exit
