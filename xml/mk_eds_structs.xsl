<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/eds-definitions">
#ifndef EDS_DEFINITIONS_H
#define EDS_DEFINITIONS_H

// ------- AUTOMATICLY GENERATED FROM mk_eds_structs.xsl ------------------

#include &lt;inttypes.h&gt;

		<xsl:apply-templates select="block" />

#endif
	</xsl:template>

	<xsl:template match="block">

	typedef struct
	{
		<xsl:apply-templates select="field" />
	} eds_<xsl:value-of select="@name" />_block_t;

	#define EDS_<xsl:value-of select="@name" />_BLOCK_ID <xsl:value-of select="@type" />


	</xsl:template>

	<xsl:template match="field">

	<xsl:value-of select="@datatype"/> SED_DELETE
	<xsl:value-of select="@name" />
	<xsl:if test="@size">[<xsl:value-of select="@size"/>]</xsl:if>;

	</xsl:template>

</xsl:stylesheet>

