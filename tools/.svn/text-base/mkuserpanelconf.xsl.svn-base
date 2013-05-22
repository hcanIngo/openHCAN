<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

<!--
	Sehr gute Uebersicht zu preceding/following:
	http://infohost.nmt.edu/tcc/help/pubs/xslt/axis-sect.html
-->

<xsl:output method="text" />

<xsl:template match="/bedienfeld">
<xsl:apply-templates select="userpanel_shortcuts" />
<xsl:apply-templates select="lcd_light_settings" />
<xsl:apply-templates select="zeitzone" />
<xsl:apply-templates select="page" />
</xsl:template>

<xsl:template match="userpanel_shortcuts">
create userpanel_shortcuts
<xsl:value-of select="text()"/>
exit
</xsl:template>

<xsl:template match="lcd_light_settings">
create lcd_light_settings
<xsl:value-of select="text()"/>
exit
</xsl:template>

<xsl:template match="zeitzone">
create zeitzone
<xsl:value-of select="text()"/>
exit
</xsl:template>

<xsl:template match="page">
create <xsl:value-of select="@type" />
  set page_id <xsl:value-of select="@id"/>
<xsl:choose>
	<xsl:when test="parent::page">
  set parent_id <xsl:value-of select="parent::page/@id"/>
	</xsl:when>
	<xsl:when test="not(parent::page)">
  set parent_id 0
	</xsl:when>
</xsl:choose>
<xsl:choose>
	<xsl:when test="preceding-sibling::page">
  set prev_id <xsl:value-of select="preceding-sibling::page[1]/@id"/>
	</xsl:when>
	<xsl:when test="not(preceding-sibling::page)">
  set prev_id 0
	</xsl:when>
</xsl:choose>
<xsl:choose>
	<xsl:when test="following-sibling::page">
  set next_id <xsl:value-of select="following-sibling::page/@id"/>
	</xsl:when>
	<xsl:when test="not(following-sibling::page)">
  set next_id 0
	</xsl:when>
</xsl:choose>
<xsl:value-of select="text()"/>
exit
<xsl:apply-templates select="page" />
</xsl:template>

</xsl:stylesheet>

