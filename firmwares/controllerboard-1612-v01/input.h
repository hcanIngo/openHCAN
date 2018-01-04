#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/**
 * Liefert den Zustand des angegebenen Tasterports; der Wertebereich fuer
 * n ist [0..244]
 */
uint8_t inputport_read(uint8_t pullup, uint8_t n);

#endif
