# main
#
include ./MakefileDefines.inc
include $(HCANHAB_DIR)/$(ZIEL_PC)/Makefile

# epiphany-browser, firefox, chromium-browser
BROWSER = firefox
#URL = http://$(ZIEL_PC):8080/classicui/app?sitemap=hcan
URL = http://$(ZIEL_PC):8080/basicui/app?sitemap=hcan
# http://$(ZIEL_PC):8080/ui/index.html  # zur Konfiguration

run_webclient: 
	$(BROWSER) $(URL)

########################
clean:
	rm -fR ./staticConfigurations/conf_templates
	cd xsl; make cleanHabHcan

generateHabHcan: 
	#make clean
	cd xsl; make generateHabHcan HCAN_INSTALLATION=$(CONF)/$(HCAN_INSTALLATION)
	
installHabHcan:	
	# persoenliche conf holen:
	mkdir -p ./staticConfigurations/conf_templates
	$(_SCP) $(CONF)/* ./staticConfigurations/conf_templates/
	@#
	make generateHabHcan
	@#
	# persoenliche conf nochmal holen:
	$(_SCP) $(CONF)/* ./staticConfigurations/conf_templates/
	@#
	# statische allgemein gueltige conf holen:  
	$(_SCP) ./staticConfigurations/fix/conf/* ./staticConfigurations/conf_templates/	
	@# 
	# Ins Ziel kopieren:
	$(_SCP) ./generated/hcan_cat.sitemap $(SCP_ZIEL_DIR)/conf/sitemaps/hcan.sitemap
	$(_SCP) ./generated/*.items $(SCP_ZIEL_DIR)/conf/items/
	$(_SCP) ./generated/*.rules $(SCP_ZIEL_DIR)/conf/rules/
	@# 
	# static confs hinzufuegen: (static-sitemap bereits in generated enthalten)
	$(_SCP) ./staticConfigurations/fix/transform/*.* $(SCP_ZIEL_DIR)/conf/transform/
	$(_SCP) ./staticConfigurations/conf_templates/*.cfg $(SCP_ZIEL_DIR)/conf/services/
	$(_SCP) ./staticConfigurations/conf_templates/*.items $(SCP_ZIEL_DIR)/conf/items/
	$(_SCP) ./staticConfigurations/conf_templates/*.rules $(SCP_ZIEL_DIR)/conf/rules/

installationXml4telican:
	$(_SSH) $(ZIEL_USER)@$(ZIEL_PC) mkdir -p /etc/hcan 
	$(_SCP) $(CONF)/$(HCAN_INSTALLATION) /etc/hcan/

	