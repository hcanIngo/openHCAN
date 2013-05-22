<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/eds-definitions">
-- AUTOMATICLY GENERATED FROM mk_eds_sql.xsl

		<xsl:apply-templates select="block" />

	</xsl:template>

	<xsl:template match="block">

drop table <xsl:value-of select="@name" />;
create table <xsl:value-of select="@name" />
(
	id serial primary key
	, id_board integer references board
	<xsl:apply-templates select="field" />
);
	</xsl:template>

	<xsl:template match="field">

,<xsl:value-of select="@name" /> SED_DELETE <xsl:value-of select="@datatype"/>

	</xsl:template>

</xsl:stylesheet>

