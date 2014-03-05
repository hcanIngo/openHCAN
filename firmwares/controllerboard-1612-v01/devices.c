#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <canix/eds.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "devices.h"
#include "log.h"

uint8_t *pdevice_data[MAX_PDEVICE_DATA];
uint8_t device_data[MAX_DEVICE_DATA];
uint16_t device_data_ram_usage;

void devices_init(void)
{
	size_t i;

	for (i = 0; i < MAX_PDEVICE_DATA; i++)
		pdevice_data[i] = (uint8_t *) 0;
	for (i = 0; i < sizeof(device_data); i++)
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

	/*
	for (it = EDS_DATA_START; it != 0;
			it = eds_find_next_block_between(it, 1, 255)) */
	
	// Bugix: /prj/tickets/TID0938/Log
	EDS_foreach_block_between(it,1,255)
	{
		uint8_t data_size = 0;
		uint8_t config_size = 0;
		uint8_t *p;

		switch (EDS_BLOCK_TYPE(it))
		{
			case EDS_dcf77_receiver_BLOCK_ID: 
				config_size = sizeof (eds_dcf77_receiver_block_t);
				data_size = sizeof (device_data_dcf77_receiver);
				break;
			case EDS_dunstabzugport_BLOCK_ID: 
				config_size = sizeof (eds_dunstabzugport_block_t);
				data_size = sizeof (device_data_dunstabzugport);
				break;
			case EDS_heizung_BLOCK_ID: 
				config_size = sizeof (eds_heizung_block_t);
				data_size = sizeof (device_data_heizung);
				break;
			case EDS_lichtzone_BLOCK_ID: 
				config_size = sizeof (eds_lichtzone_block_t);
				data_size = sizeof (device_data_lichtzone);
				break;
			case EDS_helligkeitssensor_BLOCK_ID:
				config_size = sizeof (eds_helligkeitssensor_block_t);
				data_size = sizeof (device_data_helligkeitssensor);
				break;
			case EDS_poti_BLOCK_ID:
				config_size = sizeof (eds_poti_block_t);
				data_size = sizeof (device_data_poti);
				break;
			case EDS_multitaster_BLOCK_ID:
				config_size = sizeof (eds_multitaster_block_t);
				data_size = sizeof (device_data_multitaster);
				break;
			case EDS_powerport_BLOCK_ID: 
				config_size = sizeof (eds_powerport_block_t);
				data_size = sizeof (device_data_powerport);
				break;
			case EDS_reedkontakt_BLOCK_ID: 
				config_size = sizeof (eds_reedkontakt_block_t);
				data_size = sizeof (device_data_reedkontakt);
				break;
			case EDS_rolladen_BLOCK_ID: 
				config_size = sizeof (eds_rolladen_block_t);
				data_size = sizeof (device_data_rolladen);
				break;
			case EDS_rolladenSchlitzpos_BLOCK_ID:
				config_size = sizeof (eds_rolladenSchlitzpos_block_t);
				data_size = sizeof (device_data_rolladenSchlitzpos);
				break;
			case EDS_rolladenAutomat_BLOCK_ID:
				config_size = sizeof (eds_rolladenAutomat_block_t);
				data_size = sizeof (device_data_rolladenAutomat);
				break;
			case EDS_powerportAutomat_BLOCK_ID:
				config_size = sizeof (eds_powerportAutomat_block_t);
				data_size = sizeof (device_data_powerportAutomat);
				break;
			case EDS_ports_BLOCK_ID:
				config_size = sizeof (eds_ports_block_t);
				data_size = sizeof (device_data_ports);
				break;
			case EDS_schalter_BLOCK_ID:
				config_size = sizeof (eds_schalter_block_t);
				data_size = sizeof (device_data_schalter);
				break;
			case EDS_zentralheizungspumpe_BLOCK_ID:
				config_size = sizeof (eds_zentralheizungspumpe_block_t);
				data_size = sizeof (device_data_zentralheizungspumpe);
				break;
			case EDS_tastdimmer_BLOCK_ID: 
				config_size = sizeof (eds_tastdimmer_block_t);
				data_size = sizeof (device_data_tastdimmer);
				break;
			case EDS_taster_BLOCK_ID: 
				config_size = sizeof (eds_taster_block_t);
				data_size = sizeof (device_data_taster);
				break;
			case EDS_tempsensor_BLOCK_ID: 
				config_size = sizeof (eds_tempsensor_block_t);
				data_size = sizeof (device_data_tempsensor);
				break;
			case EDS_timeservice_BLOCK_ID: 
				config_size = sizeof (eds_timeservice_block_t);
				data_size = sizeof (device_data_timeservice);
				break;
			case EDS_zeitschaltuhr_BLOCK_ID: 
				config_size = sizeof (eds_zeitschaltuhr_block_t);
				data_size = sizeof (device_data_zeitschaltuhr);
				break;
			case EDS_zeitzone_BLOCK_ID: 
				config_size = sizeof (eds_zeitzone_block_t);
				data_size = sizeof (device_data_zeitzone);
				break;
			default: 
				// Falls wir den Block Typ nicht kennen, dann
				// weiter mit dem naechsten Block...
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
			message.data[3] = (uint16_t)it >> 8;
			message.data[4] = (uint16_t)it & 0xff;
			message.data[5] = EDS_BLOCK_SIZE(it);
			message.data[6] = config_size;
			message.size = 7;

			canix_frame_send_with_prio(&message, HCAN_PRIO_HI);

			canix_syslog_P(SYSLOG_PRIO_CRITICAL, 
					PSTR("EDS config/firmware compat error"));

			// damit der User eine Chance hat, die Config zu editieren,
			// initialisieren wir den RAM, so dass keine Devices geladen
			// sind und starten wie gewohnt:
			devices_init();
			device_data_ram_usage = 0;
			return;
		}

		// Nun weiter mit dem Laden der Config
		p = &device_data[device_data_allocated];
		device_data_allocated += data_size;
		pdevice_data[pdevice_data_allocated++] = p;

		if (pdevice_data_allocated > MAX_DEVICE_DATA)
		{
			// das RAM fuer die Aufnahme der Config ist voll
			// hier wird abgebrochen:
			canix_syslog_P(SYSLOG_PRIO_CRITICAL, 
					PSTR("EDS config RAM overflow"));
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
			case EDS_dcf77_receiver_BLOCK_ID: 
				dcf77_receiver_init( (device_data_dcf77_receiver*) p, it); 
				break;
			case EDS_dunstabzugport_BLOCK_ID: 
				dunstabzugport_init( (device_data_dunstabzugport*) p, it); 
				break;
			case EDS_heizung_BLOCK_ID: 
				heizung_init( (device_data_heizung*) p, it); 
				break;
			case EDS_helligkeitssensor_BLOCK_ID:
				helligkeitssensor_init( (device_data_helligkeitssensor*) p, it);
				break;
			case EDS_multitaster_BLOCK_ID:
				multitaster_init( (device_data_multitaster*) p, it);
				break;
			case EDS_poti_BLOCK_ID:
				poti_init( (device_data_poti*) p, it);
				break;
			case EDS_ports_BLOCK_ID:
				ports_init( (device_data_ports*) p, it);
				break;
			case EDS_reedkontakt_BLOCK_ID: 
				reedkontakt_init( (device_data_reedkontakt*) p, it); 
				break;
			case EDS_rolladen_BLOCK_ID: 
				rolladen_init( (device_data_rolladen*) p, it); 
				break;
			case EDS_rolladenSchlitzpos_BLOCK_ID:
				rolladenSchlitzpos_init( (device_data_rolladenSchlitzpos*) p, it);
				break;
			case EDS_rolladenAutomat_BLOCK_ID:
				rolladenAutomat_init( (device_data_rolladenAutomat*) p, it);
				break;
			case EDS_powerportAutomat_BLOCK_ID:
				powerportAutomat_init( (device_data_powerportAutomat*) p, it);
				break;
			case EDS_zentralheizungspumpe_BLOCK_ID:
				zentralheizungspumpe_init( (device_data_zentralheizungspumpe*) p, it);
				break;
			case EDS_tempsensor_BLOCK_ID: 
				tempsensor_init( (device_data_tempsensor*) p, it); 
				break;
			case EDS_zeitschaltuhr_BLOCK_ID: 
				zeitschaltuhr_init( (device_data_zeitschaltuhr*) p, it); 
				break;
			case EDS_zeitzone_BLOCK_ID: 
				zeitzone_init( (device_data_zeitzone*) p, it); 
				break;
		}
	}

	// die verwendeten Bytes an RAM merken
	device_data_ram_usage = device_data_allocated;
}
