CONF = /home/il/Dropbox/HCAN/HI_SERVER/openhab_conf
include $(CONF)/myconf.inc

#ZIEL_PC = localhost
ZIEL_PC = bananapi

OPENHABVERSION = 1.8.1
HCAN_INSTALLATION = installation.xml


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