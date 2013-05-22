<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/eds-definitions">

// ------- AUTOMATICLY GENERATED FROM mk_eds_telican_desc.xsl ---

#include &lt;inttypes.h&gt;
#include &lt;eds_desc.h&gt;

vector&lt;eds_block_desc_t&gt; eds_get_block_descriptions()
{
	vector&lt;eds_block_desc_t&gt; blocks;
	<xsl:apply-templates select="block" />

	return blocks;
}

	</xsl:template>

	<xsl:template match="block">

	{
		eds_block_desc_t block(
			<xsl:value-of select="@type" />,
			"<xsl:value-of select="@name" />",
			<xsl:value-of select="@size" />);
		<xsl:apply-templates select="field" />
		blocks.push_back(block);
	}

	</xsl:template>

	<xsl:template match="field">
		block.fields.push_back(
			eds_block_field_desc_t("<xsl:value-of select="@datatype"/>",
			"<xsl:value-of select="@name" />",
			<xsl:value-of select="@pos" />

			<xsl:if test="@size">
			,<xsl:value-of select="@size" />
			</xsl:if>
			));
	</xsl:template>

</xsl:stylesheet>

