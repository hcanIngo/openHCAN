# Verwendung per:  make --makefile=cp2pi.mk installHcanSocketd

TARGET = hcansocketd

CONF = /home/ingo/Dropbox/HCAN/HI_SERVER/openhab2_conf_mqtt
include $(CONF)/myconf.inc
ZIEL_PC = bananapi
ZIEL_USER = tt

_SCP = sudo sshpass -p $(PT) scp -r

SCP_ZIEL_DIR = $(ZIEL_USER)@$(ZIEL_PC):/home/$(ZIEL_USER)

installHcanSocketd:
	$(_SCP) ./* $(SCP_ZIEL_DIR)/openHCAN/$(TARGET)/
	
installOpenHcan:
	$(_SCP) ../* $(SCP_ZIEL_DIR)/openHCAN


	
# syslog: 
# ls -als /var/log/
# less /var/log/lastlog
# sudo less /var/log/daemon.log
# less +F /var/log/syslog
# cat /var/log/syslog |grep error
