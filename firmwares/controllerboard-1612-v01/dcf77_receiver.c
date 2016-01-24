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
/*              Adapted for openHCAN:                                   */
/*                      Christoph Delfs                                 */
/*                                                                      */
/*                                                                      */
/************************************************************************/


#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <devices.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <hcan_multicast.h>
#include "dcf77_receiver.h"
#include "darlingtonoutput.h"
#include "tasterinput.h"
#include <avr/pgmspace.h>

#define code PROGMEM
#define LPM(i) (__LPM_classic__(i))


// Es dauert in der Regel 2 bis 3 Minuten, bis eine gültige Zeit ermittelt wurde.
// Über den monitor_port kann man prüfen, ob der Sekundentakt richtig erkannt wird.


// Controller-eigene interne Uhr:
extern volatile canix_rtc_clock_t canix_rtc_clock;


void dcf77_receiver_init(device_data_dcf77_receiver* p, eds_block_p it) 
{ 
  // Ausgabeport ermitteln: hier kann eine LED zur Visualisierung des DCF77-Signals angeschlossen werden
  p->last_time_frame_received = 0;
  p->dcf77error = 0;  // sammelt alle Fehler, die die DCF77-Routinen beschreiben, innerhalb einer Minute
  p->reported_dcf77errors = 0; // Variable für die Fehlerausgabe
  p->time.second = 0;
  p->timeflags = 0;
  p->dcf77_pulse = 0;     // Pulsdauer (zwischen 0,1 ['0'] und 0,2s ['1'])
  p->dcf77_period = 0;    // Taktung der Pulse (1 Sekunde, außer bei Sekunde 59)
  p->ct_100Hz = 0;         // 100Hz Grund-Takt
  p->reported_minute = -1;

  p->valid_flag = 0;
  p->report_flag = 0;

  // dcf77_handler:
  p->dcf77_time=0;
  p->dcf77_input_old=0;
  p->parity = 0;
}


