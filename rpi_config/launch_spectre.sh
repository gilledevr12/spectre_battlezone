#!/bin/bash
SPECTRE_DIR=/home/pi/spectre_battlezone
LASER_PROGRAM=$SPECTRE_DIR/laser/laser-brains

# Ensure recent config files are stored correctly


if [ -f $LASER_PROGRAM ]
then
	#Program does not exist. Re-run 'make'
	cd $SPECTRE_DIR/laser && make && cd ~
fi

# Launch the raw-status background program in hidden thread
/./$SPECTRE_DIR/$LASER_PROGRAM &

# Launch the GUI program
startx
