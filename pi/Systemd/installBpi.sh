#!/bin/bash

# echo "install for hcan on Bpi"

# Auf folgende Dateien greift dieses Script zu (daher muss openHCAN auf dem bpi vorliegen):
# - /home/tt/openHCAN/pi/Systemd/sun7i-a20-can.dts
# - /home/tt/openHCAN/pi/Systemd/rc.local
# - /home/tt/openHCAN/pi/Systemd/mosquitto_pw.conf
# - /home/tt/openHCAN/pi/Systemd/startHcanDienste.sh
# - /home/tt/openHCAN/pi/Systemd/hcan.service

installMosq() {
	sudo apt-get install mosquitto
}

canDToverlay() {
	#ZIEL_USER=$1
	#sudo echo "overlays=can" >> /boot/armbianEnv.txt  # https://docs.armbian.com/User-Guide_Allwinner_overlays/ 
	cd /home/$ZIEL_USER/openHCAN/pi/Systemd/           # sun7i-a20-can.dts verwenden da das von Armbian nicht geht
	sudo armbian-add-overlay sun7i-a20-can.dts
	
	echo "Reboot erforderlich! (Dieses Sktipt beenden und den Neustart manuell anstossen)"
	#sudo reboot
}
	
ipLinkSet_CAN__Persistent() {
	ZIEL_USER=$1
	sudo cp /home/$ZIEL_USER/openHCAN/pi/Systemd/rc.local /home/$ZIEL_USER
	sudo mv /home/$ZIEL_USER/rc.local /etc/rc.local
	sudo chmod u=rxs,g=x,o= /etc/rc.local
	#sudo systemctl enable rc.local
}

4_all_hcan() {
	sudo apt-get update
	sudo apt-get install git pkg-config indent xalan g++ libboost-dev libboost-program-options-dev libboost-regex-dev libreadline-gplv2-dev libxml++2.6-dev clang
	git clone https://github.com/hcanIngo/openHCAN.git
	cd openHCAN; sudo make strukturen xx="sudo make clean"; make cDienste xx="sudo make clean"; make cppDienste xx="sudo make clean"
	cd openHCAN; sudo make strukturen xx="make all"; make cDienste xx="make all"; make cppDienste xx="make all"	
	cd openHCAN; sudo make strukturen xx="make install"; make cDienste xx="make install"; make cppDienste xx="make install"
	autostartHcanDienste tt hcan
	sudo apt-get update
}

audio() {
	# audio: https://forum.armbian.com/index.php?/topic/1960-guide-configuring-orange-pi-pc-for-analog-line-out-jack-audio-output-and-simultaneous-hdmi-output-with-software-mixing/
	sudo apt-get install mplayer
}

pwMosq_1() {
	ZIEL_USER=$1
	# siehe:  http://www.steves-internet-guide.com/mqtt-username-password-example/
	# Pw im pi-tt-home erzeugen: 
	mosquitto_passwd -c /home/$ZIEL_USER/pwmosq.txt hcan
	# Pw-Datei auf dem pi verschieben:
	sudo mv /home/$ZIEL_USER/pwmosq.txt /etc/mosquitto/pwmosq.txt
}

pwMosq_2() {
	ZIEL_USER=$1
	# Eintrag einfuegen, sodass die pw-Datei verwendet wird: 
	sudo cp /home/$ZIEL_USER/openHCAN/pi/Systemd/mosquitto_pw.conf /home/$ZIEL_USER/
	sudo mv /home/$ZIEL_USER/mosquitto_pw.conf /etc/mosquitto/conf.d/
	# Kontrolle
	less /etc/mosquitto/conf.d/mosquitto_pw.conf
}

testCan() {
	echo "-- sun4i_can meldet sich? --"
	sudo modinfo sun4i_can
	echo "----------------------------"
	echo "-- can0 aufgelistet?  - falls nicht: Device-Tree-patchen --"
	ip link
}

testMosq() {
	echo "-- laeuft der mosquitto-MQTT-Broker? --"
	#ps -A | grep mosq
	make status SERVICE=mosquitto
	echo "----------------------------"
}

