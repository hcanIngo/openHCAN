#include "lcdctrl.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <canix/canix.h>
#include <canix/eds.h>
#include <canix/tools.h>
#include <canix/syslog.h>
#include <eds-structs.h>

void lcdctrl_init(void)
{
	DDRD |= (1<<PD4) | (1<<PD5);

	TCCR1A = (1<<WGM10)|    // 8 bit fast PWM, see WGM12 below
		     (1<<COM1A1)|   // set OC1A/B on compare match, clear them at top
			 (1<<COM1B1);
	TCCR1B = (1<<CS10)|     // 1:1 prescale
		     (1<<WGM12) ;  // fast PWM

	OCR1A = 0;
	OCR1B = 0;
}

void lcdctrl_set_backlight(uint8_t amount)
{
	OCR1A = amount;
}

void lcdctrl_set_contrast(uint8_t amount)
{
	OCR1B = amount;
}

void lcdctrl_blink(void)
{
	int j,i;

	for (j = 0; j < 3; j++)
	{
		for (i = 0; i < 10; i++)
		{
			OCR1A = i*10;
			OCR1B = 10;
			canix_sleep_100th(1);
		}
	}

	lcdctrl_set_display_light(LCD_MODE_ACTIVE);
}

void lcdctrl_set_display_light(uint8_t mode)
{
	// mode: 0 = inaktiv (Nacht), 1 = aktiv
	if (mode == LCD_MODE_ACTIVE)
	{
		// mode: aktiv (Display leuchtet hell)
		lcdctrl_set_backlight(LCD_BACKLIGHT_DEFAULT);
		lcdctrl_set_contrast(LCD_CONTRAST_DEFAULT);
	}
	else if (mode == LCD_MODE_INACTIVE)
	{
		// mode: inaktiv (Display leuchtet nur zur Orientierung)
		// 
		// Wenn ein EDS Block lcd_light_settings existiert, so muessen
		// die Werte aus diesem ausgelesen werden:

		eds_block_p it, it2;

		canix_syslog_P(SYSLOG_PRIO_DEBUG,
			PSTR("lcdctrl_set_light: mode=inactive"));

		for (it = EDS_DATA_START; it != 0; 
				it = eds_find_next_block(it, EDS_lcd_light_settings_BLOCK_ID))
		{
			// aha, wir haben das passenden Block gefunden
			// Als naechstes muessen die Zeitzonen geprueft werden;
			// Die Werte der ersten Zeitzone, die passt, werden
			// verwendet.
			//
			eds_lcd_light_settings_block_t light_block;

			if (EDS_BLOCK_TYPE(it) != EDS_lcd_light_settings_BLOCK_ID)
				continue;

			eeprom_read_block(&light_block, (it+2), sizeof(light_block));

			canix_syslog_P(SYSLOG_PRIO_DEBUG,
				PSTR("lcdctrl_set_light: light block found @%d"),it);
			canix_syslog_P(SYSLOG_PRIO_DEBUG,
				PSTR(" zeitzone: %d"), light_block.zeitzone_id);
			canix_syslog_P(SYSLOG_PRIO_DEBUG,
				PSTR(" backlight: %d"), light_block.backlight);
			canix_syslog_P(SYSLOG_PRIO_DEBUG,
				PSTR(" contrast: %d"), light_block.contrast);

			// ueber alle Zeitzonen iterieren
			//if (time_matches(p->
			for (it2 = EDS_DATA_START; it2 != 0; 
					it2 = eds_find_next_block(it2, EDS_zeitzone_BLOCK_ID))
			{
				eds_zeitzone_block_t zz;

				if (EDS_BLOCK_TYPE(it2) != EDS_zeitzone_BLOCK_ID)
					continue;

				eeprom_read_block(&zz, (it2+2), sizeof(zz));

				canix_syslog_P(SYSLOG_PRIO_DEBUG,
					PSTR("lcdctrl_set_light: zeit zone found @%d"),it2);

				if ((light_block.zeitzone_id == zz.gruppe) && 
						time_matches (zz.from_hour, zz.from_minute,
							zz.to_hour, zz.to_minute,
							zz.day_pattern))
				{
					// wir haben die gesuchte Zeitzone gefunden und
					// die aktuelle Uhrzeit passt; also 
					
					lcdctrl_set_backlight(light_block.backlight);
					lcdctrl_set_contrast(light_block.contrast);

					canix_syslog_P(SYSLOG_PRIO_DEBUG,
							PSTR("lcdctrl_set_light: %d,%d"),
							light_block.backlight, light_block.contrast);

					return;
				}
			}
		}

		// wenn wir hier angekommen sind, wurde kein passender EDS Block
		// gefunden. In diesem Fall werden die Std-Werte verwendet:

		lcdctrl_set_backlight(LCD_BACKLIGHT_DEFAULT_LOW);
		lcdctrl_set_contrast(LCD_CONTRAST_DEFAULT_LOW);
	}
}

