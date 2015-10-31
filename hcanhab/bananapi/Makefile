# bananapi (mit Bananian 1504 und 1508)
include $(HCANHAB_DIR)/MakefileDefines.inc

START_SYSTEM = Sysvinit
#START_SYSTEM = Systemd

include $(HCANHAB_DIR)/bananapi/$(START_SYSTEM)/Makefile

all:
	cd $(HCANHAB_DIR)/localhost/; make unzip_openhab_installation
	cd $(HCANHAB_DIR)/localhost/; make mv to=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	#
	make openhab_start_withScreen
	make autostart
	# ggf. make myopenhab
	@#
	cd $(HCANHAB_DIR); make generateHabHcan
	cd $(HCANHAB_DIR); make installHabHcan
	@#
	make install_oracle_arm_jre
	# make reboot

install_oracle_arm_jre:
	@#cd $(OPENHABDOWNLOADS_DIR); tar xfvz $(OPENHABDOWNLOADS_DIR)/jdk-8u33-linux-arm-vfp-hflt.tar.gz
	cd $(OPENHABDOWNLOADS_DIR); tar xfvz $(OPENHABDOWNLOADS_DIR)/jdk-8u65-linux-arm32-vfp-hflt.tar.gz
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/jdk1.8.0
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/myOpenHabInstallation
	@#$(_SCP) $(OPENHABDOWNLOADS_DIR)/jdk1.8.0_33/* $(SCP_ZIEL_DIR)/../jdk1.8.0
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/jdk1.8.0_65/* $(SCP_ZIEL_DIR)/../jdk1.8.0
	# Falls noetig, auf Bananapi: sudo chmod 755 /home/tt/jdk1.8.0/*

openhab_start_withScreen:
	@# Damit wird das richtige JDK verwendet und die Ausgaben von Screen gemanaged werden: 
	@# screen -S 'OpenHAB Server' -dm /home/bananapi/jdk1.8.0/bin/java  in der start.sh (./Bananian_1504/start.sh) voranstellen
	$(_SCP) $(HCANHAB_DIR)/bananapi/start.sh $(SCP_ZIEL_DIR)/start.sh
	$(_SCP) $(HCANHAB_DIR)/bananapi/start_debug.sh $(SCP_ZIEL_DIR)/start_debug.sh

myopenhab:
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/org.openhab.io.myopenhab-1.7.0.jar $(SCP_ZIEL_DIR)/addons/org.openhab.io.myopenhab.jar
	@echo "\nuuid: (notieren und mit q beenden)" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/uuid
	@echo "\nsecret (notieren und mit q beenden):" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/secret
	@echo "Hintergrund:  Inhalt von   webapps/static/uuid  und webapps/static/secret"
	@echo "\n\n   @ https://my.openhab.org/ @ Account  beides eingetragen! "
	@echo "\n Die beiden Dateien werden erst nach sh ./myOpenHabInstallation/start.sh -Aufruf angelegt (ssh bananapi@tt).\n\n"  
	
reboot:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo reboot
