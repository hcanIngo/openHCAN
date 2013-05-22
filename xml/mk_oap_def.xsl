<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

<xsl:template match="/classes">
#ifndef OAP_DEF_H
#define OAP_DEF_H

#ifndef true
#define true 1
#define false 0
#endif

#define OAP_TYPE_BOOL        1
#define OAP_TYPE_INT         2
#define OAP_TYPE_TIME        3

#define OAP_RW               0
#define OAP_PERSISTENT       1
#define OAP_ARRAY            2

#define OAP_FLAGS_RW_PERS_ARRAY    7
#define OAP_FLAGS_RW_PERS          3

// ------- AUTOMATICLY GENERATED FROM mk_oap_def.xsl -----------------

		<xsl:apply-templates select="class" />

#endif
</xsl:template>

<xsl:template match="class">

// class: <xsl:value-of select="@name"/>
#define OAP_CLASS_<xsl:value-of select="@name"/> SED_DELETE <xsl:value-of select="@id"/>

	<xsl:apply-templates select="field" />
	<xsl:apply-templates select="array" />

</xsl:template>

<xsl:template match="field">
#define OAP_FIELD_<xsl:value-of select="../@name"/>_<xsl:value-of select="@name"/> SED_DELETE <xsl:value-of select="@id"/>
</xsl:template>

</xsl:stylesheet>

