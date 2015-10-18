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
		<xsl:apply-templates select="bereich/board/powerport"/> <!-- Z.B. Musikanlage im EG -->
</xsl:template>

<xsl:template match="powerport">
		<xsl:choose>
		<xsl:when test="(@typ = 'lampe')">
Switch LAMPE_<xsl:value-of select="@name" /> 	/*"<xsl:value-of select="substring-after(@name,'__')" /> [MAP(de.map):%s]"*/ (<xsl:value-of select="substring-before(@name,'__')" />, Lampen) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=lampe&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:60000:JS(getSwitchState.js)]", autoupdate="false" }
		</xsl:when>
		<xsl:otherwise>
Switch SONSTIGE_<xsl:value-of select="@name" /> 	/*"<xsl:value-of select="substring-after(@name,'__')" /> [MAP(de.map):%s]" &lt;socket&gt;*/ (<xsl:value-of select="substring-before(@name,'__')" />, Sonstige) { http="&lt;[http://C1612server/?callback=myjp&amp;cmd=&amp;d=sonstige&amp;setid=&amp;qid=<xsl:value-of select="@gruppe" />:70000:JS(getSwitchState.js)]", autoupdate="false" }	
		</xsl:otherwise>
		</xsl:choose>
</xsl:template>


</xsl:stylesheet>

