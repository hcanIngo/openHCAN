<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

	<xsl:template match="/Controllerboard-devicelist">

// ------- AUTOMATICLY GENERATED FROM mk_devices_h.xsl -----------------

#ifndef DEVICES_H
#define DEVICES_H

#include &lt;canix/eds.h&gt;
	<xsl:apply-templates select="device" />
// definiert die Groesse der verschiedenen Device-Configs im EEPROM
extern uint8_t device_config_size[];
extern uint8_t device_data_size[];

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32__)
	#define MAX_PDEVICE_DATA         64
	#define MAX_DEVICE_DATA         768
	#define MAX_MEM_CRITICAL_SIZE   256
#elif defined(__AVR_ATmega644P__) // ATmega644p, mit doppelter EEPROM- und SRAM-Groesse:
	#define MAX_PDEVICE_DATA        128
	#define MAX_DEVICE_DATA        2048
	#define MAX_MEM_CRITICAL_SIZE   256
#endif

extern uint8_t *pdevice_data[];
extern uint8_t device_data[];
extern uint16_t device_data_ram_usage;

void devices_load_config(void);
void hauselektrik_callback(const canix_frame *frame);

#endif
	</xsl:template>



	<xsl:template match="device">
#include "devices/<xsl:value-of select="@name" />.h"
	</xsl:template>

</xsl:stylesheet>