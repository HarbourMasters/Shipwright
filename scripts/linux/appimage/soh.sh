#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"/../..

export PATH="$HERE"/bin:"$HERE"/usr/bin:"$PATH"
export LD_LIBRARY_PATH="$HERE"/usr/lib:"$LD_LIBRARY_PATH"
export ZENITY=$(command -v zenity)

if [ -z ${SHIP_HOME+x} ]; then
export SHIP_HOME=$PWD
fi

if [ -z ${SHIP_BIN_DIR+x} ]; then
export SHIP_BIN_DIR="$HERE/usr/bin"
fi

if [[ ! -e "$SHIP_HOME"/mods ]]; then
    mkdir -p "$SHIP_HOME"/mods
    touch "$SHIP_HOME"/mods/custom_otr_files_go_here.txt
fi

while [[ (! -e "$SHIP_HOME"/oot.otr) || (! -e "$SHIP_HOME"/oot-mq.otr) ]]; do
        for romfile in "$SHIP_HOME"/*.*64
        do
            if [[ -e $romfile ]]; then
                export ASSETDIR="$(mktemp -d /tmp/assets-XXXXX)"
                ln -s "$HERE"/usr/bin/{assets,soh.elf,ZAPD} "$ASSETDIR"
                export OLDPWD="$PWD"
                mkdir -p "$ASSETDIR"/tmp
	            mkdir -p "$ASSETDIR"/Extract
                ln -s "$romfile" "$ASSETDIR"/tmp/rom.z64
                cd "$ASSETDIR"
                ROMHASH=$(sha1sum -b "$ASSETDIR"/tmp/rom.z64 | awk '{ print $1 }')
                case "$ROMHASH" in
                cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
                    if [[ ! -e "$SHIP_HOME"/oot.otr ]]; then
                        ROM=GC_NMQ_D
                        OTRNAME="oot.otr"
                    fi
                    ;;
                0227d7c0074f2d0ac935631990da8ec5914597b4)
                    if [[ ! -e "$SHIP_HOME"/oot.otr ]]; then
                        ROM=GC_NMQ_PAL_F
                        OTRNAME="oot.otr"
                    else
                        continue
                    fi
                    ;;
                cfbb98d392e4a9d39da8285d10cbef3974c2f012)
                    if [[ ! -e "$SHIP_HOME"/oot.otr ]]; then
                        ROM=N64_PAL_11
                        OTRNAME="oot.otr"
                    else
                        continue
                    fi
                    ;;
                50bebedad9e0f10746a52b07239e47fa6c284d03)
                    if [[ ! -e "$SHIP_HOME"/oot-mq.otr ]]; then
                        ROM=GC_MQ_D
                        OTRNAME="oot-mq.otr"
                    else
                        continue
                    fi
                    ;;
                079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
                    if [[ ! -e "$SHIP_HOME"/oot-mq.otr ]]; then
                        ROM=GC_MQ_D
                        OTRNAME="oot-mq.otr"
                    else
                        continue
                    fi
                    ;;
                cfecfdc58d650e71a200c81f033de4e6d617a9f6)
                    if [[ ! -e "$SHIP_HOME"/oot-mq.otr ]]; then
                        ROM=GC_MQ_D
                        OTRNAME="oot-mq.otr"
                    else
                        continue
                    fi
                    ;;
                517bd9714c73cb96c21e7c2ef640d7b55186102f)
                    if [[ ! -e "$SHIP_HOME"/oot-mq.otr ]]; then
                        ROM=GC_MQ_D
                        OTRNAME="oot-mq.otr"
                    else
                        continue
                    fi
                    ;;
                *)
                    echo -e "\n$romfile - $ROMHASH rom hash does not match\n"
                    continue;;
                esac
                if [[ ! -e "$SHIP_HOME"/"$OTRNAME" ]]; then
                    cp -r "$ASSETDIR"/assets/game "$ASSETDIR"/Extract/assets
                    if [ -n "$ZENITY" ]; then
                        (echo "# 25%"; echo "25"; sleep 2; echo "# 50%"; echo "50"; sleep 3; echo "# 75%"; echo "75"; sleep 2; echo "# 100%"; echo "100"; sleep 3) |
                        zenity --progress --title="OTR Generating..." --timeout=10 --percentage=0 --icon-name=soh --window-icon=soh.png --height=80 --width=400 &
                    else
                        echo "Processing..."
                    fi
                    assets/extractor/ZAPD.out ed -eh -i assets/extractor/xmls/"${ROM}" -b tmp/rom.z64 -fl assets/extractor/filelists -o placeholder -osf placeholder -gsf 1 -rconf assets/extractor/Config_"${ROM}".xml -se OTR --otrfile "${OTRNAME}" > /dev/null 2>&1
                    cp "$ASSETDIR"/"$OTRNAME" "$SHIP_HOME"
                fi
            else
                if [[ (! -e "$SHIP_HOME"/oot.otr) && (! -e "$SHIP_HOME"/oot-mq.otr) ]]; then
                    if [ -n "$ZENITY" ]; then
                        zenity --error --timeout=5 --text="Place ROM in $SHIP_HOME" --title="Missing ROM file" --width=500 --width=200
                    else
                        echo -e "\nPlace ROM in this folder\n"
                    fi	
                    exit
                fi
            fi
        done
        if [[ (! -e "$SHIP_HOME"/oot.otr) && (! -e "$SHIP_HOME"/oot-mq.otr) ]]; then
            if [ -n "$ZENITY" ]; then
                zenity --error --timeout=10 --text="No valid ROMs were provided, No OTR was generated." --title="Incorrect ROM file" --width=500 --width=200
            else
                echo "No valid roms provided, no OTR was generated."
            fi
            rm -r "$ASSETDIR"
            exit
        else
            (cd "$SHIP_BIN_DIR"; ./soh.elf)
            exit
        fi
        rm -r "$ASSETDIR"
done	
	(cd "$SHIP_BIN_DIR"; ./soh.elf)
exit
