#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <lcdstatemachine.h>
#include <keys.h>
#include <string.h>
#include <stdio.h>
#include <lcd.h>
#include <lcdctrl.h>
#include <eds.h>
#include <eds-structs.h>
#include <canix/rtc.h>
#include <canix/tools.h>
#include <canix/syslog.h>
#include <hcan_multicast.h>
#include <hcan.h>
#include <hcan-def.h>

#include <heiz_page.h>
#include <gesamt_heiz_page.h>
#include <weck_page.h>
#include <rolladen_page.h>
#include <taster_page.h>
#include <powerport_page.h>
#include <fenster_monitor_page.h>
#include <temp_anzeige_page.h>

lcdstate_t lcdstate;


volatile uint8_t keypress_time;
uint8_t lcd_led_countdown;
uint16_t lcd_screensafer_countdown;

void print_page(void);

void load_error_page(uint8_t code)
{
	// Error Page
	lcdstate.active_page.block_type = 255;
	lcdstate.active_page.block_size = 1;
	lcdstate.active_page.page_id = 255;
	lcdstate.active_page.parent_id = 0;
	lcdstate.active_page.next_id = 0;
	lcdstate.active_page.prev_id = 0;
	lcdstate.active_page.data[0] = code;
}

uint8_t page_stack[8];
uint8_t page_stack_ptr = 0;

void page_path_stack_push(uint8_t page_id)
{

	if (page_stack_ptr >= 7)
	{
		load_error_page(ERROR_PAGE_PAGE_STACK_OVERFLOW);
		print_page();
		return;
	}

	page_stack[page_stack_ptr++] = page_id;
}

uint8_t page_path_stack_pop(void)
{
	uint8_t page_id;

	if (page_stack_ptr == 0)
	{
		load_error_page(ERROR_PAGE_PAGE_STACK_UNDERFLOW);
		print_page();
		return 255;
	}

	page_id = page_stack[--page_stack_ptr];

	return page_id;
}

void load_page(uint8_t page)
{
	// ueber alle eds bloecke iterieren und die Seite suchen
	//
	//

	eds_block_p it;

	canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load page %d\n"),page);


	/*
	it = eds_find_next_block_between(EDS_DATA_START, 
			EDS_PAGE_ID_START, EDS_PAGE_ID_END);
	for (; it != 0; it = eds_find_next_block_between(it,
				EDS_PAGE_ID_START, EDS_PAGE_ID_END))
				*/

	EDS_foreach_block_between(it, EDS_PAGE_ID_START, EDS_PAGE_ID_END)
	{

		uint8_t page_id;

		// War testweise noetig, um den Bug (siehe /prj/tickets/TID0938/Log)
		// zu fixen: 
		// if ((EDS_BLOCK_TYPE(it) < 100) || (EDS_BLOCK_TYPE(it) > 150))
		//	continue;
		
		page_id = EDS_BLOCK_DATA(it,0);


		if (page_id == page)
		{
			// wir haben die Seite gefunden; jetzt muss sie in 
			// die lcdpage_t Struktur kopiert werden
			//
			// Die 2 sind fuer den Header des Blocks; den wollen wir
			// auch laden, um mit dem Typ arbeiten zu koennen
			size_t size = EDS_BLOCK_SIZE(it) + 2;

			if ((size) > sizeof(lcdstate.active_page))
			{
				canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("page (%d) size too large\n"),page);
				load_error_page(ERROR_PAGE_SIZE_TOO_LARGE);
			}
			else
			{
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #1 .ap.block_type=%d\n"),lcdstate.active_page.block_type);
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #1 .ap.block_size=%d\n"),lcdstate.active_page.block_size);
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #1 .ap.page_id=%d\n"),lcdstate.active_page.page_id);
				eeprom_read_block(&lcdstate.active_page, it, size);
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #2 .ap.block_type=%d\n"),lcdstate.active_page.block_type);
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #2 .ap.block_size=%d\n"),lcdstate.active_page.block_size);
				canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_page: #2 .ap.page_id=%d\n"),lcdstate.active_page.page_id);
				return;
			}
		}
	}
	canix_syslog_P(SYSLOG_PRIO_ERROR,PSTR("page %d not found\n"),page);
	load_error_page(ERROR_PAGE_PAGE_NOT_FOUND);
}

void goto_page_up(void)
{
	if (lcdstate.active_page.parent_id)
	{
		load_page(lcdstate.active_page.parent_id);
		print_page();
	}
}

