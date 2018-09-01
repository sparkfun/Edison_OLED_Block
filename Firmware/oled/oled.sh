#!/bin/bash

function oled-menu () {
	local SELECTION
	local SCREEN_BUF
	local OPTIONS
	local CHOICE
	local FROM
	local MAXCHOICE
	local index
	local icount
	local button
	
	SELECTION="$1"
	SCREEN_BUF="$2"
	shift 2
	
	if [[ $# -eq 0 ]]; then
		return
	fi
	
	MAXCHOICE=0
	OPTIONS=()
	while [[ $# -gt 0 ]]; do
		OPTIONS+=("$1")
		shift
		[[ $MAXCHOICE -lt 6 ]] && MAXCHOICE=$(( $MAXCHOICE + 1 ))
	done

	FROM=0
	CHOICE=0
	while true; do
		icount=0
		oled clear save "${SCREEN_BUF}"
		for o in ${OPTIONS[@]:$FROM}; do
			oled load "${SCREEN_BUF}" setCursor 6 $(( $icount * 8 )) print "$o" save "${SCREEN_BUF}"
			icount=$(($icount + 1))
			[[ $icount -eq 6 ]] && break
		done
		oled load "${SCREEN_BUF}" \
			line 0 $(( $CHOICE * 8 + 1 )) 5 $(( $CHOICE * 8 + 3 )) \
			line 0 $(( $CHOICE * 8 + 1 )) 0 $(( $CHOICE * 8 + 5 )) \
			line 0 $(( $CHOICE * 8 + 5 )) 5 $(( $CHOICE * 8 + 3 )) \
			display
		for button in $(oled wait buttons); do
			case $button in
				U)
					CHOICE=$(($CHOICE - 1))
					[[ $CHOICE -eq -1 ]] && [[ $FROM -gt 0 ]] && FROM=$(( $FROM - 1 ))
					[[ $CHOICE -eq -1 ]] && CHOICE=$(( $CHOICE + 1 ))
					;;
				D)
					CHOICE=$(($CHOICE + 1))
					[[ $CHOICE -eq $MAXCHOICE ]] && [[ $FROM -lt $(( ${#OPTIONS[@]} - 6)) ]] && FROM=$(( FROM + 1 ))
					[[ $CHOICE -eq $MAXCHOICE ]] && CHOICE=$(( $CHOICE - 1 ))
					;;
				B)
					rm "${SCREEN_BUF}"
					oled clear display
					return
					;;
				A)
					$SELECTION $(( $CHOICE + $FROM ))
					;;
			esac
		done
	done
}
