# Verwendung per:  make --makefile=cp2pi.mk installMqtt

TARGET = hcan4mqttpc

CONF = /home/il/Dropbox/HCAN/HI_SERVER/openhab2_conf_mqtt
include $(CONF)/myconf.inc
ZIEL_PC = bananapi
ZIEL_USER = tt

_SCP = sshpass -p $(PT) scp -r
#_SCP = scp -r

SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)

installMqtt:
	$(_SCP) ./* $(SCP_ZIEL_DIR)/openHCAN/$(TARGET)/
	
installOpenHcan:
	$(_SCP) ../* $(SCP_ZIEL_DIR)/openHCAN