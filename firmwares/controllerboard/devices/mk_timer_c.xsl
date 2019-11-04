<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/Controllerboard-devicelist">

// ------- AUTOMATICLY GENERATED FROM mk_timer_c.xsl -----------------

#include &lt;canix/canix.h&gt;
#include &lt;canix/led.h&gt;
#include &lt;canix/tools.h&gt;
#include &lt;canix/rtc.h&gt;
#include &lt;canix/syslog.h&gt;
#include &lt;canix/mem_check.h&gt;
#include &lt;hcan.h&gt;

#include &lt;avr/io.h&gt;
#include &lt;avr/interrupt.h&gt;
#include &lt;avr/eeprom.h&gt;
#include &lt;avr/wdt.h&gt;

#include &quot;timer.h&quot;
#include &quot;timeservice.h&quot;
#include &quot;dcf77_receiver.h&quot;
#include &quot;devices.h&quot;

volatile uint8_t timer_counter_100th = 0;
volatile uint8_t timer_counter_10th = 0;
volatile uint8_t timer_counter_1 = 0;

volatile uint8_t timer_event_100th = 0;
volatile uint8_t timer_event_10th = 0;
volatile uint8_t timer_event_1 = 0;

// Achtung: RTC Timer Handler werden von einer ISR aufgerufen
// daher hier nichts machen, was wiederum Interrupts ausloesen koennte
void timer_handler(void)
{
	timer_counter_100th++;
	timer_event_100th = 1;

	if (timer_counter_100th == 10)
	{
		// 10msec Takt
		timer_counter_10th++;
		timer_counter_100th = 0;
		timer_event_10th = 1;
	}

	if (timer_counter_10th == 10)
	{
		// 100msec Takt
		timer_counter_1++;
		timer_counter_10th = 0;
		timer_event_1 = 1;
	}
}

// Fuer die timer_handler gilt:
// 1: 1th, 10: 10th, 100: 100th  -Sekundenaufruftakt
void idle_handler(void)
{
	uint8_t i;
	// uint16_t unusedSRAM; //in bytes

	// 10msec Takt:
	if (timer_event_100th)
	{
		// Event Flag zuruecksetzen
		timer_event_100th = 0;

		if (expanderActive) handlerForExpanderUpdate ();
		else if (portsDeviceCreated) configurePorts (); // Ausnahme: EMV-Sicherung

		for (i = 0; i &lt; MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
	<xsl:for-each select="device">
					case EDS_<xsl:value-of select="@name" />_BLOCK_ID:
					<xsl:value-of select="@name" />_timer_handler( (device_data_<xsl:value-of select="@name" />*) p, 100);
					break;
	</xsl:for-each>
					case EDS_dcf77_receiver_BLOCK_ID:
		    			dcf77_receiver_timer_handler((device_data_dcf77_receiver *) p, 100);
		    			break;
				}
			}
		}
	}

	// 100msec Takt
	if (timer_event_10th)
	{
		// Event Flag zuruecksetzen
		timer_event_10th = 0;

		for (i = 0; i &lt; MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
	<xsl:for-each select="device">
					case EDS_<xsl:value-of select="@name" />_BLOCK_ID:
					<xsl:value-of select="@name" />_timer_handler( (device_data_<xsl:value-of select="@name" />*) p, 10);
					break;
	</xsl:for-each>
				}
			}
		}
	}

	// 1sec Takt
	if (timer_event_1)
	{
		// Event Flag zuruecksetzen
		timer_event_1 = 0;

		for (i = 0; i &lt; MAX_PDEVICE_DATA; i++)
		{
			uint8_t *p = pdevice_data[i];
			if (p)
			{
				switch (*p)
				{
	<xsl:for-each select="device">
					case EDS_<xsl:value-of select="@name" />_BLOCK_ID:
						<xsl:value-of select="@name" />_timer_handler( (device_data_<xsl:value-of select="@name" />*) p, 1);
						break;
	</xsl:for-each>
					case EDS_timeservice_BLOCK_ID:
		    			timeservice_timer_handler((device_data_timeservice *) p, 1);
		    			break;
					case EDS_dcf77_receiver_BLOCK_ID:
		    			dcf77_receiver_timer_handler((device_data_dcf77_receiver *) p, 1);
		    			break;
				}
			}
		}

		if (unknown_device_found > 0) { // wenn ein unbekanntest Device gefunden wurde, ist die Variable 1
			unknown_device_found++; // dann zaehlen wir die Sek. hoch ...
		}
		if(unknown_device_found == 10) { // ... und nach 10 Sek. kommt eine Syslogmeldung
			canix_syslog_P(SYSLOG_PRIO_ERROR,
				PSTR(&quot;unknown EDS_BLOCK_TYPE found. See Boot MSG&quot;));
			unknown_device_found = 1; // und im naechsten Durchlauf wieder hochzaehlen bis 10 ...
		}

		/* vermutlich nicht ganz korrekt und per "EDS config RAM overflow" schon zum Teil abgedeckt:
		unusedSRAM = get_mem_unused();
		if( unusedSRAM &lt; MAX_MEM_CRITICAL_SIZE )
		{
			canix_syslog_P(SYSLOG_PRIO_CRITICAL,
					PSTR("mem_free: %d bytes"), unusedSRAM);
		}*/
	}
}
	</xsl:template>


</xsl:stylesheet>
