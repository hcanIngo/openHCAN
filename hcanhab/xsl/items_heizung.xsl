<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
	<xsl:apply-templates select="bereich/raum/board/heizung"/>
	<xsl:apply-templates select="bereich/board/heizung"/>
</xsl:template>

<xsl:template match="heizung">
Number HEIZUNG_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" />" &lt;heating&gt;
<!-- Number SOLLTEMP_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" /> [soll: %d °C]" &lt;temperature&gt; (Heizungen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=heizung&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:1000:JS(getState.js)]", autoupdate="false" } -->
Number ISTSOLLTEMP_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" /> [soll: %d °C]" &lt;temperature&gt; (Heizungen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=heizung&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:1000:JS(getState.js)]", autoupdate="false" }
</xsl:template>


</xsl:stylesheet>

