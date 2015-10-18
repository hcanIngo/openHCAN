<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
		<xsl:apply-templates select="bereich/raum/board/tempsensor"/>
		<xsl:apply-templates select="bereich/board/tempsensor"/>
</xsl:template>

<xsl:template match="tempsensor">
Number TEMPERATUR_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" /> [%d °C]" &lt;temperature&gt; (Temperaturen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=tempsensor&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:30100:JS(getState.js)]" }
</xsl:template>


</xsl:stylesheet>

