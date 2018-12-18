# bananapi (mit Armbian)
#
include $(HCANHAB_DIR)/MakefileDefines.inc

START_SYSTEM = Systemd

include $(HCANHAB_DIR)/bananapi/$(START_SYSTEM)/Makefile

all:
	@# Voraussetzung: ZIEL_PC = localhost  ->  make all
	@# 
	make cleanZielOrdner
	@#
	cd $(HCANHAB_DIR)/localhost/; make unzip_openhab_installation
	cd $(HCANHAB_DIR)/localhost/; make moveIt OHInstZiel=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	@#
	make openhab_start_withScreen
	make autostart
	@#
	cd $(HCANHAB_DIR); make installHabHcan
	@#
	make install_oracle_arm_jre
	@#
	# make reboot
	# make myopenhab

cleanZielOrdner:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) rm -f -R /home/$(ZIEL_USER)/$(ZIEL_ORDNER)

install_oracle_arm_jre:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) rm -f -R /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/../jdk1.8.0
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/jdk1.8.0
	@#
	@# zuvor:                                                    jdk-8u171-linux-arm32-vfp-hflt.tar.gz
	cd $(OPENHABDOWNLOADS_DIR); tar xfvz $(OPENHABDOWNLOADS_DIR)/jdk-8u191-linux-arm32-vfp-hflt.tar.gz
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/jdk1.8.0_191/* $(SCP_ZIEL_DIR)/../jdk1.8.0
	# Falls noetig, auf dem Bananapi: sudo chmod 755 /home/tt/jdk1.8.0/*

openhab_start_withScreen:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo apt-get install screen mc vim git htop
	@# Damit wird das richtige JDK verwendet und die Ausgaben von Screen gemanaged werden: 
	$(_SCP) $(HCANHAB_DIR)/bananapi/setenv $(SCP_ZIEL_DIR)/runtime/bin/setenv

myopenhab:
	# siehe http://docs.openhab.org/addons/io/openhabcloud/readme.html
	@echo "\nuuid: (notieren und mit q beenden)" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/userdata/uuid
	@echo "\nsecret (notieren und mit q beenden):" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/userdata/openhabcloud/secret
	@echo "Hintergrund:  Inhalt von   uuid  und  secret"
	@echo "\n\n   @ https://myopenhab.org/ @ Account  beides eingetragen! "
	@echo "\n Die beiden Dateien werden erst nach sh ./myOpenHabInstallation/start.sh -Aufruf angelegt (ssh bananapi@tt).\n\n"  
	
reboot:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo reboot
