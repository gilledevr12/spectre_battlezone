#!/bin/bash
echo "What is the Tag ID of this device?"
read TAG_ID

#Replace the tag ID in the .bashrc for laser-brains method launch
sed -i "s/laser-brains [0-9]/laser-brains $TAG_ID/" example
