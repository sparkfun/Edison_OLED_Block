#!/bin/bash

function oled-menu () {
	local SELECTION
	local SCREEN_BUF
	local OPTIONS
	local CHOICE
	local index
	local button
	
	SELECTION="$1"
	SCREEN_BUF="$2"
	shift 2
	oled clear save "${SCREEN_BUF}"
	
	if [[ $# -eq 0 ]]; then
		return
	fi
	
	OPTIONS=()
	while [[ $# -gt 0 ]]; do
		OPTIONS+=("$1")
		shift
	done
	for index in ${!OPTIONS[@]}; do
		oled load "${SCREEN_BUF}" setCursor 6 $(( $index * 8 )) print "${OPTIONS[$index]}" save "${SCREEN_BUF}"
	done
	
	CHOICE=0
	while true; do
		oled load "${SCREEN_BUF}" \
			line 0 $(( $CHOICE * 8 + 1 )) 5 $(( $CHOICE * 8 + 3 )) \
			line 0 $(( $CHOICE * 8 + 1 )) 0 $(( $CHOICE * 8 + 5 )) \
			line 0 $(( $CHOICE * 8 + 5 )) 5 $(( $CHOICE * 8 + 3 )) \
			display
		for button in $(oled wait buttons); do
			case $button in
				U)
					CHOICE=$(($CHOICE - 1))
					[[ $CHOICE -eq -1 ]] && CHOICE=$(( $CHOICE + 1 ))
					;;
				D)
					CHOICE=$(($CHOICE + 1))
					[[ $CHOICE -eq ${#OPTIONS[@]} ]] && CHOICE=$(( $CHOICE - 1 ))
					;;
				L)
					rm "${SCREEN_BUF}"
					oled clear display
					return
					;;
				[RSAB])
					$SELECTION $button "${OPTIONS[$CHOICE]}"
					;;
			esac
		done
	done
}