#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"/../..

export PATH="$HERE"/bin:"$HERE"/usr/bin:"$PATH"
export LD_LIBRARY_PATH="$HERE"/usr/lib:"$LD_LIBRARY_PATH"
export ZENITY=$(command -v zenity)

while [[ ! -e "$PWD"/oot.otr ]]; do
	export ASSETDIR="$(mktemp -d /tmp/assets-XXXXX)"
	ln -s "$HERE"/usr/bin/{assets,soh.elf,OTRGui} "$ASSETDIR"
	export OLDPWD="$PWD"
	mkdir -p "$ASSETDIR"/tmp
	mkdir -p "$ASSETDIR"/Extract/assets
	if [ -e "$PWD"/*.*64 ]; then
		ln -s "$OLDPWD"/*.*64 "$ASSETDIR"/tmp/rom.z64
		cp -r "$ASSETDIR"/assets/game/ship_of_harkinian "$ASSETDIR"/Extract/assets/
		cd "$ASSETDIR"
		ROMHASH=$(sha1sum -b "$ASSETDIR"/tmp/rom.z64 | awk '{ print $1 }')
		case "$ROMHASH" in
		cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
			ROM=GC_NMQ_D;;
		0227d7c0074f2d0ac935631990da8ec5914597b4)
			ROM=GC_NMQ_PAL_F;;
		*)
  			if [ -n "$ZENITY" ]; then
				zenity --error --timeout=10 --text="ROM hash <b>$ROMHASH</b> does not match" --title="Incorrect ROM file" --width=500 --width=200
			else
  				echo -e "\nrom hash does not match\n"
  			fi
  			exit;;
		esac
		if [ -n "$ZENITY" ]; then
			(echo "# 25%"; echo "25"; sleep 2; echo "# 50%"; echo "50"; sleep 3; echo "# 75%"; echo "75"; sleep 2; echo "# 100%"; echo "100"; sleep 3) |
			zenity --progress --title="OTR Generating..." --timeout=10 --percentage=0 --icon-name=soh --window-icon=soh.png --height=80 --width=400 &
		else
			echo "Processing..."
		fi
		assets/extractor/ZAPD.out ed -eh -i assets/extractor/xmls/"${ROM}" -b tmp/rom.z64 -fl assets/extractor/filelists -o placeholder -osf placeholder -gsf 1 -rconf assets/extractor/Config_"${ROM}".xml -se OTR > /dev/null 2>&1
		cp "$ASSETDIR"/oot.otr "$OLDPWD"
		echo "Restart $APPIMAGE to play!" 
		sleep 3
		rm -r "$ASSETDIR"
		break
	else
		if [ -n "$ZENITY" ]; then
			zenity --error --timeout=5 --text="Place ROM in $OWD" --title="Missing ROM file" --width=500 --width=200
		else
			echo -e "\nPlace ROM in this folder\n"
		fi	
		exit
	fi
done	
	cd "$OWD"
	ln -s "$HERE"/usr/bin/gamecontrollerdb.txt "$PWD"
	"$HERE"/usr/bin/soh.elf
	unlink "$PWD"/gamecontrollerdb.txt
exit
