# Systemd
include $(HCANHAB_DIR)/MakefileDefines.inc

OPENHAB_SERVICE = openhab.service

autostart:
	@echo -- Systemd-Autostart --
	$(_SCP) $(HCANHAB_DIR)/$(ZIEL_PC)/Systemd/$(OPENHAB_SERVICE) $(SCP_ZIEL_DIR)/../$(OPENHAB_SERVICE)
	#
	# make stop
	#
	# Auf dem Bananapi verschieben
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(OPENHAB_SERVICE) /usr/lib/systemd/system/$(OPENHAB_SERVICE)	
	#
	# Symbolischen Link fuer Autostart beim Booten: 
	@# https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units
	sudo systemctl enable $(OPENHAB_SERVICE)

start: 
	make stop
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) start

stop: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) stop

state:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) status