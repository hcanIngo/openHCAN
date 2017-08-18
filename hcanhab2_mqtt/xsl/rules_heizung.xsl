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


rule "Heizung OpenHab-Selektion veraendert"
when
	Item HEIZMODE_<xsl:value-of select="@name" /> received command
then
 	//logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: " + receivedCommand.toString)

 	if(receivedCommand == "1") { // Aus
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Aus")
		// HEIZMODE_<xsl:value-of select="@name" />.sendCommand("1")
		sollTemp<xsl:value-of select="@name" /> = 0
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(sollTemp<xsl:value-of select="@name" />) // sitemap-Aktu.
		return
	}
	else if(receivedCommand == "2") { // Auto
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Auto")
		// HEIZMODE_<xsl:value-of select="@name" />.sendCommand("2")
		return
	}
	else if(receivedCommand == "3") { // Therm
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Therm")
		sollTemp<xsl:value-of select="@name" /> = 20.5	
	}
	else if(receivedCommand == "4") { // ✳ (3 Std.)
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost kurz")
		// TODO Schnittstelle zu mqttPahoClient erweitern um Dauer
		sollTemp<xsl:value-of select="@name" /> = 10	
	}
	else if(receivedCommand == "5") { // ✳ lange
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost lang")
		// TODO Schnittstelle zu mqttPahoClient erweitern um Dauer
		sollTemp<xsl:value-of select="@name" /> = 10	
	}	
	
	logInfo("IL@HM Solltemp angepasst ", "sollTemp<xsl:value-of select="@name" />: " + sollTemp<xsl:value-of select="@name" />.toString)
	
	//SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(sollTemp<xsl:value-of select="@name" />) // sitemap-Aktu.
	SOLLTEMP_<xsl:value-of select="@name" />.sendCommand(sollTemp<xsl:value-of select="@name" />) // mqtt-cmd
end

</xsl:template>

</xsl:stylesheet>
