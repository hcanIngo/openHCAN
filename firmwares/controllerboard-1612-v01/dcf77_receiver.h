#ifndef DCF77_RECEIVER_H
#define DCF77_RECEIVER_H

#include <canix/canix.h>
#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>
#include <canix/rtc.h>

typedef struct
{
  uint8_t type;
  eds_dcf77_receiver_block_t config;

  uint8_t last_time_frame_received;
  /**
   * besagt, vor wievielen Sekunden das letzte Time Frame
   * gesendet wurde
   */
  uint8_t last_time_frame_sent;
  
  canix_rtc_clock_t newtime; // Struktur, die die DCF77-Routinen beschreiben
  canix_rtc_clock_t time;    // Struktur, die für die Zeitausgabe verwendet wird

  uint8_t timeflags;
  uint8_t dcf77_pulse;     // Pulsdauer (zwischen 0,1 ['0'] und 0,2s ['1'])
  uint8_t dcf77_period;    // Taktung der Pulse (1 Sekunde, außer bei Sekunde 59)
  uint8_t ct_100Hz;         // 100Hz Grund-Takt
  uint8_t dcf77error;  // sammelt alle Fehler, die die DCF77-Routinen beschreiben, innerhalb einer Minute
  uint8_t reported_dcf77errors; // Variable für die Fehlerausgabe
  uint8_t reported_minute;

  uint8_t valid_flag;
  uint8_t report_flag;

  uint8_t dcf77_time;
  uint8_t dcf77_input_old;
  uint8_t parity;

} device_data_dcf77_receiver;


// Fehlercodes:
#define DCF77_NO_SIGNAL               0x80
#define DCF77_ILLEGAL_PERIOD_LENGTH   0x40
#define DCF77_ILLEGAL_PULSE_LENGTH    0x20
#define DCF77_PARITY_ERROR            0x10


#define ONE_TICK	0
#define ONE_SECOND	1

#define DIVISOR_DCF77 1024L
#define TICKS_PER_SECOND 100

#define TICKS_ZERO 10
#define TICKS_ONE  20

#define TICK_ERROR 2 // Flankenuebergaenge werden mit einer Ungenauigkeit von bis zu 2 Ticks erfasst 

void dcf77_receiver_init(device_data_dcf77_receiver* p, eds_block_p it);
inline void dcf77_receiver_timer_handler(device_data_dcf77_receiver *p, uint8_t zyklus);
void dcf77_receiver_can_callback(const canix_frame *frame);

// Routinen zur Signalauswertung:
void scan_dcf77( device_data_dcf77_receiver *p );
void decode_dcf77( uint8_t pulse, device_data_dcf77_receiver *p );
void dcf77_handler(device_data_dcf77_receiver *p);
#endif

