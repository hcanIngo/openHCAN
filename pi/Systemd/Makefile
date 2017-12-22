# Systemd
CONF = /home/ingo/Dropbox/HCAN/HI_SERVER/openhab2_conf_mqtt
include $(CONF)/myconf.inc	
ZIEL_PC = bananapi
ZIEL_USER = tt

SERVICE = hcan

_SCP = sudo sshpass -p $(PT) scp -r
_SSH = sudo sshpass -p $(PT) ssh -t

SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)

all:
	make 1_installMosq
	make 2_canDToverlay
	make testCan
	make 3_ipLinkSet_CAN__Persistent
	make pwMosq_1
	make pwMosq_2
	make autostartHcanDienste
	make 5_audio
	@# openhab installieren

1_installMosq: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo apt-get install mosquitto

2_canDToverlay:
	$(_SCP) ./armbianEnv.txt $(ZIEL_USER)@$(ZIEL_PC):/boot/armbianEnv.txt
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo reboot
	@# https://docs.armbian.com/User-Guide_Allwinner_overlays/ 
	@# /boot/dtb/overlay/README.sun7i-a20-overlays,  /boot/dtb/overlay/sun7i-a20-can.dtbo
	@# 1. overlays=can   in  /boot/armbianEnv.txt  einfuegen
	@# 2. reboot
	@# 3. make testCan
	
3_ipLinkSet_CAN__Persistent:
	$(_SCP) ./rc.local $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv /home/$(ZIEL_USER)/rc.local /etc/rc.local
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo chmod u=rxs,g=x,o= /etc/rc.local
	@#$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl enable rc.local

4_all_hcan:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo apt-get update
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo apt-get install git pkg-config indent xalan g++ libboost-dev libboost-program-options-dev libboost-regex-dev libreadline-gplv2-dev libxml++2.6-dev clang
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) git clone https://github.com/hcanIngo/openHCAN.git
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) cd openHCAN; sudo make strukturen xx="sudo make clean"; make cDienste xx="sudo make clean"; make cppDienste xx="sudo make clean"
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) cd openHCAN; sudo make strukturen xx="make all"; make cDienste xx="make all"; make cppDienste xx="make all"	
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) cd openHCAN; sudo make strukturen xx="make install"; make cDienste xx="make install"; make cppDienste xx="make install"
	make autostart
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo apt-get update	
	
5_audio:
	@# https://forum.armbian.com/index.php?/topic/1960-guide-configuring-orange-pi-pc-for-analog-line-out-jack-audio-output-and-simultaneous-hdmi-output-with-software-mixing/
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo sudo apt-get install mplayer

pwMosq_1: 
	# siehe:  http://www.steves-internet-guide.com/mqtt-username-password-example/
	# Pw im pi-tt-home erzeugen: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mosquitto_passwd -c pwmosq.txt hcan
	# Pw-Datei auf dem pi verschieben:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv /home/$(ZIEL_USER)/pwmosq.txt /etc/mosquitto/pwmosq.txt
pwMosq_2: 
	# Eintrag einfuegen, sodass die pw-Datei verwendet wird: 
	$(_SCP) ./mosquitto_pw.conf $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv /home/$(ZIEL_USER)/mosquitto_pw.conf /etc/mosquitto/conf.d/
	# Kontrolle
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /etc/mosquitto/conf.d/mosquitto_pw.conf
	
###
testCan:
	@echo -- sun4i_can meldet sich? --
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo modinfo sun4i_can
	@echo ----------------------------
	@echo -- can0 aufgelistet?  - falls nicht: Device-Tree-patchen --
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) ip link

testMosq: 
	@echo -- laeuft der mosquitto-MQTT-Broker? --
	@#$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) ps -A | grep mosq
	make status SERVICE=mosquitto
	@echo ----------------------------

showMosqLog: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) nano /var/log/mosquitto/mosquitto.log
	
showOHLog: 
	@#$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less ~/myOpenHabInstallation/openhab.log
	@#$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /var/log/openhab2/openhab.log
	config:edit org.openhab.mqtt
	config:property-list
	config:delete org.openhab.mqtt

testHcanDienste: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) ps -A | grep hcan

showKernelVersion:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) uname -r

autostartHcanDienste:
	@echo -- Systemd-Autostart --
	$(_SCP) ./startHcanDienste.sh $(SCP_ZIEL_DIR)
	$(_SCP) ./$(SERVICE).service $(SCP_ZIEL_DIR)
	#
	# make stop
	#
	# Auf dem Bananapi verschieben + Anpassung der Rechte
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo chmod a+x /home/$(ZIEL_USER)/startHcanDienste.sh
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(SERVICE).service /etc/systemd/system/$(SERVICE).service	
	make start
	#
	# Symbolischen Link fuer Autostart beim Booten: 
	@# https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl enable $(SERVICE)
	@# Ergebnis:  Created symlink /etc/systemd/system/multi-user.target.wants/hcan.service → /etc/systemd/system/hcan.service

start: 
	make stop
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl start $(SERVICE)

stop: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl stop $(SERVICE)

status:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) systemctl status $(SERVICE)