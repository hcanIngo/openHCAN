generateHabHcan: 
	make cleanHabHcan; make HabHcan.sitemap; make all.items; make all.rules

cleanHabHcan:
	rm -fR ../generated

HabHcan.sitemap: mk_HabHcan_sitemap.xsl $(HCAN_INSTALLATION)
	mkdir -p ../generated
	xalan -in $(HCAN_INSTALLATION) -xsl mk_HabHcan_sitemap.xsl > ../generated/HabHcanDyn.sitemap
	cat ../generated/HabHcanDyn.sitemap ../staticConfigurations/conf_templates/staticPart.sitemap > ../generated/hcan_cat.sitemap
	echo "} // Endeklammer der Gesamtsitemap (dyn.+ statische Anteile)" >> ../generated/hcan_cat.sitemap
	cd ../generated/; rm -f HabHcanDyn.sitemap


ITEM_FILES := $(patsubst items_%.xsl,%.items,$(wildcard items_*.xsl))

all.items: $(ITEM_FILES)
	echo ITEM_FILES: $(ITEM_FILES)

$(ITEM_FILES): %.items: items_%.xsl
	xalan -in $(HCAN_INSTALLATION) -xsl $<    >  ../generated/$@
	
	
RULE_FILES := $(patsubst rules_%.xsl,%.rules,$(wildcard rules_*.xsl))
	
all.rules: $(RULE_FILES)
	echo RULE_FILES: $(RULE_FILES)
	# 
	cat ../generated/heizungGlobalVariablen.rules ../generated/heizung.rules > ../generated/heizung_cat.rules
	cd ../generated/; rm -f heizungGlobalVariablen.rules heizung.rules

$(RULE_FILES): %.rules: rules_%.xsl
	xalan -in $(HCAN_INSTALLATION) -xsl $<    >  ../generated/$@
