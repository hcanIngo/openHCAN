# bananapi
include ../Makefile

ZIEL_OS = Bananian_1504
OPENHAB_SERVICE = openhab

all:
	cd ../localhost/; make unzip_openhab_installation
	cd ../localhost/; make mv to=$(SCP_ZIEL_DIR)
	#
	make openhab_start_withScreen
	# make install_oracle_arm_jre
	# make openhab_autostartService
	# ggf. make myopenhab


install_oracle_arm_jre: 
	cd $(OPENHABDOWNLOADS); tar xfvz $(OPENHABDOWNLOADS)/jdk-8u33-linux-arm-vfp-hflt.tar.gz
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/jdk1.8.0
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/myOpenHabInstallation
	$(_SCP) $(OPENHABDOWNLOADS)/jdk1.8.0_33/* $(SCP_ZIEL_DIR)/../jdk1.8.0
	# Falls noetig, auf Bananapi: sudo chmod 755 /home/tt/jdk1.8.0/*

openhab_start_withScreen:
	@# Damit wird das richtige JDK verwendet und die Ausgaben von Screen gemanaged werden: 
	@# screen -S 'OpenHAB Server' -dm /home/bananapi/jdk1.8.0/bin/java  in der start.sh (./Bananian_1504/start.sh) voranstellen
	$(_SCP) ./$(ZIEL_OS)/start.sh $(SCP_ZIEL_DIR)/start.sh
	$(_SCP) ./$(ZIEL_OS)/start_debug.sh $(SCP_ZIEL_DIR)/start_debug.sh

myopenhab:
	$(_SCP) $(OPENHABDOWNLOADS)/org.openhab.io.myopenhab-1.7.0.jar $(SCP_ZIEL_DIR)/addons/org.openhab.io.myopenhab.jar
	@echo "\nuuid:" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/uuid
	@echo "\nsecret:" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/secret
	@echo "\n\n   @ https://my.openhab.org/  wird der Inhalt von  webapps/static/uuid  und webapps/static/secret    eingetragen! "
	@echo "\n Die beiden Dateien werden erst nach sh ./myOpenHabInstallation/start.sh -Aufruf angelegt (ssh bananapi@tt).\n\n"  

openhab_autostartService:
	$(_SCP) ./$(ZIEL_OS)/$(OPENHAB_SERVICE) $(SCP_ZIEL_DIR)/../$(OPENHAB_SERVICE)
	#
	make stop
	#
	# Auf dem Bananapi verschieben
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(OPENHAB_SERVICE) /etc/init.d/$(OPENHAB_SERVICE)
	#
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo chmod a+x /etc/init.d/$(OPENHAB_SERVICE)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo update-rc.d $(OPENHAB_SERVICE) defaults
	
########################	
reboot:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo reboot

start: 
	make stop
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) start

stop: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) stop

state:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) status
