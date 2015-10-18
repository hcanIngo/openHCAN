CONF = /home/il/Dropbox/HCAN/HI_SERVER/openhab_conf
include $(CONF)/myconf.inc

ZIEL_PC = localhost
#ZIEL_PC = bananapi

ZIEL_ORDNER = myOpenHabInstallation
_SSH = sshpass -p $(PT) ssh -t
_SCP = sshpass -p $(PT) scp -r

ifeq ($(ZIEL_PC),localhost)
	OPENHABVERSION = 1.7.1
	ZIEL_USER = il
	START = sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start.sh # start_debug.sh
	STOP = # sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/pskill
	STATE =  
	HCAN_INSTALLATION = installation.xml
else ifeq  ($(ZIEL_PC),bananapi)
	### Banana-Pi-Server
	ZIEL_OS = Bananian_1504
	OPENHABVERSION = 1.7.1
	ZIEL_USER = tt
	OPENHAB_SERVICE = openhab
	START = $(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) start
	STOP  = $(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) stop
	STATE = $(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) status
	HCAN_INSTALLATION = installation.xml
endif

SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/$(ZIEL_ORDNER)

# epiphany-browser, firefox, chromium-browser
BROWSER = firefox
URL = http://$(ZIEL_PC):8080/openhab.app?sitemap=hcan
REMOTE_URL = https://my.openhab.org/openhab.app?sitemap=hcan

all: 	
	make install_openhab_runtime
ifeq  ($(ZIEL_PC),bananapi)
		# make openhab_start_withScreen
		# make install_oracle_arm_jre
		# make openhab_autostartService
		# ggf. make myopenhab
endif	
	make generateHabHcan
	make installHabHcan

########################
install_openhab_runtime:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) rm -Rf /home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-runtime.zip \
		-d /home/$(ZIEL_USER)/$(ZIEL_ORDNER)	
	@#
	@# Liste von Bindings die entpackt werden sollen:	
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.binding.tcp-$(OPENHABVERSION).jar \
		org.openhab.binding.http-$(OPENHABVERSION).jar \
		org.openhab.binding.ntp-$(OPENHABVERSION).jar \
		org.openhab.binding.exec-$(OPENHABVERSION).jar \
		org.openhab.binding.weather-$(OPENHABVERSION).jar \
		org.openhab.binding.enigma2-$(OPENHABVERSION).jar \
		-d /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/addons
	@#
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.persistence.db4o-$(OPENHABVERSION).jar \
		org.openhab.persistence.logging-$(OPENHABVERSION).jar \
		org.openhab.persistence.exec-$(OPENHABVERSION).jar \
		org.openhab.persistence.rrd4j-$(OPENHABVERSION).jar \
		-d /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/configurations/persistence

install_oracle_arm_jre: 
	cd $(OPENHABDOWNLOADS); tar xfvz $(OPENHABDOWNLOADS)/jdk-8u33-linux-arm-vfp-hflt.tar.gz
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/jdk1.8.0
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /home/$(ZIEL_USER)/myOpenHabInstallation
	$(_SCP) $(OPENHABDOWNLOADS)/jdk1.8.0_33/* $(SCP_ZIEL_DIR)/../jdk1.8.0
	# Falls noetig, auf Bananapi: sudo chmod 755 /home/tt/jdk1.8.0/*

openhab_start_withScreen:
	@# Damit wird das richtige JDK verwendet und die Ausgaben von Screen gemanaged werden: 
	@# screen -S 'OpenHAB Server' -dm /home/bananapi/jdk1.8.0/bin/java  in der start.sh (./Bananian_1504/start.sh) voranstellen
	$(_SCP) ./staticConfigurations/fix/$(ZIEL_OS)/start.sh $(SCP_ZIEL_DIR)/start.sh
	$(_SCP) ./staticConfigurations/fix/$(ZIEL_OS)/start_debug.sh $(SCP_ZIEL_DIR)/start_debug.sh

myopenhab:
	$(_SCP) $(OPENHABDOWNLOADS)/org.openhab.io.myopenhab-1.7.0.jar $(SCP_ZIEL_DIR)/addons/org.openhab.io.myopenhab.jar
	@echo "\nuuid:" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/uuid
	@echo "\nsecret:" 
	@$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) less /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/webapps/static/secret
	@echo "\n\n   @ https://my.openhab.org/  wird der Inhalt von  webapps/static/uuid  und webapps/static/secret    eingetragen! "
	@echo "\n Die beiden Dateien werden erst nach sh ./myOpenHabInstallation/start.sh -Aufruf angelegt (ssh bananapi@tt).\n\n"  

openhab_autostartService:
	$(_SCP) ./staticConfigurations/fix/$(ZIEL_OS)/$(OPENHAB_SERVICE) $(SCP_ZIEL_DIR)/../$(OPENHAB_SERVICE)
	#
	# make stop
	# Auf dem Bananapi verschieben
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(OPENHAB_SERVICE) /etc/init.d/$(OPENHAB_SERVICE)
	#
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo chmod a+x /etc/init.d/$(OPENHAB_SERVICE)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo update-rc.d $(OPENHAB_SERVICE) defaults

########################	
reboot:
	# $(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo shutdown -r -t 0
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo reboot

start: 
	make stop
	$(START)

stop: 
	$(STOP)

state:
	$(STATE)

run_webclient: 
	$(BROWSER) $(URL)

rrun_webclient_remote: 
	$(BROWSER) $(REMOTE_URL)

	
########################
clean:
	cd xsl; make cleanHabHcan

generateHabHcan: 
	cd xsl; make generateHabHcan HCAN_INSTALLATION=$(CONF)/$(HCAN_INSTALLATION)
	
installHabHcan:	
	# persoenliche conf holen:
	$(_SCP) $(CONF)/* ./staticConfigurations/conf_templates/
	@# 
	# IP des C1612-Servers eintragen:
	find ./staticConfigurations/conf_templates/*.* -type f -exec sed -i "s/C1612server/$(C1612ServerIP)/g" {} \;
	find ./generated/*.* -type f -exec sed -i "s/C1612server/$(C1612ServerIP)/g" {} \;	
	@# 
	# Ins Ziel kopieren:
	$(_SCP) ./generated/hcan_cat.sitemap $(SCP_ZIEL_DIR)/configurations/sitemaps/hcan.sitemap
	$(_SCP) ./generated/*.items $(SCP_ZIEL_DIR)/configurations/items/
	$(_SCP) ./generated/*.rules $(SCP_ZIEL_DIR)/configurations/rules/
	@# 
	# static confs hinzufuegen: (static-sitemap bereits in generated enthalten)
	$(_SCP) ./staticConfigurations/fix/transform/*.* $(SCP_ZIEL_DIR)/configurations/transform/
	$(_SCP) ./staticConfigurations/conf_templates/openhab.cfg $(SCP_ZIEL_DIR)/configurations/openhab.cfg
	$(_SCP) ./staticConfigurations/conf_templates/*.items $(SCP_ZIEL_DIR)/configurations/items/
	$(_SCP) ./staticConfigurations/conf_templates/*.rules $(SCP_ZIEL_DIR)/configurations/rules/
