<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
	<xsl:apply-templates select="bereich/raum/board/rolladen"/>
	<xsl:apply-templates select="bereich/board/rolladen"/>
</xsl:template>

<xsl:template match="rolladen">


rule "Rolladenkommando empfangen"
when
	Item ROLLADEN_<xsl:value-of select="@name" /> received command
then
 	var Number sollPos = ROLLADEN_<xsl:value-of select="@name" />.state
 	// logInfo("IL-Roll", "ROLLADEN_<xsl:value-of select="@name" />: " + receivedCommand.toString)

	sollPos = 100 - sollPos; // invertieren, da die rolladen von openhab zu openhcan invertierte Logik aufweisen
	SET_ROLLADEN_<xsl:value-of select="@name" />.sendCommand(sollPos.toString)
end

</xsl:template>


</xsl:stylesheet>

