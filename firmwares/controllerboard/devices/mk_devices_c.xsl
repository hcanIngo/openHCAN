<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/Controllerboard-devicelist">

// ------- AUTOMATICLY GENERATED FROM mk_devices_c.xsl -----------------

#include &lt;canix/canix.h&gt;
#include &lt;canix/led.h&gt;
#include &lt;canix/tools.h&gt;
#include &lt;canix/rtc.h&gt;
#include &lt;canix/syslog.h&gt;
#include &lt;canix/eds.h&gt;
#include &lt;hcan.h&gt;

#include &lt;avr/io.h&gt;
#include &lt;avr/interrupt.h&gt;
#include &lt;avr/eeprom.h&gt;
#include &lt;avr/wdt.h&gt;

#include &quot;devices.h&quot;

uint8_t *pdevice_data[MAX_PDEVICE_DATA];
uint8_t device_data[MAX_DEVICE_DATA];
uint16_t device_data_ram_usage;

void devices_init(void)
{
	size_t i;

	for (i = 0; i &lt; MAX_PDEVICE_DATA; i++)
		pdevice_data[i] = (uint8_t *) 0;
	for (i = 0; i &lt; sizeof(device_data); i++)
		device_data[i] = 0;
	
	portsDeviceCreated = false;
}

void devices_load_config(void)
{
	// Liest den eeprom aus und laed die Configs
	// Baut die Instanz-Daten-Strukturen auf
	//

	size_t pdevice_data_allocated = 0;
	size_t device_data_allocated = 0;

	devices_init();

	eds_block_p it;
	unknown_device_found = 0;

	EDS_foreach_block_between(it,1,255)
	{
		uint8_t data_size = 0;
		uint8_t config_size = 0;
		uint8_t *p;

		switch (EDS_BLOCK_TYPE(it))
		{
	<xsl:for-each select="device">
			case EDS_<xsl:value-of select="@name" />_BLOCK_ID: 
				config_size = sizeof (eds_<xsl:value-of select="@name" />_block_t);
				data_size = sizeof (device_data_<xsl:value-of select="@name" />);
				break;
	</xsl:for-each>
	case EDS_dcf77_receiver_BLOCK_ID:
	    config_size = sizeof(eds_dcf77_receiver_block_t);
	    data_size = sizeof(device_data_dcf77_receiver);
	    break;
	case EDS_timeservice_BLOCK_ID:
	    config_size = sizeof(eds_timeservice_block_t);
	    data_size = sizeof(device_data_timeservice);
	    break;
			default: 
				// Falls wir den Block Typ (ausser 255) nicht kennen, dann
				// geben wir eine Syslog Meldung aus
				// laden aber weiter
				if (EDS_BLOCK_TYPE(it) != 255)
				{
					canix_syslog_P(SYSLOG_PRIO_ERROR,
						PSTR(&quot;EDS_BLOCK_TYPE unknown:%d EDS Addr:%d&quot;), EDS_BLOCK_TYPE(it), it);
					unknown_device_found = 1;
				}
				continue;

		}

		/* EDS/Firmware Compatibility-Check:
		 *
		 * Hier wird geprueft, ob die Config Blocksize, so wie sie
		 * ueber die struct der Firmware bekannt ist, auch im EDS als
		 * Blockgroesse vermerkt ist. Sind diese beiden Groessen 
		 * unterschiedlich, so besteht die Gefahr, das die Config und die
		 * Firmware zueinander inkompatibel sind. Es wird eine Meldung
		 * versandt und geresettet.
		 */
		if (config_size != EDS_BLOCK_SIZE(it))
		{
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_SYSLOG;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_SLS;
			message.data[1] = HCAN_SLS_FIRMWARE_CONFIG_COMPAT_ERROR;
			message.data[2] = EDS_BLOCK_TYPE(it);
			message.data[3] = (uint16_t)it &gt;&gt; 8;
			message.data[4] = (uint16_t)it &amp; 0xff;
			message.data[5] = EDS_BLOCK_SIZE(it);
			message.data[6] = config_size;
			message.size = 7;

			canix_frame_send_with_prio(&amp;message, HCAN_PRIO_HI);

			canix_syslog_P(SYSLOG_PRIO_CRITICAL, 
					PSTR(&quot;EDS config/firmware compat error&quot;));

			// damit der User eine Chance hat, die Config zu editieren,
			// initialisieren wir den RAM, so dass keine Devices geladen
			// sind und starten wie gewohnt:
			devices_init();
			device_data_ram_usage = 0;
			return;
		}

		// Nun weiter mit dem Laden der Config
		p = &amp;device_data[device_data_allocated];
		device_data_allocated += data_size;
		pdevice_data[pdevice_data_allocated++] = p;

		if (pdevice_data_allocated &gt; MAX_DEVICE_DATA)
		{
			// das RAM fuer die Aufnahme der Config ist voll
			// hier wird abgebrochen:
			canix_syslog_P(SYSLOG_PRIO_CRITICAL, 
					PSTR(&quot;EDS config RAM overflow&quot;));
			return;
		}

		// Daten von Config ins RAM kopieren:
		//
		// An Position 0 steht der Typ, daher lassen wir diese
		// Position aus (p + 1)
		//
		// it zeigt auf den Block; nach 2 Bytes beginnen die Nutzdaten;
		// daher it + 2
		eeprom_read_block(p + 1, it + 2, config_size);

		// Block Typ ins RAM kopieren
		p[0] = EDS_BLOCK_TYPE(it);

		switch (EDS_BLOCK_TYPE(it))
		{
	<xsl:for-each select="device">
			case EDS_<xsl:value-of select="@name" />_BLOCK_ID:
				<xsl:value-of select="@name" />_init( (device_data_<xsl:value-of select="@name" />*) p, it);
				break;
	</xsl:for-each>
					case EDS_dcf77_receiver_BLOCK_ID:
		    			dcf77_receiver_init((device_data_dcf77_receiver *) p, it);
		    			break;

		}
	}

	// die verwendeten Bytes an RAM merken
	device_data_ram_usage = device_data_allocated;
}

void hauselektrik_callback(const canix_frame *frame)
{
	uint8_t i;

	// Dieser Handler wird fuer alle Destination Adressen ausgefuehrt
	// daher muss gefiltert werden, was uns betrifft und was nicht:
	if ( (frame->data[0] != HCAN_SRV_HES))
		// Diese Message ist nicht interessant, daher ignorieren
		return;

	if ((frame->dst != canix_selfaddr()) &amp;&amp; (
		(frame-&gt;dst != HCAN_MULTICAST_CONTROL) &amp;&amp;
		(frame-&gt;dst != HCAN_MULTICAST_INFO)))
		return; // Diese Message ist nicht interessant, daher ignorieren

	for (i = 0; i &lt; MAX_PDEVICE_DATA; i++)
	{
		uint8_t *p = (uint8_t *) pdevice_data[i];
		if (p)
		{
			switch (*p)
			{
	<xsl:for-each select="device">
				case EDS_<xsl:value-of select="@name" />_BLOCK_ID:
					<xsl:value-of select="@name" />_can_callback( (device_data_<xsl:value-of select="@name" />*) p, frame);
					break;
	</xsl:for-each>
			}
		}
	}
}

	</xsl:template>


</xsl:stylesheet>
