# Systemd
include $(HCANHAB_DIR)/MakefileDefines.inc

SERVICE = openhab

autostart:
	@echo -- Systemd-Autostart --
	$(_SCP) $(HCANHAB_DIR)/$(ZIEL_PC)/Systemd/$(SERVICE).service $(SCP_ZIEL_DIR)/../$(SERVICE).service
	#
	# make stop
	#
	# Auf dem Bananapi verschieben
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(SERVICE).service /etc/systemd/system/$(SERVICE).service	
	#
	# Symbolischen Link fuer Autostart beim Booten: 
	@# https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units
	@# $(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl enable $(SERVICE)

start: 
	make stop
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl start $(SERVICE)

stop: 
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo systemctl stop $(SERVICE)

status:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) systemctl status $(SERVICE)