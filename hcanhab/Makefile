CONF = /home/il/Dropbox/HCAN/HI_SERVER/openhab_conf
include $(CONF)/myconf.inc

#ZIEL_PC = localhost
ZIEL_PC = bananapi

ifeq ($(ZIEL_PC),localhost)
	ZIEL_USER = il
else ifeq  ($(ZIEL_PC),bananapi)
	ZIEL_USER = tt
endif

ZIEL_ORDNER = myOpenHabInstallation
OPENHABVERSION = 1.7.1
SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
HCAN_INSTALLATION = installation.xml

# epiphany-browser, firefox, chromium-browser
BROWSER = firefox
URL = http://$(ZIEL_PC):8080/openhab.app?sitemap=hcan
REMOTE_URL = https://my.openhab.org/openhab.app?sitemap=hcan


all: 	
	cd $(ZIEL_PC); make all	
	make generateHabHcan
	make installHabHcan

########################
_SSH = sshpass -p $(PT) ssh -t
_SCP = sshpass -p $(PT) scp -r
SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
########################	
reboot:
	cd $(ZIEL_PC); make reboot

start: 
	cd $(ZIEL_PC); make start

stop: 
	cd $(ZIEL_PC); make stop

state:
	cd $(ZIEL_PC); make state

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
