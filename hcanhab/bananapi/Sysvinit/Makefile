# Sysvinit
include $(HCANHAB_DIR)/MakefileDefines.inc

OPENHAB_SERVICE = openhab

autostart:
	@echo -- Sysvinit-Autostart --
	$(_SCP) $(HCANHAB_DIR)/$(ZIEL_PC)/Sysvinit/$(OPENHAB_SERVICE) $(SCP_ZIEL_DIR)/../$(OPENHAB_SERVICE)
	#
	# make stop
	#
	# Auf dem Bananapi verschieben
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo mv ./$(OPENHAB_SERVICE) /etc/init.d/$(OPENHAB_SERVICE)
	#
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo chmod a+x /etc/init.d/$(OPENHAB_SERVICE)
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo update-rc.d $(OPENHAB_SERVICE) defaults

start:
	make stop
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) start

stop:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) stop

state:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) sudo /etc/init.d/$(OPENHAB_SERVICE) status
