#!/bin/bash
if [ $EUID != 0 ]
then
	echo "This must be ran with root privileges"
	exit
fi

echo "Which network would you like to connect to?"
echo "1: Bluezone"
echo "2: spectre network"
read NETWORK
if [ $NETWORK -eq 1 ]
then
	sed -i 's/wpa-ssid .*/wpa-ssid "BLUEZONE"/' /etc/network/interfaces
else
	sed -i 's/wpa-ssid .*/wpa-ssid "spectre"/' /etc/network/interfaces
fi

#reboot to change networks 
sudo reboot

