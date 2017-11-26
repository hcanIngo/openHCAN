# localhost
#
include $(HCANHAB_DIR)/MakefileDefines.inc

all:
	make cleanZielOrdner
	make unzip_openhab_installation
	make moveIt OHInstZiel=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	@#
	cd $(HCANHAB_DIR); make installHabHcan

cleanZielOrdner:
	rm -f -R /home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	
unzip_openhab_installation:
	mkdir -p $(OPENHABDOWNLOADS_DIR)/tempinst
	unzip -u $(OPENHABDOWNLOADS_DIR)/$(OPENHABVERSION).zip \
		-d $(OPENHABDOWNLOADS_DIR)/tempinst

moveIt:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p $(OHInstZiel)
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/tempinst/* $(ZIEL_USER)@$(ZIEL_PC):$(OHInstZiel)
	rm -f -R $(OPENHABDOWNLOADS_DIR)/tempinst

start: 
	make stop
	sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start.sh
	@# sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start_debug.sh
