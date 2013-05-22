<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/hcan-protocols">
// ------- AUTOMATICLY GENERATED FROM mk_dump_data_h.xsl -----------------


#include &lt;string&gt;
#include &lt;vector&gt; 
#include &lt;frame_message_description.h&gt; 

using namespace std;

vector&lt;frame_message_description_t&gt; frame_message_description;

void init_frame_message_description()
{

		<xsl:apply-templates select="protocol" />
};

	</xsl:template>

	<xsl:template match="protocol">
		<xsl:apply-templates select="service" />

	</xsl:template>

	<xsl:template match="service">
		<xsl:apply-templates select="message" />
	</xsl:template>

	<xsl:template match="message">
{
		frame_message_description_t msg;
		msg.proto = <xsl:value-of select="../../@id"/>;
		msg.service = <xsl:value-of select="../@id"/>;
		msg.service_name = string("<xsl:value-of select="../@name"/>");
		msg.command = <xsl:value-of select="@id"/>;
		msg.command_name = string("<xsl:value-of select="@name"/>");
		msg.details = string("<xsl:value-of select="@details"/>");

		<xsl:for-each select="*">
		msg.param_names.push_back("<xsl:value-of select="@name" />");
		</xsl:for-each>
		frame_message_description.push_back (msg);
};

	</xsl:template>

</xsl:stylesheet>

