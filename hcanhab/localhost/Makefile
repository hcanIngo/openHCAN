# localhost
include ../Makefile

all:
	make unzip_openhab_installation
	make mv to=/home/$(ZIEL_USER)/$(ZIEL_ORDNER)

unzip_openhab_installation:
	mkdir -p $(OPENHABDOWNLOADS)/tempinst
	unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-runtime.zip \
		-d $(OPENHABDOWNLOADS)/tempinst
	@#
	@# Liste von Bindings die entpackt werden sollen:	
	unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.binding.tcp-$(OPENHABVERSION).jar \
		org.openhab.binding.http-$(OPENHABVERSION).jar \
		org.openhab.binding.ntp-$(OPENHABVERSION).jar \
		org.openhab.binding.exec-$(OPENHABVERSION).jar \
		org.openhab.binding.weather-$(OPENHABVERSION).jar \
		org.openhab.binding.enigma2-$(OPENHABVERSION).jar \
		-d $(OPENHABDOWNLOADS)/tempinst/addons
	@#
	unzip -u $(OPENHABDOWNLOADS)/distribution-$(OPENHABVERSION)-addons.zip \
		org.openhab.persistence.db4o-$(OPENHABVERSION).jar \
		org.openhab.persistence.logging-$(OPENHABVERSION).jar \
		org.openhab.persistence.exec-$(OPENHABVERSION).jar \
		org.openhab.persistence.rrd4j-$(OPENHABVERSION).jar \
		-d $(OPENHABDOWNLOADS)/tempinst/configurations/persistence

mv:
	mkdir -p $(to)
	$(_SCP) $(OPENHABDOWNLOADS)/tempinst/* $(to)
	rm -f -R $(OPENHABDOWNLOADS)/tempinst
	

########################	
reboot:
	# keine Funktion

start: 
	make stop
	sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/start.sh # start_debug.sh

stop: 
	# sh /home/$(ZIEL_USER)/$(ZIEL_ORDNER)/pskill

state:
	# keine Funktion	