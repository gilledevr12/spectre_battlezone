#!/bin/bash

# Get tag num from user
echo "What is the Tag ID of this device?"
read TAG_ID

# Ensure recent config files are stored
cp tag_xinitrc 	    /home/pi/.xinitrc       # Startx by default
cp tag_xserverrc 	/home/pi/.xserverrc     # to disable the cursor on the browser
cp tag_bashrc 	    /home/pi/.bashrc        # Custom xinitrc for connecting to gameserver
cp launch_tag.sh 	/home/pi/.              # the game executable

# Replace the tag ID in the .bashrc for laser-brains method launch
sed -i "s/$LASER_PROGRAM &/$LASER_PROGRAM $TAG_ID&/" /home/pi/launch_tag.sh

# Modify the default x11 xorg.conf to disable screen timeout
echo "Run the command ** sudo cp tag_xorg.conf /etc/X11/xorg.conf ** to disable screen timeout"
#sudo cp tag_xorg.conf /etc/X11/xorg.conf -> we dont have sudo permissions in this script

# Modify the contents of the kernel image to force resolution out
echo 'Run the comand ** sudo sed -i "s/&disable_overscan&/disable_overscan=1/" /boot/config.txt ** to force resolution out'
#sudo sed -i "s/&disable_overscan&/disable_overscan=1/" /boot/config.txt -> we dont have sudo permissions in this script