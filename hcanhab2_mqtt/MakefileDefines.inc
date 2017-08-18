CONF = /home/il/Dropbox/HCAN/HI_SERVER/openhab2_conf_mqtt
include $(CONF)/myconf.inc

OPENHABVERSION = openhab-2.1.0
#HCAN_INSTALLATION = installation_Test.xml
#HCAN_INSTALLATION = installation_Test_Lens.xml
HCAN_INSTALLATION = installation.xml


#ZIEL_PC = localhost
ZIEL_PC = bananapi

ifeq ($(ZIEL_PC),localhost)
	ZIEL_USER = il
else ifeq  ($(ZIEL_PC),bananapi)
	ZIEL_USER = tt
endif

# nicht veraendern: 
ZIEL_ORDNER = myOpenHabInstallation

########################
_SSH = sshpass -p $(PT) ssh -t
_SCP = sshpass -p $(PT) scp -r
SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
