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

                # Remap v64 and n64 hashes to their z64 has equivalent
                # ZAPD will handle converting the data into z64 format
                case "$ROMHASH" in
                a9059b56e761c9034fbe02fe4c24985aaa835dac) # v64
                    ROMHASH=cee6bc3c2a634b41728f2af8da54d9bf8cc14099
                    ;;
                24708102dc504d3f375a37f4ae4e149c167dc515) # n64
                    ROMHASH=cee6bc3c2a634b41728f2af8da54d9bf8cc14099
                    ;;
                580dd0bd1b6d2c51cc20a764eece84dba558964c) # v64
                    ROMHASH=0227d7c0074f2d0ac935631990da8ec5914597b4
                    ;;
                d6342c59007e57c1194661ec6880b2f078403f4e) # n64
                    ROMHASH=0227d7c0074f2d0ac935631990da8ec5914597b4
                    ;;
                663c34f1b2c05a09e5beffe4d0dcd440f7d49dc7) # v64
                    ROMHASH=cfbb98d392e4a9d39da8285d10cbef3974c2f012
                    ;;
                24c73d378b0620a380ce5ef9f2b186c6c157a68b) # n64
                    ROMHASH=cfbb98d392e4a9d39da8285d10cbef3974c2f012
                    ;;
                973bc6fe56010a8d646166a1182a81b4f13b8cf9) # v64
                    ROMHASH=50bebedad9e0f10746a52b07239e47fa6c284d03
                    ;;
                d327752c46edc70ff3668b9514083dbbee08927c) # v64
                    ROMHASH=50bebedad9e0f10746a52b07239e47fa6c284d03
                    ;;
                ecdeb1747560834e079c22243febea7f6f26ba3b) # v64
                    ROMHASH=079b855b943d6ad8bd1eb026c0ed169ecbdac7da
                    ;;
                f19f8662ec7abee29484a272a6fda53e39efe0f1) # n64
                    ROMHASH=079b855b943d6ad8bd1eb026c0ed169ecbdac7da
                    ;;
                ab519ce04a33818ce2c39b3c514a751d807a494a) # v64
                    ROMHASH=cfecfdc58d650e71a200c81f033de4e6d617a9f6
                    ;;
                c19a34f7646305e1755249fca2071e178bd7cd00) # n64
                    ROMHASH=cfecfdc58d650e71a200c81f033de4e6d617a9f6
                    ;;
                25e8ae79ea0839ca5c984473f7460d8040c36f9c) # v64
                    ROMHASH=517bd9714c73cb96c21e7c2ef640d7b55186102f
                    ;;
                166c02770d67fcc3954c443eb400a6a3573d3fc0) # n64
                    ROMHASH=517bd9714c73cb96c21e7c2ef640d7b55186102f
                    ;;
                esac

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
