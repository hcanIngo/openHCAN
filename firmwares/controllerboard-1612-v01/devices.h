

// ------- AUTOMATICLY GENERATED FROM mk_devices_h.xsl -----------------

#ifndef DEVICES_H
#define DEVICES_H

#include <canix/eds.h>

#include "devices/analogComparator.h"

#include "devices/heizung.h"

#include "devices/helligkeitssensor.h"

#include "devices/lichtzone.h"

#include "devices/multitaster.h"

#include "devices/ports.h"

#include "devices/poti.h"

#include "devices/powerportAutomat.h"

#include "devices/powerport.h"

#include "devices/reedkontakt.h"

#include "devices/rolladenAutomat.h"

#include "devices/rolladen.h"

#include "devices/rolladenSchlitzpos.h"

#include "devices/schalter.h"

#include "devices/taster.h"

#include "devices/tempsensor.h"

#include "devices/zeitschaltuhr.h"

#include "devices/zeitzone.h"

#include "devices/zentralheizungspumpe.h"

// definiert die Groesse der verschiedenen Device-Configs im EEPROM
extern uint8_t  device_config_size[];
extern uint8_t  device_data_size[];


#ifdef MCU_atmega32
#define MAX_PDEVICE_DATA         64
#define MAX_DEVICE_DATA         768
#define MAX_MEM_CRITICAL_SIZE   256
#elif MCU_atmega644		// ATmega644p, mit doppelter EEPROM- und
				// SRAM-Groesse:
#define MAX_PDEVICE_DATA        128
#define MAX_DEVICE_DATA        2048
#define MAX_MEM_CRITICAL_SIZE   256
#endif

extern uint8_t *pdevice_data[];
extern uint8_t  device_data[];
extern uint16_t device_data_ram_usage;

void            devices_load_config(void);
void            hauselektrik_callback(const canix_frame * frame);

#endif
