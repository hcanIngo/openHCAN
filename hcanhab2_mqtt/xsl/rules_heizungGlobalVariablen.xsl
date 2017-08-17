<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
// deprecated: import org.openhab.core.library.types.*
// deprecated: import org.openhab.model.script.actions.*
// deprecated: import org.openhab.core.transform.actions.*

	<xsl:apply-templates select="bereich/raum/board/heizung"/>
	<xsl:apply-templates select="bereich/board/heizung"/>
</xsl:template>

<xsl:template match="heizung">
var Number sollTemp<xsl:value-of select="@name" /> = 0 <!-- Hinweis: Globale Variablen müssen am Anfahng stehen. -->
</xsl:template>


</xsl:stylesheet>

