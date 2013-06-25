/************************************************************************/
/*									*/
/*			DCF77 Time Information Receiver			*/
/*									*/
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/*              Original Sources (DCF77.ZIP):                           */
/*              http://www.mikrocontroller.net/topic/58769#456232       */
/*                                                                      */
/*              Adapted for HCAN:                                       */
/*                      Christoph Delfs                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/


#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <devices.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <hcan_multicast.h>
#include <user_interrupt.h>
#include <dcf77-receiver.h>
#include <darlingtonoutput.h>
#include<avr/pgmspace.h>

#define code PROGMEM
#define LPM(i) (__LPM_classic__(i))


// Es dauert in der Regel 2 bis 3 Minuten, bis eine gültige Zeit ermittelt wurde.
// Über den monitor_port kann man prüfen, ob der Sekundentakt richtig erkannt wird.

canix_rtc_clock_t newtime; // Struktur, die die DCF77-Routinen beschreiben
canix_rtc_clock_t time;    // Struktur, die für die Zeitausgabe verwendet wird

volatile uint8_t timeflags;
uint8_t dcf77_pulse;     // Pulsdauer (zwischen 0,1 ['0'] und 0,2s ['1'])
uint8_t dcf77_period;    // Taktung der Pulse (1 Sekunde, außer bei Sekunde 59)
uint8_t ct_64Hz;         // 64Hz Grund-Takt
uint8_t dcf77error = 0;  // sammelt alle Fehler, die die DCF77-Routinen beschreiben, innerhalb einer Minute
volatile uint8_t reported_dcf77errors = 0; // Variable für die Fehlerausgabe

// HCAN Device Parameter
uint8_t port;
static uint8_t monitor_port;

uint8_t  report_flag = 0; // nur von 0 verschieden, wenn eine neue Minute begonnen hat
uint8_t  valid_flag  = 0; // nur von 0 verschieden, wenn eine gültige Zeit empfangen wurde

// Controller-eigene interne Uhr:
extern volatile canix_rtc_clock_t canix_rtc_clock;


void dcf77_receiver_init(device_data_dcf77_receiver* p, eds_block_p it) 
{ 
  uint8_t n;


  // Port holen:
  n = port = p->config.port;

  // Ausgabeport ermitteln: hier kann eine LED zur Visualisierung des DCF77-Signals angeschlossen werden
  monitor_port = p->config.monitor_port;
  p->last_time_frame_received = 0;

  if (n < 8)
    {
      // Pins sind 1:1 von PORTC auszulesen
      
      // Pullup einschalten
      PORTC |= (1<< n);
      
      // Modus Input setzen
      DDRC &= ~ (1<< n);
      
      
    }
  else if (n < 16)
    {
      // auf den Bereich 0-7 holen:
      n &= 0x07;
      
      // Pins sind zu spiegeln ( 0 -> 7, 1 -> 6 etc.)
      n = 7 - n;
      
      // Pullup einschalten
      PORTA |= (1<< n);
      
      // Modus Input setzen
      DDRA &= ~ (1<< n);
    }      

  time.second = 0;

  // "Interrupt-Routine" für den DCF77-Empfänger einhängen:
  user_interrupt_hook_in_my_ISR(dcf77_ISR);
}


// Device Handler wird alle 10ms aufgerufen:
void dcf77_receiver_timer_handler(device_data_dcf77_receiver *p)
{
  static uint8_t reported_minute = -1;

  // Liegt jetzt eine gültige DCF77-Zeit vor?
  scan_dcf77();

  // Der Zähler (wann wurde zuletzt eine Zeitinformation empfangen?) wird hochgezählt.
  // Dies geschieht auch, wenn noch keine gültige Zeit vorliegt.
  if ((p->last_time_frame_received < 255)&&(timeflags &  (1 << ONE_SECOND)))
    p->last_time_frame_received++;
    
  // Wenn kein Signal anliegt, wird die canix-Uhr herangezogen, um im Minutentakt zu berichten:
  if ((canix_rtc_clock.second == 0)&&(reported_dcf77errors>=DCF77_NO_SIGNAL) &&
      (canix_rtc_clock.minute != reported_minute)) {
    report_flag     = 1;
    reported_minute = canix_rtc_clock.minute;
  }

  // Daraus ergibt sich: Ein Fehler wird zu jeder vollen Minute ausgegeben:
  // a) getriggert durch die canix-eigene Uhr
  // b) getriggert durch den erkannten Anfang einer neuen Minute durch das DCF77-Modul
  // Dabei wird eine Fehlerausgabe durch b) immer erfolgen (als Rückmeldung, daß der Minutenübergang erkannt wurde)

  // Jede volle Minute ggf. Zeit oder Fehler ausgeben:
  if (report_flag) {
    
    canix_frame message;
    
    message.src = canix_selfaddr();
    message.dst = HCAN_MULTICAST_INFO;
    message.proto = HCAN_PROTO_SFP;

    // Wenn die letzte Minute fehlerhaft empfangen wurde, Fehler ggf. absenden:
    if ((reported_dcf77errors)&&(p->config.enable_error_reporting)) {
      message.data[0] = HCAN_SRV_HES;
      message.data[1] = HCAN_HES_DCF77_ERROR;
      message.data[2] = p->config.id;
      message.data[3] = reported_dcf77errors;
    
      message.size    = 4;
      canix_frame_send(&message);      

      // sicherstellen, daß nur einmal pro Minute der Fehlerstatus mitgeteilt wird:
      reported_minute = canix_rtc_clock.minute;

    }
    else {     
      // Zeitinformation wird gesendet, wenn lange genug keine Zeitinformation empfangen wurde 
      // und eine gültige Zeitinformation vorliegt.
      // p->last_time_frame_received wird durch den Callback dcf77_receiver_can_callback auf 0 gesetzt,
      // wenn eine Zeitinformation mit niedrigerem Level empfangen wurde.
      if ((p->last_time_frame_received >= p->config.takeover_time)&& valid_flag) {
	
	// Zeit-Info:
	message.data[0] = HCAN_SRV_RTS;
	message.data[1] = HCAN_RTS_TIME_INFO;
	message.data[2] = p->config.level;
	message.data[3] = time.day_of_week;
	message.data[4] = time.hour;
	message.data[5] = time.minute;
	message.data[6] = time.second;
	
	message.size    = 7;
	canix_frame_send(&message);
	
	// Datumsinfo:
	message.data[0] = HCAN_SRV_RTS;
	message.data[1] = HCAN_RTS_DATE_INFO;
	message.data[2] = p->config.level;
	message.data[3] = time.day_of_month;
	message.data[4] = time.month_of_year;
	message.data[5] = time.year;
	
	message.size    = 6;
	canix_frame_send(&message);
	
	
	p->last_time_frame_sent = 0;	
	valid_flag = 0;
      }	  
    }
  }
  report_flag = 0;
  timeflags   = 0;
}



// Mit Hilfe des Callbacks wird geprüft, ob eine HCAN_RTS Nachricht mit niedrigerem Level gesendet wurde.
void dcf77_receiver_can_callback(const canix_frame *frame)
{

  if (frame->data[1] == HCAN_RTS_TIME_INFO) {
    uint8_t i;
    // Alle DCF77-Empfänger werden durchsucht und separat geprüft:
    // (Der Code ist allerdings nicht dafür ausgelegt, daß mehr als ein DCF77-Device angeschlossen wird.)
    for (i = 0; i < MAX_PDEVICE_DATA; i++)      {
      device_data_dcf77_receiver *p = (device_data_dcf77_receiver *) pdevice_data[i];      
      if (p && p->type == EDS_dcf77_receiver_BLOCK_ID)
	// Wenn der Level der empfangenen Nachricht niedriger als der Level des Devices ist,
	// wird dessen Zähler auf 0 gesetzt.
	if (frame->data[2] < p->config.level)
	  p->last_time_frame_received = 0;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////

//uint8_t ISR_timer_counter;  // Rückmeldung für den user interrupt


// "Interrupt-Routine" für den DCF77-Empfänger
uint8_t dcf77_ISR(void)
{
  static uint8_t val = 1;
  static uint8_t dcf77_time, old_dcf77;

  uint8_t DCF77_PIN;
  //  uint8_t DCF77_PORT;
  uint8_t DCF77;

  // Portabfrage
  if (port < 8) {
    DCF77_PIN  = PINC;
    //  DCF77_PORT = PORTC;
    DCF77 = 1 << port;
  }
  else
  {
    uint8_t k;
    k = port &  0x07;
    k = 7 - k;
    DCF77_PIN  = PINA;
    // DCF77_PORT = PORTA;
    DCF77 = 1<< k;
  }



  if( dcf77_time != 0xFF )			// bei 255 Ticks mit dem Zählen aufhören, sonst würde die Rechnung durcheinander kommen
    dcf77_time++;                               // sonst hochzählen
  else
    dcf77error  |= DCF77_NO_SIGNAL;    // so eine lange Periode kann es nicht geben..
				
  if( (DCF77_PIN ^ old_dcf77) & DCF77 ){	// Hat sich der Pegel geändert?
    // dann: "Toggeln" des Ausgabeports:
    if (monitor_port < 12) {
      darlingtonoutput_setpin(monitor_port,val); // Flankenwechsel ausgeben, wenn gewünscht
      val^=1;
    }

    // Ein neuer Puls bedeutet eine fallende Flanke: 1->0 Übergang
    // Das Ende eines Pulse ist ein 0->1 Übergang

    old_dcf77 ^= 0xFF;				// Flankenwechsel registrieren 
    if( old_dcf77 & DCF77 ){                    // War ein 1->0 Wechsel? (Beginn eines Informations-Pulses)
      dcf77_period = dcf77_time;		// merken, wie viele Ticks seit dem letzten Pulsanfang vergangen sind (Periodenbestimmung)
      dcf77_time = 0;				// und wieder bei 0 loszählen
    }else{
                                                // nein: dann ein 0->1 Wechsel (Ende eines Informations-Pulses)
      dcf77_pulse = dcf77_time;			// Pulslänge merken, die Ticks werden aber weitergezählt für die Periodenbestimmung
    }
  }

  if( ++ct_64Hz <TICKS_PER_SECOND )	       
    // wieder ein Tick vorbei
    timeflags = 1<<ONE_TICK;			 
  else {
    // Flag setzen, dass eine Sekunde herum ist (und natürlich ein Tick)
    timeflags = (1<<ONE_SECOND) ^ (1<<ONE_TICK); 
    ct_64Hz = 0;                                 // Zähler zurücksetzen
  }

  // Nächster Aufruf:
  return ((uint16_t)(256 - T2COUNT));

}

// Die Tabelle BITNO enthält mehrere Informationen:
// o Paritätsbits sind mit 0xFF gekennzeichnet
// o Jedes Infobit besitzt in den Bits 0-3 einen Marker für den "Wert" des Bits
//   Dieser Marker wird im Zusammenhang mit der Tabelle BMASK benutzt.
// o Jedes Infobit besitzt in den Bits 4-6 einen Marker: 2 für Minute, 1 für Stunde, 5 für Tag u.s.w.
//   Dieser Marker dient als Index in die Struktur newtime, um direkt in die Struktur schreiben zu können
//   und ist definiert durch den Aufbau des Typs canix_rtc_clock_t (aus <canix/rtc.h>).

// Anmerkung: die Tabelle ist gegenüber der Originaltabelle von Peter Dannegger 
// durch den Aufbau von canix_rtc_clock_t verändert.

const uint8_t code BITNO[]  = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,		// Minute
    0xFF,						// Parität
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15,			// Stunde
    0xFF,						// Parität
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55,			// Tag
    0x00, 0x01, 0x02,					// Wochentag
    0x60, 0x61, 0x62, 0x63, 0x64,			// Monat
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,	// Jahr
    0xFF };						// Parität


// Diese Tabelle beschreibt den "Wert" der Bits für Minute, Stunde, Tag, Wochentag, Monat und Jahr:
// Z.B.: Bit 5 trägt, wenn gesetzt, den Wert 20
const uint8_t BMASK[] code  = { 1, 2, 4, 8, 10, 20, 40, 80 };


// Das nächste Bit wird empfangen:
void decode_dcf77( uint8_t pulse )
{
  static uint8_t parity = 0;
  uint8_t i;
  uint8_t *d;

  // Siehe http://de.wikipedia.org/wiki/DCF77
  // Nur die Bits 21 bis 58 enthalten die regulären ZeitInformation:
  i = newtime.second - 21;
  if( i >= sizeof( BITNO ))			
    return;

  // Anmerkung:
  // Die Bits 16 und 19 enthalten Informationen zur Sommerzeit bzw. Schaltsekunde
  // und werden nicht ausgewertet.

  // Nach den Infobits für Minute, Stunde und den restlichen Infobits sind Paritätsbits eingelassen.
  // Mit jedem neuen Bit wird der erwartete Paritätswert neu bestimmt.
  // Auch das abschließende Paritätsbit wird mitverarbeitet.
  parity ^= pulse;

  // Jetzt wird geprüft, ob das aktuelle Bit etwa das Paritätsbit ist:
  i = LPM(&BITNO[i]);
  if( i == 0xFF ){    // wenn ja..
    if( parity )      // und die Parität 1 ist.. 
      dcf77error |= DCF77_PARITY_ERROR; // dann war das ein Fehler.. ("Even-Parity", gerade Parität)
    parity = 0;       // in jedem Fall geht es nun mit der nächsten Bit-Sequenz weiter
    return;
  }

  // Das empfangene Bit war kein Paritätsbit. 
  // Nun wird das Bit direkt - entsprechend seiner Wertigkeit - in das Feld <newtime> hineingeschrieben:

  // 'd' ist der Byte-Offset
  // 'i>>4' ermittelt die Bits 4-6 und liefert den Index auf das Byte in der Struktur
  // Siehe Tabelle BIT_NO
  // Für die Minute ist i>>4 gleich 2, für die Stunde ist er gleich 1, u.s.w.
  d = (uint8_t *)&newtime.day_of_week + (i >> 4); 

  // Jetzt wird die Index eliminiert, um den "Wert" des Bits zu bestimmen:
  i &= 0x0F;			

  // Wenn es das erste Bit ist, wird der Eintrag auf 0 gesetzt
  if( i == 0 )
    *d = 0;

  // Wenn das Bit gesetzt ist, wird sein "Wert" gleich zum Eintrag dazuaddiert.
  if( pulse )
    *d += LPM(&BMASK[i]);
}

void scan_dcf77( void )
{
  // Mit jedem Sekundentakt (Ausnahme siehe unten) wird eine Bitinformation mitgeliefert.
  // Die Dauer der Flankenwechsel werden in '0' und '1' umgewandelt:

  reported_dcf77errors = dcf77error; // alte Fehler merken

  if( dcf77_pulse ){
    // Kurzer Puls: '0'
    // Erfolg, wenn ein Tick die Länge 5...6 hat
    if( dcf77_pulse > (TICKS_ZERO-TICK_ERROR-1) && dcf77_pulse < (TICKS_ZERO+TICK_ERROR+1)){
      decode_dcf77( 0 );
    }else{
      // Langer Puls: '1'
      // Erfolg, wenn ein Tick die Länge 11..12 hat
      if( dcf77_pulse > (TICKS_ONE-TICK_ERROR-1) && dcf77_pulse < (TICKS_ONE+TICK_ERROR+1)){
	decode_dcf77( 1 );
      }else{
	// sonst ist die Pulslänge nicht einzuordnen:
	dcf77error |= DCF77_ILLEGAL_PULSE_LENGTH;
      }
    }
    dcf77_pulse = 0;
  }

  if( dcf77_period ){
    if( newtime.second < 60 )
      newtime.second++;
    // Die 59. Sekunde enthält kein Informationsbit (keinen Flankenwechsel)
    // D.h., 2 Sekunden werden Ticks bis zum Beginn der nächsten Periode hochgezählt.
    // 2 Sekunden = 2*TICKS_PER_SECOND:
    if(( dcf77_period > 2*TICKS_PER_SECOND-TICK_ERROR-TICK_ERROR-1) && (dcf77_period < (2*TICKS_PER_SECOND+TICK_ERROR+1))) {
      // Dieser Flankenwechsel ist gleich dem Anfang einer neuen Minute:
      if( dcf77error == 0 && newtime.second == 59 ){
        valid_flag         = 1;              // Zeitinformation gültig!
	ct_64Hz            = 0;              // Zähler gleich zurücksetzen
	time.minute        = newtime.minute; // ermittelte Zeit für die Ausgabe retten
	time.hour          = newtime.hour;
	time.day_of_week   = newtime.day_of_week;
	time.day_of_month  = newtime.day_of_month;
	time.month_of_year = newtime.month_of_year;
	time.year          = newtime.year;
      }
      newtime.second = 0;   // das ist ok und der Sekundenzähler wird angepaßt
      dcf77error = 0;       // alte Fehler werden ggf. gelöscht -> neuer Versuch                            
      report_flag = 1;      // für Zeit- oder Fehlerausgabe
    }
    else{
      // Eine Periode entspricht 1 Sekunde, d.h. 60 Ticks mit der Länge T2COUNT 
      // Wenn die Periode zu viele oder zu wenige Ticks enthält,stimmt irgendetwas nicht:
      if( dcf77_period < TICKS_PER_SECOND-TICK_ERROR-1 || dcf77_period > TICKS_PER_SECOND+TICK_ERROR)
        dcf77error |= DCF77_ILLEGAL_PERIOD_LENGTH;
      //
      
    }
    dcf77_period = 0;
  }
}

