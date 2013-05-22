#ifndef DEVICES_H
#define DEVICES_H

#include <canix/eds.h>

#include "dcf77-receiver.h"
#include "dunstabzugport.h"
#include "heizung.h"
#include "lichtzone.h"
#include "multitaster.h"
#include "poti.h"
#include "helligkeitssensor.h"
#include "zentralheizungspumpe.h"
#include "schalter.h"
#include "powerport.h"
#include "reedkontakt.h"
#include "powerportAutomat.h"
#include "rolladenAutomat.h"
#include "rolladen.h"
#include "rolladenSchlitzpos.h"
#include "tastdimmer.h"
#include "taster.h"
#include "tempsensor.h"
#include "timeservice.h"
#include "zeitschaltuhr.h"
#include "zeitzone.h"

// definiert die Groesse der verschiedenen Device-Configs im EEPROM
extern uint8_t device_config_size[];
extern uint8_t device_data_size[];




#ifdef MCU_atmega32
#define MAX_PDEVICE_DATA         64
#define MAX_DEVICE_DATA         768
#define MAX_MEM_CRITICAL_SIZE   256
#elif MCU_atmega644 //mit doppelter EEPROM- und SRAM-Groesse 
	#define MAX_PDEVICE_DATA        128
	#define MAX_DEVICE_DATA        1536
	#define MAX_MEM_CRITICAL_SIZE   512
#endif

extern uint8_t *pdevice_data[];
extern uint8_t device_data[];
extern uint16_t device_data_ram_usage;

extern uint8_t shiftIOExt;

void devices_load_config(void);

#endif
