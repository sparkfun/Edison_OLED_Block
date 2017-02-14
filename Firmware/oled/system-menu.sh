#!/bin/bash

cd /usr/local/sbin

. ./oled.sh

oled begin

MENU_ITEMS=()
MENU_FUNCTIONS=()

for F in /usr/local/sbin/system-menu.d/*.sh; do
	. "$F"
done

function menuf () {
	${MENU_FUNCTIONS[$1]}
}

while true; do
	oled-menu "menuf" /tmp/system.menu.tmp "${MENU_ITEMS[@]}"
	while [[ -n $(oled buttons) ]]; do
		oled wait
	done
	while [[ -z $(oled buttons) ]]; do
		oled wait
	done
done
	