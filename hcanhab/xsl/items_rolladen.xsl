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
Rollershutter ROLLADEN_<xsl:value-of select="@name" /> 	"<!-- <xsl:copy-of select="$myRaum" />_ --><xsl:value-of select="substring-after(@name,'__')" />" &lt;rollershutter&gt; (Rolladen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=rolladen&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:50000:JS(getRollershutterState.js)]", autoupdate="false"}
//Dimmer ROLLADEN_<xsl:value-of select="@name" /> 	"<!-- <xsl:copy-of select="$myRaum" />_ --><xsl:value-of select="substring-after(@name,'__')" /> [%d %%]" &lt;rollershutter&gt; (Rolladen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=rolladen&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:50000:JS(getRollershutterState.js)]", autoupdate="false"}
</xsl:template>


</xsl:stylesheet>

