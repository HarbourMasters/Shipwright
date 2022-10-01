#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"/../..

export PATH="$HERE"/bin:"$HERE"/usr/bin:"$PATH"
export LD_LIBRARY_PATH="$HERE"/usr/lib:"$LD_LIBRARY_PATH"
export ZENITY=$(command -v zenity)

if [ -z ${SHIP_HOME+x} ]; then
export SHIP_HOME=$PWD
fi

while [[ ! -e "$SHIP_HOME"/oot.otr ]]; do
	export ASSETDIR="$(mktemp -d /tmp/assets-XXXXX)"
	ln -s "$HERE"/usr/bin/{assets,soh.elf,OTRGui} "$ASSETDIR"
	export OLDPWD="$PWD"
	mkdir -p "$ASSETDIR"/tmp
	mkdir -p "$ASSETDIR"/Extract
	if [ -e "$SHIP_HOME"/*.*64 ]; then
		ln -s "$SHIP_HOME"/*.*64 "$ASSETDIR"/tmp/rom.z64
		cp -r "$ASSETDIR"/assets/game "$ASSETDIR"/Extract/assets
		cd "$ASSETDIR"
		ROMHASH=$(sha1sum -b "$ASSETDIR"/tmp/rom.z64 | awk '{ print $1 }')
		case "$ROMHASH" in
		cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
			ROM=GC_NMQ_D;;
		0227d7c0074f2d0ac935631990da8ec5914597b4)
			ROM=GC_NMQ_PAL_F;;
		50bebedad9e0f10746a52b07239e47fa6c284d03)
			ROM=GC_MQ_D;;
		079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
			ROM=GC_MQ_D;;	
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
		cp "$ASSETDIR"/oot.otr "$SHIP_HOME"
		echo "Restart $APPIMAGE to play!" 
		sleep 3
		rm -r "$ASSETDIR"
		break
	else
		if [ -n "$ZENITY" ]; then
			zenity --error --timeout=5 --text="Place ROM in $SHIP_HOME" --title="Missing ROM file" --width=500 --width=200
		else
			echo -e "\nPlace ROM in this folder\n"
		fi	
		exit
	fi
done	
	(cd "$HERE/usr/bin"; ./soh.elf)
exit
