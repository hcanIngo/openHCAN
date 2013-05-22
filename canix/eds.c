#include <eds.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <canix/syslog.h>
#include <hcan.h>
#include <hcan-def.h>
#include <string.h>
#include <stdio.h>

#define EDS_MAGIC1 0xe4
#define EDS_MAGIC2 0x7d

void eds_format(void)
{
	uint8_t *p;
	uint8_t size;

	p = EDS_START;

	eeprom_write_byte(p++,EDS_MAGIC1);
	eeprom_write_byte(p++,EDS_MAGIC2);

	// Jetzt den freien Bereich mit leeren Bloecken fuellen:
	
	while (p < (EDS_END - 2))
	{
		uint8_t *block = p;

		// Block Type: 0 (leer)
		eeprom_write_byte(p++,0);

		// Herausfinden, wieviel Platz noch da ist:

		size = ((EDS_END - p) > 128) ? 128 : EDS_END - p;
		eeprom_write_byte(p++,size);

		canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("EDS: format: p = %d, size=%d"),
				(uint16_t)block,size);

		p += size;
	}
}

uint8_t *eds_next_block(uint8_t *p)
{
	return (p + 2 + EDS_BLOCK_SIZE(p));
}

uint8_t eds_init(void)
{
	// Testen, ob 
	//
	
	uint8_t *p;

	p = (uint8_t *) EDS_START;

	if (eeprom_read_byte((uint8_t *)p++) == EDS_MAGIC1 &&
			eeprom_read_byte((uint8_t *)p) == EDS_MAGIC2)
	{
		canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("EDS: data space is formated."));
		return EDS_OK;
	}

	canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("EDS: data space not formated"));
	canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("EDS: formating..."));
	eds_format();
	canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("EDS: done."));

	return EDS_NOT_FORMATED;
}

uint8_t *eds_alloc(uint8_t size)
{
	uint8_t *p;

	p = EDS_DATA_START;

	// Nach einem Typ 0 Block suchen:
	for (p = EDS_DATA_START; p < EDS_END; p = eds_next_block(p))
	{
		if ((EDS_BLOCK_TYPE(p) == 0) && (EDS_BLOCK_SIZE(p) >= size))
		{
			if (EDS_BLOCK_SIZE(p) == size)
			{
				return p;
			}
			if (EDS_BLOCK_SIZE(p) >= size + 2)
			{
				// neuen, leeren Block ans Ende schreiben
				eeprom_write_byte(p + 2 + size,0);
				eeprom_write_byte(p + 2 + size + 1, 
						EDS_BLOCK_SIZE(p) - size - 2);

				// allozierten Block kuerzen:
				eeprom_write_byte(p + 1, size);
				return p;
			}
		}
	}

	return 0;
}

uint8_t eds_free(uint8_t *block)
{
	uint8_t *p;

	for (p = EDS_DATA_START; p < EDS_END; p = eds_next_block(p))
	{
		if ((uint16_t) p == (uint16_t) block)
		{
			// ok, der Zeiger scheint zu stimmen, d.h. es ist ein
			// valider Block
			
			EDS_SET_BLOCK_TYPE(p, 0);
			return EDS_OK;

		}
	}
	return EDS_INVALID_POINTER;
}

void eds_defragment(void)
{
	uint8_t *p;
	uint8_t *last_free;
	//uint8_t runns = 0;

	last_free = 0;

	for (p = EDS_DATA_START; p < EDS_END; p = eds_next_block(p))
	{
		// wenn der letzte Block frei war und dieser es wieder ist:
		if (last_free && EDS_BLOCK_IS_FREE(p))
		{
			uint16_t last_size,size;

			last_size = EDS_BLOCK_SIZE(last_free);
			size = EDS_BLOCK_SIZE(p);

			// wenn die Gesamtgroesse nicht zu gross ist, dann zusammen-
			// fassen
			if ((last_size + 2 + size) <= 255)
			{
				EDS_SET_BLOCK_SIZE(last_free, last_size + 2 + size);
				p = last_free;
			}
		}

		// den aktuellen Block als freien Block merken, falls
		// er frei ist
		if (EDS_BLOCK_IS_FREE(p))
			last_free = p;
		else
			last_free = 0;
	}
}

