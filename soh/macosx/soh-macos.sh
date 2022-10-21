#!/bin/bash

SNAME="$(dirname $0)"
export DATA_SHARE="$HOME/Library/Application Support/com.shipofharkinian.soh"
export SNAME
export RESPATH="${SNAME%/MacOS*}/Resources"
export LIBPATH="${SNAME%/MacOS*}/Frameworks"
export DYLD_FALLBACK_LIBRARY_PATH="$LIBPATH"

if [ ! -e "$DATA_SHARE" ]; then	mkdir "$DATA_SHARE"; fi

# If either OTR doesn't exist kick off the OTR gen process
if [ ! -e "$DATA_SHARE"/oot.otr ] || [ ! -e "$DATA_SHARE"/oot-mq.otr ]; then

	# If no ROMs exist kick off the file selection prompts
	while [ ! -e "$DATA_SHARE"/*.*64 ] && [ ! -e "$DATA_SHARE"/oot*.otr ]; do

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
			case "$ROMHASH" in
				cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
					ROM_TYPE=0;;
				0227d7c0074f2d0ac935631990da8ec5914597b4)
					ROM_TYPE=0;;
				50bebedad9e0f10746a52b07239e47fa6c284d03)
					ROM_TYPE=1;;
				079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
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

			cp "$DROPROM" "$DATA_SHARE"

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

	# At this point we should now have 1 or more valid roms in $DATA_SHARE directory

	# Prepare tmp dir
	for ROMPATH in "$DATA_SHARE"/*.*64
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
		case "$ROMHASH" in
			cee6bc3c2a634b41728f2af8da54d9bf8cc14099)
				ROM=GC_NMQ_D
				OTRNAME="oot.otr";;
			0227d7c0074f2d0ac935631990da8ec5914597b4)
				ROM=GC_NMQ_PAL_F
				OTRNAME="oot.otr";;
			50bebedad9e0f10746a52b07239e47fa6c284d03)
				ROM=GC_MQ_D
				OTRNAME="oot-mq.otr";;
			079b855b943d6ad8bd1eb026c0ed169ecbdac7da)
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
		if [ -e "$DATA_SHARE"/"$OTRNAME" ]; then
			rm -r "$ASSETDIR"
			continue;
		fi

		osascript -e 'display notification "Generating OTR..." with title "Ship Of Harkinian"'
		assets/extractor/ZAPD.out ed -i assets/extractor/xmls/"${ROM}" -b tmp/rom.z64 -fl assets/extractor/filelists -o placeholder -osf placeholder -gsf 1 -rconf assets/extractor/Config_"${ROM}".xml -se OTR
		if [ -e "$ASSETDIR"/oot.otr ]; then
			osascript -e 'display notification "OTR successfully generated" with title "Ship Of Harkinian"'
			cp "$ASSETDIR"/oot.otr "$DATA_SHARE"/"$OTRNAME"
			rm -r "$ASSETDIR"
		fi
	done

	if [ ! -e "$DATA_SHARE"/oot*.otr ]; then
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