// Device Handler wird alle 10ms aufgerufen:
inline void dcf77_receiver_timer_handler(device_data_dcf77_receiver *p, uint8_t zyklus)
{
  if (zyklus != 100) return; // 100tel-Sekunden-Zyklus verwendet

  // Read input from DCF77 receiver:
  dcf77_handler(p);
  // Liegt jetzt eine gültige DCF77-Zeit vor?
  scan_dcf77(p);

  // Der Zähler (wann wurde zuletzt eine Zeitinformation empfangen?) wird hochgezählt.
  // Dies geschieht auch, wenn noch keine gültige Zeit vorliegt.
  if ((p->last_time_frame_received < 255)&&(p->timeflags &  (1 << ONE_SECOND)))
    p->last_time_frame_received++;
    
  // Wenn kein Signal anliegt, wird die canix-Uhr herangezogen, um im Minutentakt zu berichten:
  if ((canix_rtc_clock.second == 0)&&(p->reported_dcf77errors>=DCF77_NO_SIGNAL)) {
    p->report_flag     = 1;
  }

  // Daraus ergibt sich: Ein Fehler wird zu jeder vollen Minute ausgegeben:
  // a) getriggert durch die canix-eigene Uhr
  // b) getriggert durch den erkannten Anfang einer neuen Minute durch das DCF77-Modul
  // Dabei wird eine Fehlerausgabe durch b) immer erfolgen (als Rückmeldung, daß der Minutenübergang erkannt wurde)

  // Jede volle Minute ggf. Zeit oder Fehler ausgeben:
  if (p->report_flag) {

    p->report_flag = 0;
    
    canix_frame message;
    
    message.src = canix_selfaddr();
    message.dst = HCAN_MULTICAST_INFO;
    message.proto = HCAN_PROTO_SFP;

    // Wenn die letzte Minute fehlerhaft empfangen wurde, Fehler ggf. absenden:
    if ((p->reported_dcf77errors)&&(p->config.enable_error_reporting)&&(canix_rtc_clock.minute != p->reported_minute)) {
      message.data[0] = HCAN_SRV_HES;
      message.data[1] = HCAN_HES_DCF77_ERROR;
      message.data[2] = p->config.id;
      message.data[3] = p->reported_dcf77errors;
    
      message.size    = 4;
      canix_frame_send(&message);      

    // sicherstellen, daß nur einmal pro Minute der Fehlerstatus mitgeteilt wird:
    p->reported_minute = canix_rtc_clock.minute;
    

    }
    else {     
      // Zeitinformation wird gesendet, wenn lange genug keine Zeitinformation empfangen wurde 
      // und eine gültige Zeitinformation vorliegt.
      // p->last_time_frame_received wird durch den Callback dcf77_receiver_can_callback auf 0 gesetzt,
      // wenn eine Zeitinformation mit niedrigerem Level empfangen wurde.
      if ((p->last_time_frame_received >= p->config.takeover_time)&& p->valid_flag) {
	
	// Zeit-Info:
	message.data[0] = HCAN_SRV_RTS;
	message.data[1] = HCAN_RTS_TIME_INFO;
	message.data[2] = p->config.level;
	message.data[3] = p->time.day_of_week;
	message.data[4] = p->time.hour;
	message.data[5] = p->time.minute;
	message.data[6] = p->time.second;
	
	message.size    = 7;
	canix_frame_send(&message);
	
	// Datumsinfo:
	message.data[0] = HCAN_SRV_RTS;
	message.data[1] = HCAN_RTS_DATE_INFO;
	message.data[2] = p->config.level;
	message.data[3] = p->time.day_of_month;
	message.data[4] = p->time.month_of_year;
	message.data[5] = p->time.year;
	
	message.size    = 6;
	canix_frame_send(&message);
	
	
	p->last_time_frame_sent = 0;	
	p->valid_flag = 0;
      }	  
    }
  }
  p->timeflags   = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////


void dcf77_handler(device_data_dcf77_receiver *p)
{

  uint8_t dcf77_input;


  dcf77_input = tasterport_read(p->config.port);

  if( p->dcf77_time != 0xFF ) {			// bei 255 Ticks mit dem Zählen aufhören, sonst würde die Rechnung durcheinander kommen
    p->dcf77_time++;           
  }                    // sonst hochzählen
  else {
    p->dcf77error  |= DCF77_NO_SIGNAL;    // so eine lange Periode kann es nicht geben..
  }
			
  if( dcf77_input != p->dcf77_input_old ){	// Hat sich der Pegel am Empfaenger geaendert?

    p->dcf77_input_old = dcf77_input; 

    // Ein neuer Puls bedeutet eine fallende Flanke: 1->0 Übergang
    // Das Ende eines Pulse ist ein 0->1 Übergang

    if(dcf77_input){                    // War ein 1->0 Wechsel? (Beginn eines Informations-Pulses)
      p->dcf77_period = p->dcf77_time;		// merken, wie viele Ticks seit dem letzten Pulsanfang vergangen sind (Periodenbestimmung)
      p->dcf77_time = 0;				// und wieder bei 0 loszählen
    }
    else{
                                                // nein: dann ein 0->1 Wechsel (Ende eines Informations-Pulses)
      p->dcf77_pulse = p->dcf77_time;			// Pulslänge merken, die Ticks werden aber weitergezählt für die Periodenbestimmung
      // Report pulse length:
      if (p->config.enable_error_reporting) {
	canix_frame message;
    
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;

	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_DCF77_PULSE_DURATION;
	message.data[2] = p->config.id;
	message.data[3] = p->dcf77_pulse;
	message.size    = 4;
	canix_frame_send(&message);      
      }      
    }
  }
  if( ++p->ct_100Hz <TICKS_PER_SECOND )	       
    // wieder ein Tick vorbei
    p->timeflags = 1<<ONE_TICK;			 
  else {
    // Flag setzen, dass eine Sekunde herum ist (und natürlich ein Tick)
    p->timeflags = (1<<ONE_SECOND) ^ (1<<ONE_TICK); 
    p->ct_100Hz = 0;                                 // Zähler zurücksetzen
  }
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
void decode_dcf77( uint8_t pulse,device_data_dcf77_receiver *p)
{
  uint8_t i;
  uint8_t *d;

  // Siehe http://de.wikipedia.org/wiki/DCF77
  // Nur die Bits 21 bis 58 enthalten die regulären ZeitInformation:
  i = p->newtime.second - 21;
  if( i >= sizeof( BITNO ))			
    return;

  // Anmerkung:
  // Die Bits 16 und 19 enthalten Informationen zur Sommerzeit bzw. Schaltsekunde
  // und werden nicht ausgewertet.

  // Nach den Infobits für Minute, Stunde und den restlichen Infobits sind Paritätsbits eingelassen.
  // Mit jedem neuen Bit wird der erwartete Paritätswert neu bestimmt.
  // Auch das abschließende Paritätsbit wird mitverarbeitet.
  p->parity ^= pulse;

  // Jetzt wird geprüft, ob das aktuelle Bit etwa das Paritätsbit ist:
  i = LPM(&BITNO[i]);
  if( i == 0xFF ){    // wenn ja..
    if( p->parity )      // und die Parität 1 ist.. 
      p->dcf77error |= DCF77_PARITY_ERROR; // dann war das ein Fehler.. ("Even-Parity", gerade Parität)
    p->parity = 0;       // in jedem Fall geht es nun mit der nächsten Bit-Sequenz weiter
    return;
  }

  // Das empfangene Bit war kein Paritätsbit. 
  // Nun wird das Bit direkt - entsprechend seiner Wertigkeit - in das Feld <newtime> hineingeschrieben:

  // 'd' ist der Byte-Offset
  // 'i>>4' ermittelt die Bits 4-6 und liefert den Index auf das Byte in der Struktur
  // Siehe Tabelle BIT_NO
  // Für die Minute ist i>>4 gleich 2, für die Stunde ist er gleich 1, u.s.w.
  d = (uint8_t *)&p->newtime.day_of_week + (i >> 4); 

  // Jetzt wird die Index eliminiert, um den "Wert" des Bits zu bestimmen:
  i &= 0x0F;			

  // Wenn es das erste Bit ist, wird der Eintrag auf 0 gesetzt
  if( i == 0 )
    *d = 0;

  // Wenn das Bit gesetzt ist, wird sein "Wert" gleich zum Eintrag dazuaddiert.
  if( pulse )
    *d += LPM(&BMASK[i]);
}

void scan_dcf77( device_data_dcf77_receiver *p )
{
  // Mit jedem Sekundentakt (Ausnahme siehe unten) wird eine Bitinformation mitgeliefert.
  // Die Dauer der Flankenwechsel werden in '0' und '1' umgewandelt:

  p->reported_dcf77errors = p->dcf77error; // alte Fehler merken

  if( p->dcf77_pulse ){
    // Kurzer Puls: '0'
    // Erfolg, wenn ein Tick die Länge 5...6 hat
    if( p->dcf77_pulse > (TICKS_ZERO-TICK_ERROR-1) && p->dcf77_pulse < (TICKS_ZERO+TICK_ERROR+1)){
      decode_dcf77( 0,p );
    }else{
      // Langer Puls: '1'
      // Erfolg, wenn ein Tick die Länge 11..12 hat
      if( p->dcf77_pulse > (TICKS_ONE-TICK_ERROR-1) && p->dcf77_pulse < (TICKS_ONE+TICK_ERROR+1)){
	decode_dcf77( 1,p );
      }else{
	// sonst ist die Pulslänge nicht einzuordnen:
	p->dcf77error |= DCF77_ILLEGAL_PULSE_LENGTH;
      }
    }
    p->dcf77_pulse = 0;
  }

  if( p->dcf77_period ){
    if( p->newtime.second < 60 )
      p->newtime.second++;
    // Die 59. Sekunde enthält kein Informationsbit (keinen Flankenwechsel)
    // D.h., 2 Sekunden werden Ticks bis zum Beginn der nächsten Periode hochgezählt.
    // 2 Sekunden = 2*TICKS_PER_SECOND:
    if(( p->dcf77_period > 2*TICKS_PER_SECOND-TICK_ERROR-TICK_ERROR-1) && (p->dcf77_period < (2*TICKS_PER_SECOND+TICK_ERROR+1))) {
      // Dieser Flankenwechsel ist gleich dem Anfang einer neuen Minute:
      if( p->dcf77error == 0 && p->newtime.second == 59 ){
        p->valid_flag         = 1;              // Zeitinformation gültig!
	p->ct_100Hz           = 0;              // Zähler gleich zurücksetzen
	p->time.minute        = p->newtime.minute; // ermittelte Zeit für die Ausgabe retten
	p->time.hour          = p->newtime.hour;
	p->time.day_of_week   = p->newtime.day_of_week;
	p->time.day_of_month  = p->newtime.day_of_month;
	p->time.month_of_year = p->newtime.month_of_year;
	p->time.year          = p->newtime.year;
      }
      p->newtime.second = 0;   // das ist ok und der Sekundenzähler wird angepaßt
      p->dcf77error = 0;       // alte Fehler werden ggf. gelöscht -> neuer Versuch                            
      p->report_flag = 1;      // für Zeit- oder Fehlerausgabe
    }
    else{
      // Eine Periode entspricht 1 Sekunde, d.h. 60 Ticks mit der Länge T2COUNT 
      // Wenn die Periode zu viele oder zu wenige Ticks enthält,stimmt irgendetwas nicht:
      if( p->dcf77_period < TICKS_PER_SECOND-TICK_ERROR-1 || p->dcf77_period > TICKS_PER_SECOND+TICK_ERROR)
        p->dcf77error |= DCF77_ILLEGAL_PERIOD_LENGTH;
      //
      
    }
    p->dcf77_period = 0;
  }
}