void eds_read_string(uint8_t *block, char *s, uint8_t len)
{
	uint8_t i;

	for (i = 0; (i < len-1); i++)
	{
		s[i] = eeprom_read_byte(block + i);
	}
	s[i] = 0;
}

eds_block_p eds_find_next_block(eds_block_p p, uint8_t type)
{
	if (p == 0)
	{
		p = EDS_DATA_START;
		if (EDS_BLOCK_TYPE(p) == type)
			return p;
	}

	while (p < EDS_END)
	{
		p = eds_next_block(p);
		if (EDS_BLOCK_TYPE(p) == type)
			return p;
	}
	return 0;
}

eds_block_p eds_find_next_block_between(eds_block_p p, 
		uint8_t type1, uint8_t type2)
{
	if (p == 0)
	{
		p = EDS_DATA_START;
		if ((EDS_BLOCK_TYPE(p) >= type1) && (EDS_BLOCK_TYPE(p) <= type2))
			return p;
	}

	while (p < EDS_END)
	{
		p = eds_next_block(p);
		if ((EDS_BLOCK_TYPE(p) >= type1) && (EDS_BLOCK_TYPE(p) <= type2))
			return p;
	}
	return 0;
}

void eds_can_callback(const canix_frame *frame)
{
	eds_block_p h1,h2;
	uint8_t index;
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_EDS;

	switch (frame->data[1])
	{
		case HCAN_EDS_GET_NEXT_BLOCK :
			h1 = (eds_block_p) ((frame->data[2] << 8) | (frame->data[3]));

			// Falls wir "0" bekommen haben, Data Start zuweisen
			if (h1 == (eds_block_p) 0)
				h2 = EDS_DATA_START;
			else
				h2 = eds_next_block(h1);

			// Falls wir am Ende sind, dann "0" zurueckliefern
			if (h2 >= EDS_END)
				h2 = 0;

			answer.data[1] = HCAN_EDS_GET_NEXT_BLOCK_REPLAY;
			answer.data[2] = ((uint16_t)h2 >> 8);
			answer.data[3] = ((uint16_t)h2);
			answer.data[4] = EDS_BLOCK_TYPE(h2);
			answer.data[5] = EDS_BLOCK_SIZE(h2);
			answer.size = 6;
			canix_frame_send(&answer);
			break;

		case HCAN_EDS_ALLOCATE_BLOCK :
			h2 = eds_alloc(frame->data[3]);
			EDS_SET_BLOCK_TYPE(h2,frame->data[2]);
			answer.data[1] = HCAN_EDS_ALLOCATE_BLOCK_REPLAY;
			answer.data[2] = ((uint16_t)h2 >> 8);
			answer.data[3] = ((uint16_t)h2);
			answer.size = 4;
			canix_frame_send(&answer);
			break;

		case HCAN_EDS_FREE_BLOCK:
			h1 = (eds_block_p) ((frame->data[2] << 8) | (frame->data[3]));
			uint8_t res = eds_free(h1);
			answer.data[1] = HCAN_EDS_FREE_BLOCK_REPLAY;
			answer.data[2] = res;
			answer.size = 3;
			canix_frame_send(&answer);
			break;
			
		case HCAN_EDS_DEFRAGMENT:
			eds_defragment();
			break;

		case HCAN_EDS_READ_FROM_BLOCK :
			h1 = (eds_block_p) ((frame->data[2] << 8) | (frame->data[3]));
			index = frame->data[4];

			answer.data[1] = HCAN_EDS_READ_FROM_BLOCK_REPLAY;
			answer.data[2] = eeprom_read_byte(h1 + index);
			answer.data[3] = eeprom_read_byte(h1 + index + 1);
			answer.data[4] = eeprom_read_byte(h1 + index + 2);
			answer.data[5] = eeprom_read_byte(h1 + index + 3);
			answer.size = 6;
			canix_frame_send(&answer);
			break;

		case HCAN_EDS_WRITE_TO_BLOCK :
			h1 = (eds_block_p) ((frame->data[2] << 8) | (frame->data[3]));
			index = frame->data[4];
			eeprom_write_byte(h1 + index, frame->data[5]);

			answer.data[1] = HCAN_EDS_WRITE_TO_BLOCK_REPLAY;
			answer.size = 2;
			canix_frame_send(&answer);
			break;

		case HCAN_EDS_FORMAT:
			eds_format();
			break;
	};
}
