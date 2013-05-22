<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

<xsl:template match="/hcan-protocols">
#ifndef HCAN_DEF_H
#define HCAN_DEF_H

// ------- AUTOMATICLY GENERATED FROM mk_hcan-def_h.xsl -----------------

		<xsl:apply-templates select="protocol" />

#endif
</xsl:template>

	<xsl:template match="protocol">
		<xsl:apply-templates select="service" />

	</xsl:template>

	<xsl:template match="service">
// Service: <xsl:value-of select="@name"/>
<xsl:apply-templates select="message" />
	</xsl:template>

	<xsl:template match="message">

// <xsl:value-of select="@description"/>
<xsl:apply-templates select="param" />
#define HCAN_<xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/> SED_DELETE <xsl:value-of select="@id"/>
	</xsl:template>

	<xsl:template match="param">
//  <xsl:value-of select="@name" /> : <xsl:value-of select="@description" />
	</xsl:template>

</xsl:stylesheet>

