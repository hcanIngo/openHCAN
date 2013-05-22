<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/hcan-protocols">
// ------- AUTOMATICLY GENERATED FROM mk_transport_cc.xsl -----------------

#include &lt;libhcan++/frame.h&gt;
#include &lt;assert.h&gt;
#include &lt;hcan_transport.h&gt;

using namespace hcan;

hcan_transport::hcan_transport(const in_addr_t hcand_ip) :
	m_hcand_ip(hcand_ip)
{
}

hcan_transport::~hcan_transport()
{
}

		<xsl:apply-templates select="protocol" />

	</xsl:template>

	<xsl:template match="protocol">
		<xsl:apply-templates select="service" />

	</xsl:template>

	<xsl:template match="service">
		<xsl:apply-templates select="message" />
	</xsl:template>

	<xsl:template match="message">

	/**
	 * <xsl:value-of select="@description"/>
	 * 
	 * <xsl:for-each select="*">
	 * @param <xsl:value-of select="@name" /> : <xsl:value-of select="@description" />
	 *
	 * </xsl:for-each>
	 */
	void hcan_transport::send_<xsl:value-of select="@name"/>(
		const uint16_t src,
		const uint16_t dst,
		<xsl:for-each select="*">
			uint8_t <xsl:value-of select="@name" />,
		</xsl:for-each>
	)
	{
		uint8_t data[] = {
			<xsl:value-of select="../@id"/>,
			<xsl:value-of select="@id"/>,
			<xsl:for-each select="*">
				<xsl:value-of select="@name" />,
			</xsl:for-each> };
		assert(8 >= sizeof(data));

		frame f(src, dst, <xsl:value-of select="../../@id"/>, data, 
			sizeof(data));
		f.write_to(socket(), m_hcand_ip);
	}

	/**
	 * <xsl:value-of select="@description"/>
	 * 
	 * <xsl:for-each select="*">
	 * @param <xsl:value-of select="@name" /> : <xsl:value-of select="@description" />
	 *
	 * </xsl:for-each>
	 */
	void hcan_transport::recv_<xsl:value-of select="@name"/>(
		const uint16_t src,
		const uint16_t dst,
		<xsl:for-each select="*">
			uint8_t *<xsl:value-of select="@name" />,
		</xsl:for-each>
	)
	{
		uint8_t data[] = {
			<xsl:value-of select="../@id"/>,
			<xsl:value-of select="@id"/>,
			<xsl:for-each select="*">
				0,
			</xsl:for-each> };

		frame f = recv_frame(dst);

		if (src)
			if (f.src() != src)
				throw transport_error 
					("unexpected packet received: src is wrong");
		if (f.size() != sizeof(data))
			throw transport_error ("unexpected size in packet received");
		if (f.data(0) != <xsl:value-of select="../@id"/>)
			throw transport_error ("unexpected packet received: service id is wrong");
		if (f.data(1) != <xsl:value-of select="@id"/>)
			throw transport_error ("unexpected packet received: commmand id is wrong");

		<xsl:for-each select="*">
			*<xsl:value-of select="@name" /> = f.data(
				<xsl:value-of select="position() + 1"/>);
		</xsl:for-each>
	}

	</xsl:template>

</xsl:stylesheet>

