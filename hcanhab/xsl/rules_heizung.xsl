<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// Ende der Definition der globalen Variablen
	<xsl:apply-templates select="bereich/raum/board/heizung"/>
	<xsl:apply-templates select="bereich/board/heizung"/>
</xsl:template>

<xsl:template match="heizung">
rule "Heizung Soll-Temperatur in OpenHab veraendert"
when
	Item SOLLTEMP_<xsl:value-of select="@name" /> received command
	// Item muss autoupdate="false" haben, sodass wenn der Button in der Sitemap gedrückt wird, das item nicht den Button-State annimmt!
then 
	sollTemp<xsl:value-of select="@name" /> = SOLLTEMP_<xsl:value-of select="@name" />.state as DecimalType // der item-State
	if (receivedCommand==2) {
		if (sollTemp<xsl:value-of select="@name" />&lt;17) sollTemp<xsl:value-of select="@name" /> = 20
		else sollTemp<xsl:value-of select="@name" /> = sollTemp<xsl:value-of select="@name" /> + 1
	}
	if (receivedCommand==1) sollTemp<xsl:value-of select="@name" /> = sollTemp<xsl:value-of select="@name" /> - 1
	
	if (sollTemp<xsl:value-of select="@name" />&lt;0)  sollTemp<xsl:value-of select="@name" /> = 0
	if (sollTemp<xsl:value-of select="@name" />&gt;30) sollTemp<xsl:value-of select="@name" /> = 30
	
	logInfo("", "sollTemp<xsl:value-of select="@name" />: " + sollTemp<xsl:value-of select="@name" />.toString)
	// say("Solltemp: " + sollTemp<xsl:value-of select="@name" />.toString)
	sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd="+sollTemp<xsl:value-of select="@name" />.toString+"&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")

	if (sollTemp<xsl:value-of select="@name" />&lt;11) {
		postUpdate("HEIZUNG_<xsl:value-of select="@name" />", "3") // Sitemap-Select-Item an die Situation anpassen: "Frostschutz"
	}
	else {
		postUpdate("HEIZUNG_<xsl:value-of select="@name" />", "1") // Sitemap-Select-Item an die Situation anpassen: "therm"
	}
	
	
	/* Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
	   Sonst wuerde die Button-'+/-'-Reaktion zu langsam erfolgen. Funzt so, da es nicht zum rule-Aufruf SOLLTEMP_... kommt: */
	postUpdate("SOLLTEMP_<xsl:value-of select="@name" />", sollTemp<xsl:value-of select="@name" />.toString)
end



rule "Heizung OpenHab-Selektion veraendert" // 0=aus, 1=therm, 2=auto, 3=frost, 4=frostLang
when
	Item HEIZUNG_<xsl:value-of select="@name" /> received command
then
	<!--sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd="+receivedCommand+"&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")-->	
 	switch(receivedCommand) {
		case 0 : sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd=aus&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
		case 1 : sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd=therm&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
		case 2 : sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd=auto&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
		case 3 : sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd=frost&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
		case 4 : sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd=frostLang&amp;d=heizung&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
	}
end

rule "Heizung - Aenderung des HCAN-Items"
when 
	Item HEIZUNG_<xsl:value-of select="@name" /> changed // Aenderung des HCAN-Items
then
	HEIZUNG_<xsl:value-of select="@name" />.state.toString // der item-State
	logInfo("IL", "changed " + HEIZUNG_<xsl:value-of select="@name" />.toString)
	      
	if (HEIZUNG_<xsl:value-of select="@name" />.state.toString == "0") postUpdate("HEIZUNG_<xsl:value-of select="@name" />", "0") // Sitemap-Select-Item an die Situation anpassen: "aus"
end

rule "Startup"
when 
	System started
then
	logInfo("IL", "System started " + HEIZUNG_<xsl:value-of select="@name" />.toString)
	
	if (HEIZUNG_<xsl:value-of select="@name" />.state.toString == "0") postUpdate("HEIZUNG_<xsl:value-of select="@name" />", "0") // Sitemap-Select-Item an die Situation anpassen: "aus"
	else postUpdate("HEIZUNG_<xsl:value-of select="@name" />", "2") // Sitemap-Select-Item an die Situation anpassen: "auto"
end
</xsl:template>

</xsl:stylesheet>
