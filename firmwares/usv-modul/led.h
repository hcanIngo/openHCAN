#ifndef LED_H
#define LED_H

#include <inttypes.h>

void led_init(void);

/**
 * wird alle 100msec aufgerufen und kuemmert sich um das Blinken
 * der LEDs
 */
void led_timer_handler(void);

/**
 * schaltet LED Nr. n;
 * state = 0: LED aus
 * state = 10; LED daueran
 * state = 5; LED blinkt mit Taktverhaeltnis 1:1
 */
void led_set(uint8_t n, uint8_t state);

#endif
