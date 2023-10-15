#!/bin/bash

SNAME="$(dirname $0)"
export SHIP_HOME="$HOME/Library/Application Support/com.shipofharkinian.soh"
export SNAME
export RESPATH="${SNAME%/MacOS*}/Resources"
export LIBPATH="${SNAME%/MacOS*}/Frameworks"
export DYLD_FALLBACK_LIBRARY_PATH="$LIBPATH"

remap_hashes ()
{
	# Remap v64 and n64 hashes to their z64 hash equivalent
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
	d0bdc2eb320668b4ba6893b9aefe4040a73123ff) # v64
		ROMHASH=328a1f1beba30ce5e178f031662019eb32c5f3b5
		;;
	4946ab250f6ac9b32d76b21f309ebb8ebc8103d2) # n64
		ROMHASH=328a1f1beba30ce5e178f031662019eb32c5f3b5
		;;
	663c34f1b2c05a09e5beffe4d0dcd440f7d49dc7) # v64
		ROMHASH=cfbb98d392e4a9d39da8285d10cbef3974c2f012
		;;
	24c73d378b0620a380ce5ef9f2b186c6c157a68b) # n64
		ROMHASH=cfbb98d392e4a9d39da8285d10cbef3974c2f012
		;;
	8ebf2e29313f44f2d49e5b4191971d09919e8e48) # v64
		ROMHASH=f46239439f59a2a594ef83cf68ef65043b1bffe2
		;;
	4264bf7b875737b8fae77d52322a5099d051fc11) # n64
		ROMHASH=f46239439f59a2a594ef83cf68ef65043b1bffe2
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
}

if [ ! -e "$SHIP_HOME" ]; then	mkdir "$SHIP_HOME"; fi

if [ ! -e "$SHIP_HOME"/mods ]; then
	mkdir -p "$SHIP_HOME"/mods
	touch "$SHIP_HOME"/mods/custom_otr_files_go_here.txt
fi