showMosqLog() {
	nano /var/log/mosquitto/mosquitto.log
}
	
showOHLog() {
	#less ~/myOpenHabInstallation/openhab.log
	#less /var/log/openhab2/openhab.log
	config:edit org.openhab.mqtt
	config:property-list
	config:delete org.openhab.mqtt
}

testHcanDienste() {
	ps -A | grep hcan
}

showKernelVersion() {
	uname -r
}

autostartHcanDienste() {
	ZIEL_USER=$1
	SERVICE=$2 # hcan
	echo "-- Systemd-Autostart --"
	sudo cp /home/$ZIEL_USER/openHCAN/pi/Systemd/startHcanDienste.sh /home/$ZIEL_USER
	sudo cp /home/$ZIEL_USER/openHCAN/pi/Systemd/$SERVICE.service /home/$ZIEL_USER
	#
	# stop
	#
	# Auf dem Bananapi verschieben + Anpassung der Rechte
	sudo chmod a+x /home/$ZIEL_USER/startHcanDienste.sh
	sudo mv ./$SERVICE.service /etc/systemd/system/$SERVICE.service	
	start hcan
	#
	# Symbolischen Link fuer Autostart beim Booten: 
	# https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units
	sudo systemctl enable hcan
	# Ergebnis:  Created symlink /etc/systemd/system/multi-user.target.wants/hcan.service → /etc/systemd/system/hcan.service
}

start() {
	SERVICE=$1 # hcan
	stop $SERVICE
	sudo systemctl start $SERVICE
}

stop() {
	SERVICE=$1 # hcan
	sudo systemctl stop $SERVICE
}

status() {
	SERVICE=$1 # hcan
	systemctl status $SERVICE
}

#######################################################################

Schleife() { 
	echo "  "
	echo "was nun? "
	while ((answer != 1))
	  do
		read -p "(1) beendet  oder  weiter mit beliebiger Taste" answer
		case "$answer" in
		  1) echo "Dieses Sktipt beenden und den Neustart manuell anstossen."
			 # sudo reboot # eine Beschaedigung des root-Dateisystems vermeiden
			 ;;
		  *) clear
			 Auswahl
			 ;;
		esac
	done
	
	clear 
	exit 255
}


Auswahl() {
	#dialog --backtitle HCAN-Installation --title Lobbox --msgbox "HCAN-Installation/-Vorbereitung auf dem Bananapi" 15 40

	auswahl=$(dialog --stdout --backtitle HCAN-Installation --title Details --radiolist "Aktion waehlen: " 16 60 5 \
		 "alles" "alles installieren" on \
		 "installMosq" "mosquitto installieren" off \
		 "canDToverlay" "Bpi-Overlay fuer CAN" off \
		 "testCan" "CAN pruefen" off \
		 "ipLinkSet_CAN__Persistent" "Bpi-Overlay fuer CAN" off \
		 "pwMosq_1" "mosquitto Conf" off \
		 "pwMosq_2" "mosquitto Conf" off \
		 "autostartHcanDienste" "" off
		 )

	case "$auswahl" in
	  alles)
		clear
		installMosq
		canDToverlay
		ipLinkSet_CAN__Persistent tt
		pwMosq_1 tt
		pwMosq_2 tt
		audio
		autostartHcanDienste tt hcan
		echo "Reboot erforderlich. Bitte manuell anstossen."
		;;
	  installMosq)
		clear
		installMosq
		;;
	  canDToverlay)
		clear
		canDToverlay tt
		;;
	  testCan)
		clear
		testCan
		;;
	  ipLinkSet_CAN__Persistent)
		clear
		ipLinkSet_CAN__Persistent tt
		;;
	  pwMosq_1)
		clear
		pwMosq_1 tt
		;;
	  pwMosq_2)
		clear
		pwMosq_2 tt
		;;
	  autostartHcanDienste)
		clear
		autostartHcanDienste tt hcan
		;;
	  *) 
	    clear
		Schleife
		;;

	# openhab installieren
	esac
	
	Schleife
}


Auswahl

# read -p "[Enter] zum Beenden..."
# clear
# exit 255
#######################################################################