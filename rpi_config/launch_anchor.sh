#!/bin/bash
SPECTRE_DIR=/home/pi/spectre_battlezone
ANCHOR_PROGRAM=$SPECTRE_DIR/dwm1000/2-init

# Ensure recent config files are stored correctly

if [ -f $ANCHOR_PROGRAM ]
then
	#Program does not exist. Re-run 'make'
	cd $SPECTRE_DIR/2-init 2 && make && cd ~
fi

# Launch the raw-status background program in hidden thread
$ANCHOR_PROGRAM 1

# Launch the GUI program
# startx 2 &
