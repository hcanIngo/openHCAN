<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/hcan-protocols">
#ifndef HCAN_TRANSPORT_H
#define HCAN_TRANSPORT_H

// ------- AUTOMATICLY GENERATED FROM mk_transport_h.xsl ------------------

#include &lt;inttypes.h&gt;
#include &lt;frame.h&gt;

namespace hcan
{
	class transport_error : public traceable_error
    {
		public:
			transport_error (const string &amp;s);
	};

	class hcan_transport
	{
		protected:
			in_addr_t m_hcand_ip;
		protected:
			virtual int socket() const = 0;
			virtual frame recv_frame (const uint16_t dst) = 0;
			virtual frame recv_frame_with_timeout(int seconds) = 0;
			hcan_transport(const in_addr_t hcand_ip);
		public:
			virtual ~hcan_transport();

		<xsl:apply-templates select="protocol" />
	};
};

#endif
	</xsl:template>

	<xsl:template match="protocol">
		<xsl:apply-templates select="service" />

	</xsl:template>

	<xsl:template match="service">
		<xsl:apply-templates select="message" />
	</xsl:template>

	<xsl:template match="message">
	void send_<xsl:value-of select="@name"/>(
		const uint16_t src,
		const uint16_t dst,
		<xsl:for-each select="*">
			uint8_t <xsl:value-of select="@name" />,
		</xsl:for-each>
	);
	void recv_<xsl:value-of select="@name"/>(
		const uint16_t src,
		const uint16_t dst,
		<xsl:for-each select="*">
			uint8_t *<xsl:value-of select="@name" />,
		</xsl:for-each>
	);

	</xsl:template>

</xsl:stylesheet>

