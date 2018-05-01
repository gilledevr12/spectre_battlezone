#!/bin/bash

# Get tag num from user
echo "What is the Anchor ID of this device?"
read ANC_ID

#Move the anchor executable to home dir
cp anchor_bashrc 	/home/pi/.              # the anchor executable
cp launch_anchor.sh 	/home/pi/.              # the anchor executable

# Replace the anchor ID in the .bashrc for laser-brains method launch
# sed -i "s/$ANCHOR_PROGRAM &/$ANCHOR_PROGRAM $ANC_ID&/" /home/pi/launch_anchor.sh
