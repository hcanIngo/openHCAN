<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
		<xsl:apply-templates select="bereich/raum/board/powerport"/>
		<xsl:apply-templates select="bereich/board/powerport"/>
</xsl:template>

<xsl:template match="powerport">
	<xsl:choose>
	<xsl:when test="(@typ = 'lampe')">
rule "Powerport (lampe) schalten"
when 
	Item LAMPE_<xsl:value-of select="@name" /> received command  or  Item LAMPE_<xsl:value-of select="@name" />_HUEE received command
then
	if (receivedCommand==ON) {
		/* Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
		   Sonst wuerde die Button-Reaktion zu langsam erfolgen. */
		LAMPE_<xsl:value-of select="@name" />.postUpdate("ON");
	}
	if (receivedCommand==OFF) {
		/* Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
		   Sonst wuerde die Button-Reaktion zu langsam erfolgen. */
		LAMPE_<xsl:value-of select="@name" />.postUpdate("OFF");
	}
end
	</xsl:when>
	<xsl:otherwise> <!-- Sonstige -->
rule "Powerport (sonstige) schalten"
when 
	Item SONSTIGE_<xsl:value-of select="@name" /> received command  or  Item SONSTIGE_<xsl:value-of select="@name" />_HUEE received command
then
	if (receivedCommand==ON) {
		/* Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
		   Sonst wuerde die Button-Reaktion zu langsam erfolgen. */
		SONSTIGE_<xsl:value-of select="@name" />.postUpdate("ON");
	}
	if (receivedCommand==OFF) {
		/* Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
		   Sonst wuerde die Button-Reaktion zu langsam erfolgen. */
		SONSTIGE_<xsl:value-of select="@name" />.postUpdate("OFF");
	}
end
	</xsl:otherwise>
	</xsl:choose>
</xsl:template>


</xsl:stylesheet>