void goto_next_page(void)
{
	if (lcdstate.active_page.next_id)
	{
		load_page(lcdstate.active_page.next_id);
		print_page();
	}
}

void goto_prev_page(void)
{
	if (lcdstate.active_page.prev_id)
	{
		load_page(lcdstate.active_page.prev_id);
		print_page();
	}
}

void goto_page(uint8_t page)
{
	canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("goto page %d\n"),page);
	load_page(page);
	print_page();
}

void goto_first_child_page(void)
{
	uint8_t self;
	eds_block_p it;

	self = lcdstate.active_page.page_id;

	page_path_stack_push(self);

	EDS_foreach_block_between(it,EDS_PAGE_ID_START, EDS_PAGE_ID_END)
	{
		if ((EDS_PAGE_PARENT(it) == self) &&
				(EDS_PAGE_PREV(it) == 0))
		{
			size_t size = EDS_BLOCK_SIZE(it) + 2;

			// wenn Datengroesse + 2 Bytes Header groesser als
			// unsere Ziel-Datenstruktur sind, dann liegt ein Problem
			// vor:
			if ((size) > sizeof(lcdstate.active_page))
			{
				load_error_page(ERROR_PAGE_SIZE_TOO_LARGE);
				print_page();
				return;
			}
			else
			{
				eeprom_read_block(&lcdstate.active_page, it, size);
				print_page();
				return;
			}
		}
	}
	load_error_page(ERROR_PAGE_CHILD_PAGE_NOT_FOUND);
	print_page();
}

void print_screensaver_page(void)
{
	char s[32];
	char tag[6];

	switch (canix_rtc_clock.day_of_week)
	{
		case 1 : strcpy_P(tag,PSTR("Mon")); break;
		case 2 : strcpy_P(tag,PSTR("Di")); break;
		case 3 : strcpy_P(tag,PSTR("Mi")); break;
		case 4 : strcpy_P(tag,PSTR("Do")); break;
		case 5 : strcpy_P(tag,PSTR("Fr")); break;
		case 6 : strcpy_P(tag,PSTR("Sa")); break;
		case 7 : strcpy_P(tag,PSTR("So")); break;
	}

	snprintf(s,sizeof(s)-1,"%s, %02d.%02d.%d",tag,
			canix_rtc_clock.day_of_month,
			canix_rtc_clock.month_of_year,
			canix_rtc_clock.year + 2000);
	lcd_gotoxy(0,0);
	lcd_puts(s);
	snprintf(s,sizeof(s)-1,"%02d:%02d Uhr", 
			canix_rtc_clock.hour,
			canix_rtc_clock.minute);
	lcd_gotoxy(0,1);
	lcd_puts(s);
}

void print_dir_page(void)
{
	char s[32];

	eds_dir_page_block_t *p = 
		(eds_dir_page_block_t *)&lcdstate.active_page.page_id;

	snprintf(s,sizeof(s)-1,">%s..",p->name);
	lcd_gotoxy(0,0);
	lcd_puts(s);
}

