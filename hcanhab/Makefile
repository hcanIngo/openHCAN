# main
include ./MakefileDefines.inc

include $(HCANHAB_DIR)/$(ZIEL_PC)/Makefile

# epiphany-browser, firefox, chromium-browser
BROWSER = firefox
URL = http://$(ZIEL_PC):8080/openhab.app?sitemap=hcan
REMOTE_URL = https://my.openhab.org/openhab.app?sitemap=hcan

run_webclient: 
	$(BROWSER) $(URL)

rrun_webclient_remote: 
	$(BROWSER) $(REMOTE_URL)

########################
clean:
	cd xsl; make cleanHabHcan

generateHabHcan: 
	cd xsl; make generateHabHcan HCAN_INSTALLATION=$(CONF)/$(HCAN_INSTALLATION)
	
installHabHcan:	
	# persoenliche conf holen:
	$(_SCP) $(CONF)/* ./staticConfigurations/conf_templates/
	@#
	make generateHabHcan
	@# 
	# IP des C1612-Servers eintragen:
	find ./staticConfigurations/conf_templates/*.* -type f -exec sed -i "s/C1612server/$(C1612ServerIP)/g" {} \;
	find ./generated/*.* -type f -exec sed -i "s/C1612server/$(C1612ServerIP)/g" {} \;	
	@# 
	# Ins Ziel kopieren:
	$(_SCP) ./generated/hcan_cat.sitemap $(SCP_ZIEL_DIR)/configurations/sitemaps/hcan.sitemap
	$(_SCP) ./generated/*.items $(SCP_ZIEL_DIR)/configurations/items/
	$(_SCP) ./generated/*.rules $(SCP_ZIEL_DIR)/configurations/rules/
	@# 
	# static confs hinzufuegen: (static-sitemap bereits in generated enthalten)
	$(_SCP) ./staticConfigurations/fix/transform/*.* $(SCP_ZIEL_DIR)/configurations/transform/
	$(_SCP) ./staticConfigurations/conf_templates/openhab.cfg $(SCP_ZIEL_DIR)/configurations/openhab.cfg
	$(_SCP) ./staticConfigurations/conf_templates/*.items $(SCP_ZIEL_DIR)/configurations/items/
	$(_SCP) ./staticConfigurations/conf_templates/*.rules $(SCP_ZIEL_DIR)/configurations/rules/
