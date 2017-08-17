# localhost
include $(HCANHAB_DIR)/MakefileDefines.inc

all:
	make cleanZielOrdner
	make unzip_openhab_installation
	make mv to=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	@#
	cd $(HCANHAB_DIR); make installHabHcan

cleanZielOrdner:
	rm -f -R /home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	
unzip_openhab_installation:
	mkdir -p $(OPENHABDOWNLOADS_DIR)/tempinst
	unzip -u $(OPENHABDOWNLOADS_DIR)/$(OPENHABVERSION).zip \
		-d $(OPENHABDOWNLOADS_DIR)/tempinst
	
mv:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p $(to)
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/tempinst/* $(ZIEL_USER)@$(ZIEL_PC):$(to)
	rm -f -R $(OPENHABDOWNLOADS_DIR)/tempinst
	
	
reboot:
	# keine Funktion

start: 
	make stop
	sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start.sh
	@# sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start_debug.sh

stop: 
	# sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/pskill

state:
	# keine Funktion	