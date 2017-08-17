<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
//
//                        openHAB        openHCAN
// Input von REEDKONTAKT: OPEN/CLOSED &lt;- REEDKONTAKT_STATE_REPLAY (42), REEDKONTAKT_STATE_CHANGE (43)
//
		<xsl:apply-templates select="bereich/raum/board/reedkontakt"/>
		<xsl:apply-templates select="bereich/board/reedkontakt"/> <!-- Z.B. Musikanlage im EG -->
</xsl:template>

<xsl:template match="reedkontakt">
Contact REEDKONTAKT_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" />" &lt;contact&gt; (Fenster) {mqtt="&lt;[bpiBroker:cb&gt;:state:CLOSED:<xsl:value-of select="@gruppe" />/CLOSED], &lt;[bpiBroker:cb&gt;:state:OPEN:<xsl:value-of select="@gruppe" />/OPEN]", autoupdate="false"}
</xsl:template>


</xsl:stylesheet>