void print_page(void)
{
	lcd_clrscr();

	switch (lcdstate.active_page.block_type)
	{
		// Error-Page
		case 255 : 
			{
				char s[32];
				snprintf(s,sizeof(s)-1,"Fehler %d:",
						lcdstate.active_page.data[0]);
				lcd_gotoxy(0,0);
				lcd_puts(s);

				switch (lcdstate.active_page.data[0])
				{
					case ERROR_PAGE_SIZE_TOO_LARGE :
						strncpy_P(s,PSTR("Seite zu gr."),sizeof(s)-1);
						break;
					case ERROR_PAGE_PAGE_NOT_FOUND :
						strncpy_P(s,PSTR("Seite n. gef."),sizeof(s)-1);
						break;
					case ERROR_PAGE_CHILD_PAGE_NOT_FOUND :
						strncpy_P(s,PSTR("Child-S. n. gef."),sizeof(s)-1);
						break;
					case ERROR_PAGE_PAGE_STACK_OVERFLOW : 
						strncpy_P(s,PSTR("page stack overflow"),sizeof(s)-1);
						break;
					case ERROR_PAGE_PAGE_STACK_UNDERFLOW : 
						strncpy_P(s,PSTR("page stack underflow"),sizeof(s)-1);
						break;
					case ERROR_PAGE_SHORTCUTS_MISSING :
						strncpy_P(s,PSTR("shortcuts error"),sizeof(s)-1);
						break;
				}
				lcd_gotoxy(0,1);
				lcd_puts(s);
			}
			break;

		case EDS_screensaver_page_BLOCK_ID :
			print_screensaver_page();
			break;

		case EDS_dir_page_BLOCK_ID :
			print_dir_page();
			break;

		case EDS_heiz_page_BLOCK_ID :
			heiz_page_print_page((eds_heiz_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_weck_page_BLOCK_ID :
			weck_page_print_page((eds_weck_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_gesamt_heiz_page_BLOCK_ID :
			gesamt_heiz_page_print_page((eds_gesamt_heiz_page_block_t *)
					&lcdstate.active_page.page_id);
			break;
		case EDS_rolladen_page_BLOCK_ID :
			rolladen_page_print_page((eds_rolladen_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_taster_page_BLOCK_ID :
			taster_page_print_page((eds_taster_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_powerport_page_BLOCK_ID :
			powerport_page_print_page((eds_powerport_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_fenster_monitor_page_BLOCK_ID :
			fenster_monitor_page_print_page((eds_fenster_monitor_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		case EDS_temp_anzeige_page_BLOCK_ID :
			temp_anzeige_page_print_page((eds_temp_anzeige_page_block_t *)
					&lcdstate.active_page.page_id);
			break;

		// Problem: Seitentyp unbekannt
		default :
			{
				char s[32];
				snprintf(s,sizeof(s)-1,"Fehler: Pagetyp");
				lcd_gotoxy(0,0);
				lcd_puts(s);
				snprintf(s,sizeof(s)-1,"%d unbekannt",
						lcdstate.active_page.block_type);
				lcd_gotoxy(0,1);
				lcd_puts(s);
			}
			break;
	}
}


void lcdstatemachine_init(void)
{
	// Wir starten im view-Mode
	lcdstate.state = STATE_VIEW;

	keypress_time = 0;
	lcd_led_countdown = 5;
	lcd_screensafer_countdown = 30;

	// Page mit der id 1 laden (Screensaver Page)
	load_page(1);

	// Seite darstellen
	print_page();

	// ab jetzt warten wir auf Tastendruecke
}


void lcdstatemachine_timer_handler(void)
{
	if (keypress_time && (keypress_time < 255))
		keypress_time++;

	if (lcd_led_countdown > 0)
	{
		lcd_led_countdown--;
		if (lcd_led_countdown == 0)
		{
			// Schoen "smooth" ausblenden...
			uint8_t i;

			i  = LCD_BACKLIGHT_DEFAULT;
			while (i > LCD_BACKLIGHT_DEFAULT_LOW)
			{
				lcdctrl_set_backlight(i--);
				canix_sleep_100th(5);
			}

			lcdctrl_set_display_light(LCD_MODE_INACTIVE);
		}
	}

	// Alle 16 Sekunden die aktuell angezeigte Seite auffrischen:
	//
	if ((canix_rtc_clock.second & 0x0f) == 0)
	{
		// explizit die Hintergrundbeleuchtung auf Inaktiv setzen,
		// wenn der Counter abgelaufen ist; denn wenn nachts das Diplay
		// ausgehen soll, wurde zuvor ja keine Taste gedrueckt (oberer
		// Block wuerde abgearbeitet, Display "smooth" ausgeblendet. Daher
		// hier explizit pruefen, ob der Counter abgelaufen ist und dann
		// die Hintergrundbeleuchtung auf Inaktiv setzen
		if (lcd_led_countdown == 0)
			lcdctrl_set_display_light(LCD_MODE_INACTIVE);

		// Display Update (z.b. Zeitanzeige, neue Temperatur ect.)
		print_page();
	}

	if (lcd_screensafer_countdown > 0)
	{
		lcd_screensafer_countdown--;
		if (lcd_screensafer_countdown == 0)
		{
			// Auf die Startseite zurueckschalten:
			load_page(1);
			print_page();
		}
	}

}

void key_down_event(uint8_t key)
{
	canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("key_down %x\n"),key);

	lcd_led_countdown = 10;
	lcdctrl_set_backlight(LCD_BACKLIGHT_DEFAULT);
	lcdctrl_set_contrast(LCD_CONTRAST_DEFAULT);

	lcd_screensafer_countdown = 300;

	keypress_time = 1;

	if (lcdstate.active_page.block_type == EDS_heiz_page_BLOCK_ID)
	{
		heiz_page_handle_key_down_event((eds_heiz_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else if (lcdstate.active_page.block_type == EDS_weck_page_BLOCK_ID)
	{
		weck_page_handle_key_down_event((eds_weck_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else if (lcdstate.active_page.block_type == EDS_gesamt_heiz_page_BLOCK_ID)
	{
		gesamt_heiz_page_handle_key_down_event((eds_gesamt_heiz_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else if (lcdstate.active_page.block_type == EDS_rolladen_page_BLOCK_ID)
	{
		rolladen_page_handle_key_down_event((eds_rolladen_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else if (lcdstate.active_page.block_type == EDS_taster_page_BLOCK_ID)
	{
		taster_page_handle_key_down_event((eds_taster_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else if (lcdstate.active_page.block_type == EDS_powerport_page_BLOCK_ID)
	{
		powerport_page_handle_key_down_event((eds_powerport_page_block_t *)
				&lcdstate.active_page.page_id, key);
	}
	else
	{
		if (lcdstate.state == STATE_VIEW)
		{
			if (key == KEY_OK)
			{
				// Je nach Page Typ:
				switch (lcdstate.active_page.block_type)
				{
					case EDS_screensaver_page_BLOCK_ID :
						goto_first_child_page();
						break;
					case EDS_dir_page_BLOCK_ID :
						goto_first_child_page();
						break;
				}
			}
			if (key == KEY_ESC)
				goto_page_up();

			if (key == KEY_UP)
				goto_prev_page();

			if (key == KEY_DOWN)
				goto_next_page();

		}
	}
}

void load_shortcut_page(uint8_t n)
{
	eds_block_p it;
	
	it = eds_find_next_block((eds_block_p)0,
			EDS_userpanel_shortcuts_BLOCK_ID);

	if (it)
	{
		userpanel_shortcuts_t shortcuts;
		eeprom_read_block(&shortcuts, (it+2), sizeof(shortcuts));
		goto_page(shortcuts.F[n-1]);
	}
	else
		load_error_page(ERROR_PAGE_SHORTCUTS_MISSING);
}

void save_shortcut_page(uint8_t n)
{
	eds_block_p it;

	// TODO: assert(n >= 0 && n <= 4);
	
	it = eds_find_next_block((eds_block_p)0,
			EDS_userpanel_shortcuts_BLOCK_ID);

	if (it)
	{
		userpanel_shortcuts_t shortcuts;
		eeprom_read_block(&shortcuts, (it+2), sizeof(shortcuts));
		shortcuts.F[n-1] = lcdstate.active_page.page_id;
		eeprom_write_block(&shortcuts, (it+2), sizeof(shortcuts));
	}
	else
		load_error_page(ERROR_PAGE_SHORTCUTS_MISSING);
}

void key_up_event(uint8_t key)
{
	uint8_t n;

	switch (key)
	{
		case KEY_F1 : n = 1; break;
		case KEY_F2 : n = 2; break;
		case KEY_F3 : n = 3; break;
		case KEY_F4 : n = 4; break;
		default: n = 0;
	}
	
	if (n)
	{
		if (keypress_time < 3)
		{
			canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("load_shortcut %x"),n);
			load_shortcut_page(n);
			canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("keytime = %x"),
					keypress_time);
		}
		else
		{
			canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("save_shortcut %x"),n);
			save_shortcut_page(n);
			canix_syslog_P(SYSLOG_PRIO_DEBUG,PSTR("keytime = %x"),
					keypress_time);
		}
	}

	keypress_time = 0;
}


void lcdstatemachine_can_callback(const canix_frame *frame)
{
	if (lcdstate.active_page.block_type == EDS_heiz_page_BLOCK_ID)
		heiz_page_can_callback( (eds_heiz_page_block_t *) 
				&lcdstate.active_page.page_id, frame);

	if (lcdstate.active_page.block_type == EDS_weck_page_BLOCK_ID)
		weck_page_can_callback( (eds_weck_page_block_t *)
				&lcdstate.active_page.page_id, frame);

	if (lcdstate.active_page.block_type == EDS_powerport_page_BLOCK_ID)
		powerport_page_can_callback( (eds_powerport_page_block_t *) 
				&lcdstate.active_page.page_id, frame);

	if (lcdstate.active_page.block_type == EDS_fenster_monitor_page_BLOCK_ID)
		fenster_monitor_page_can_callback( 
				(eds_fenster_monitor_page_block_t *) 
				&lcdstate.active_page.page_id, frame);

	if (lcdstate.active_page.block_type == EDS_temp_anzeige_page_BLOCK_ID)
		temp_anzeige_page_can_callback( 
				(eds_temp_anzeige_page_block_t *) 
				&lcdstate.active_page.page_id, frame);
}

