<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />

	<xsl:template match="/hcan-protocols">
======HCAN Protokoll Referenz======

**ACHTUNG: Diese Datei wird automatisch generiert! Aenderungen gehen
verloren!**

		<xsl:apply-templates select="protocol" />
	</xsl:template>

	<xsl:template match="protocol">
		<xsl:apply-templates select="service" />
	</xsl:template>

	<xsl:template match="service">
======<xsl:value-of select="@name"/> - <xsl:value-of select="@description"/> (<xsl:value-of select="@id"/>)======
		<xsl:apply-templates select="message" />
	</xsl:template>

	<xsl:template match="message">
**<xsl:value-of select="@name"/> (<xsl:value-of select="@id"/>)**

<xsl:value-of select="@description"/>
<xsl:if test="@details">
<xsl:value-of select="@details"/>
</xsl:if>
<xsl:if test="count(param) > 0">
^Parameter^Beschreibung^<xsl:for-each select="*">
|<xsl:value-of select="@name" />|<xsl:value-of select="@description" />|</xsl:for-each>
</xsl:if>
\\ 
</xsl:template>

</xsl:stylesheet>

