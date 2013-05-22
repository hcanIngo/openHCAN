#ifndef DCF77_RECEIVER_H
#define DCF77_RECEIVER_H

#include <canix/canix.h>
#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>

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
} device_data_dcf77_receiver;


// Fehlercodes:
#define DCF77_NO_SIGNAL               0x80
#define DCF77_ILLEGAL_PERIOD_LENGTH   0x40
#define DCF77_ILLEGAL_PULSE_LENGTH    0x20
#define DCF77_PARITY_ERROR            0x10


#define ONE_TICK	0
#define ONE_SECOND	1

#define XTAL   3686400
#define INTERRUPT_CLOCK_CYCLE_DCF77  ((1<<CS22)|(1<<CS21)|(1<<CS20)) // liefert 1024-Teiler
#define DIVISOR_DCF77 1024L
#define TICKS_PER_SECOND 60

#define T2COUNT     (XTAL / DIVISOR_DCF77 / TICKS_PER_SECOND) // 60 "Miniticks" ergeben einen Tick

// Zahl der Ticks pro für einen Puls der Länge 0,1s ('0') und Länge 0,2s ('1')
#define TICKS_ZERO ((XTAL / DIVISOR_DCF77)/(10*T2COUNT)) // 6
#define TICKS_ONE  ((XTAL / DIVISOR_DCF77)/(5*T2COUNT))  // 12

#define TICK_ERROR 1 // Flankenübergänge werden mit einer Ungenauigkeit von höchstens einem Tick erfaßt 

void dcf77_receiver_init(device_data_dcf77_receiver* p, eds_block_p it) ;
void dcf77_receiver_timer_handler(device_data_dcf77_receiver *p);
void dcf77_receiver_can_callback(const canix_frame *frame);

// Interrupt-Routine zur Signalabtastung und Puls/Periodenmessung:
uint8_t dcf77_ISR(void);

// Routinen zur Signalauswertung:
void scan_dcf77( void );
void decode_dcf77( uint8_t pulse );
#endif

