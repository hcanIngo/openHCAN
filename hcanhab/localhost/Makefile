# localhost
include $(HCANHAB_DIR)/MakefileDefines.inc

all:
	make unzip_openhab_installation
	make mv to=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)
	@#
	cd $(HCANHAB_DIR); make installHabHcan

unzip_openhab_installation:
	mkdir -p $(OPENHABDOWNLOADS_DIR)/tempinst
	unzip -u $(OPENHABDOWNLOADS_DIR)/distribution-$(OPENHABVERSION)-runtime.zip \
		-d $(OPENHABDOWNLOADS_DIR)/tempinst
	@#
	@# Liste von Bindings die entpackt werden sollen:	
	unzip -u $(OPENHABDOWNLOADS_DIR)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.binding.tcp-$(OPENHABVERSION).jar \
		org.openhab.binding.http-$(OPENHABVERSION).jar \
		org.openhab.binding.ntp-$(OPENHABVERSION).jar \
		org.openhab.binding.exec-$(OPENHABVERSION).jar \
		org.openhab.binding.weather-$(OPENHABVERSION).jar \
		org.openhab.binding.enigma2-$(OPENHABVERSION).jar \
		-d $(OPENHABDOWNLOADS_DIR)/tempinst/addons
	@#
	unzip -u $(OPENHABDOWNLOADS_DIR)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.persistence.db4o-$(OPENHABVERSION).jar \
		org.openhab.persistence.logging-$(OPENHABVERSION).jar \
		org.openhab.persistence.exec-$(OPENHABVERSION).jar \
		org.openhab.persistence.rrd4j-$(OPENHABVERSION).jar \
		-d $(OPENHABDOWNLOADS_DIR)/tempinst/configurations/persistence

mv:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p $(to)
	$(_SCP) $(OPENHABDOWNLOADS_DIR)/tempinst/* $(ZIEL_USER)@$(ZIEL_PC):$(to)
	rm -f -R $(OPENHABDOWNLOADS_DIR)/tempinst
	
	
reboot:
	# keine Funktion

start: 
	make stop
	sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start.sh # start_debug.sh

stop: 
	# sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/pskill

state:
	# keine Funktion	