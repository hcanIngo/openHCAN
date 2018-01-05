A = "<device name=\"
E = \"></device>"
DEVICES := $(patsubst %.c, $(A)%$(E) , $(wildcard *.c))


INDENT_OPTS = -orig -ci4

all:
	@#make print_deviceList
	@make devices_xml
	@#less devices.xml
	@make devices_h
	@#less ../devices.h
	@make devices_c
	@#less ../devices.c
	@make timer_c
	@#less ../timer.c

print_deviceList:
	@echo DeviceList: $(DEVICES)

devices_xml:
	@#rm -f devices.xml
	@#> devices.xml
	@echo "<!-- AUTOMATICLY GENERATED FROM /device/Makefile -->" >	devices.xml
	@echo "<Controllerboard-devicelist>" >> devices.xml
	@echo $(DEVICES) >> devices.xml
	@echo "</Controllerboard-devicelist>" >> devices.xml

devices_h: mk_devices_h.xsl devices.xml
	xalan -in devices.xml -xsl mk_devices_h.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > ../devices.h

devices_c: mk_devices_c.xsl devices.xml
	xalan -in devices.xml -xsl mk_devices_c.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > ../devices.c

timer_c: mk_timer_c.xsl devices.xml
	xalan -in devices.xml -xsl mk_timer_c.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > ../timer.c
