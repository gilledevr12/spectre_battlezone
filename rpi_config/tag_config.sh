#!/bin/bash

# Get tag num from user
echo "What is the Tag ID of this device?"
read TAG_ID

# Ensure recent config files are stored
cp xinitrc_master 	/home/pi/.xinitrc
cp bashrc_master 	/home/pi/.bashrc
cp launch_spectre.sh 	/home/pi/.

# Replace the tag ID in the .bashrc for laser-brains method launch
sed -i "s/$LASER_PROGRAM &/$LASER_PROGRAM $TAG_ID&/" /home/pi/launch_spectre.sh

#Endure the displays will not timeout or powerdown
sed -i "s/BLANK_TIME.*/BLANK_TIME=0/" /etc/kdb/config
sed -i "s/POWERDOWN_TIME.*/POWERDOWN_TIME=0/" /etc/kdb/config
/etc/init.d/kdb restart
