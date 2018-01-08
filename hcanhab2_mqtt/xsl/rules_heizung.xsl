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
 	if(receivedCommand == "1") { // Aus
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Aus")
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(0) // "sitemap-Aktu."
		return
	}
	else if(receivedCommand == "2") { // Auto
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Auto")
		return
	}
	else if(receivedCommand == "3") { // Therm
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Therm unbegr")
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(20.5) // "sitemap-Aktu."
		SOLLTEMP_DAUER_<xsl:value-of select="@name" />.sendCommand("20.5/0.0") // mqtt-cmd (0= fuer unbegrenzte Zeit)
	}
	else if(receivedCommand == "33") { // Therm (3 Std.)
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Therm 3h")
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(20.5) // "sitemap-Aktu."
		SOLLTEMP_DAUER_<xsl:value-of select="@name" />.sendCommand("20.5/3.0") // mqtt-cmd
	}
	else if(receivedCommand == "4") {
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost unbegr")
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(10) // "sitemap-Aktu."
		SOLLTEMP_DAUER_<xsl:value-of select="@name" />.sendCommand("10/0.0") // mqtt-cmd (0= fuer unbegrenzte Zeit)
	}
	else if(receivedCommand == "34") { // ✳ (3 Std.)
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost 3h")
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(10) // "sitemap-Aktu."
		SOLLTEMP_DAUER_<xsl:value-of select="@name" />.sendCommand("10/3.0") // mqtt-cmd
	}			
end

</xsl:template>

</xsl:stylesheet>