# If either OTR doesn't exist kick off the OTR gen process
if [ ! -e "$SHIP_HOME"/oot.otr ] || [ ! -e "$SHIP_HOME"/oot-mq.otr ]; then

	# If no ROMs exist kick off the file selection prompts
	while [ ! -e "$SHIP_HOME"/*.*64 ] && [ ! -e "$SHIP_HOME"/oot*.otr ]; do

		SHOULD_PROMPT_FOR_ROM=1
		while [ $SHOULD_PROMPT_FOR_ROM -eq 1 ]; do
			SHOULD_PROMPT_FOR_ROM=0
			# Use osascript to prompt the user to chose a file
			DROPROM=`osascript <<-EOF
				set romFile to choose file of type {"b64","n64","v64","z64"} with prompt "Please select your ROM:"
				return POSIX path of romFile
			EOF`

			# If no rom was selected, the user cancelled, so exit
			if [[ -z $DROPROM ]] && [[ -z "$UPLOAD_ANOTHER_RESULT" ]]; then
				echo "No ROM selected. Exiting..."
				exit 1
			elif [[ -z $DROPROM ]]; then
				break;
			fi

			# If an invalid rom was selected, let the user know and ask to try again
			ROMHASH="$(shasum "$DROPROM" | awk '{ print $1 }')"

			remap_hashes

			case "$ROMHASH" in
				cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
					ROM_TYPE=0;;
				0227d7c0074f2d0ac935631990da8ec5914597b4)
					ROM_TYPE=0;;
				328a1f1beba30ce5e178f031662019eb32c5f3b5)
					ROM_TYPE=0;;
				cfbb98d392e4a9d39da8285d10cbef3974c2f012)
					ROM_TYPE=0;;
				f46239439f59a2a594ef83cf68ef65043b1bffe2)
					ROM_TYPE=1;;
				50bebedad9e0f10746a52b07239e47fa6c284d03)
					ROM_TYPE=1;;
				079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
					ROM_TYPE=1;;
				cfecfdc58d650e71a200c81f033de4e6d617a9f6)
					ROM_TYPE=1;;
				517bd9714c73cb96c21e7c2ef640d7b55186102f)
					ROM_TYPE=1;;
				*)
					TRY_AGAIN_RESULT=`osascript <<-EOF
						set alertText to "Incompatible ROM hash"
						set alertMessage to "Incompatible ROM provided, would you like to try again?"
						return display alert alertText \
							message alertMessage \
							as critical \
							buttons {"Cancel", "Try Again"}
					EOF`
					if [[ "$TRY_AGAIN_RESULT" == "button returned:Try Again" ]]; then
						SHOULD_PROMPT_FOR_ROM=1
						continue;
					else
						echo "No ROM selected. Exiting..."
						exit 1
					fi
			esac

			cp "$DROPROM" "$SHIP_HOME"

			# Ask user if they would also like to select the other variant (MQ/Vanilla)
			if [ $ROM_TYPE -eq 0 ] && [[ -z "$UPLOAD_ANOTHER_RESULT" ]]; then
				UPLOAD_ANOTHER_RESULT=`osascript <<-EOF
					set alertText to "Success"
					set alertMessage to "Would you also like to provide a Master Quest ROM?"
					return display alert alertText \
						message alertMessage \
						buttons {"No", "Yes"}
				EOF`
			elif [[ -z "$UPLOAD_ANOTHER_RESULT" ]]; then
				UPLOAD_ANOTHER_RESULT=`osascript <<-EOF
					set alertText to "Success"
					set alertMessage to "Would you also like to provide a Vanilla (Non Master Quest) ROM?"
					return display alert alertText \
						message alertMessage \
						buttons {"No", "Yes"}
				EOF`
			fi

			if [[ "$UPLOAD_ANOTHER_RESULT" == "button returned:Yes" ]]; then
				UPLOAD_ANOTHER_RESULT="button returned:No"
				SHOULD_PROMPT_FOR_ROM=1
				continue;
			fi
			break
		done
	done

	# At this point we should now have 1 or more valid roms in $SHIP_HOME directory

	# Prepare tmp dir
	for ROMPATH in "$SHIP_HOME"/*.*64
	do
		ASSETDIR="$(mktemp -d /tmp/assets-XXXXX)"
		export ASSETDIR
		cp -r "$RESPATH/assets" "$ASSETDIR"
		mkdir -p "$ASSETDIR"/tmp
		mkdir -p "$ASSETDIR"/Extract
		cp "$ROMPATH" "$ASSETDIR"/tmp/rom.z64
		cp -r "$ASSETDIR"/assets/game "$ASSETDIR"/Extract/assets/
		cd "$ASSETDIR" || return

		# If an invalid rom was detected, let the user know
		ROMHASH="$(shasum "$ASSETDIR"/tmp/rom.z64 | awk '{ print $1 }')"

		remap_hashes

		case "$ROMHASH" in
			cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
				ROM=GC_NMQ_D
				OTRNAME="oot.otr";;
			0227d7c0074f2d0ac935631990da8ec5914597b4)
				ROM=GC_NMQ_PAL_F
				OTRNAME="oot.otr";;
			328a1f1beba30ce5e178f031662019eb32c5f3b5)
				ROM=N64_PAL_10
				OTRNAME="oot.otr";;
			cfbb98d392e4a9d39da8285d10cbef3974c2f012)
				ROM=N64_PAL_11
				OTRNAME="oot.otr";;
			f46239439f59a2a594ef83cf68ef65043b1bffe2)
				ROM=GC_MQ_PAL_F
				OTRNAME="oot-mq.otr";;
			50bebedad9e0f10746a52b07239e47fa6c284d03)
				ROM=GC_MQ_D
				OTRNAME="oot-mq.otr";;
			079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
				ROM=GC_MQ_D
				OTRNAME="oot-mq.otr";;
			cfecfdc58d650e71a200c81f033de4e6d617a9f6)
				ROM=GC_MQ_D
				OTRNAME="oot-mq.otr";;
			517bd9714c73cb96c21e7c2ef640d7b55186102f)
				ROM=GC_MQ_D
				OTRNAME="oot-mq.otr";;
			*)
				osascript -e 'display notification "One or more invalid ROM provided" with title "Ship Of Harkinian"'
				rm -r "$ASSETDIR"
				continue;
		esac

		# Only generate OTR if we don't have on of this type yet
		if [ -e "$SHIP_HOME"/"$OTRNAME" ]; then
			rm -r "$ASSETDIR"
			continue;
		fi

		osascript -e 'display notification "Generating OTR..." with title "Ship Of Harkinian"'
		assets/extractor/ZAPD.out ed -i assets/extractor/xmls/"${ROM}" -b tmp/rom.z64 -fl assets/extractor/filelists -o placeholder -osf placeholder -gsf 1 -rconf assets/extractor/Config_"${ROM}".xml -se OTR
		if [ -e "$ASSETDIR"/oot.otr ]; then
			osascript -e 'display notification "OTR successfully generated" with title "Ship Of Harkinian"'
			cp "$ASSETDIR"/oot.otr "$SHIP_HOME"/"$OTRNAME"
			rm -r "$ASSETDIR"
		fi
	done

	if [ ! -e "$SHIP_HOME"/oot*.otr ]; then
		osascript -e 'display notification "OTR failed to generate" with title "Ship Of Harkinian"'
		exit 1;
	fi
fi

arch_name="$(uname -m)"
launch_arch="arm64"
if [ "${arch_name}" = "x86_64" ] && [ "$(sysctl -in sysctl.proc_translated)" != "1" ]; then
	launch_arch="x86_64"
fi

arch -${launch_arch} "$RESPATH"/soh-macos
exit
