#ifndef TASTERINPUT_H
#define TASTERINPUT_H

/**
 * Liefert den Zustand des angegebenen Tasterports; der Wertebereich fuer
 * n ist [0..244]
 */
uint8_t tasterport_read(uint8_t n);

#endif
