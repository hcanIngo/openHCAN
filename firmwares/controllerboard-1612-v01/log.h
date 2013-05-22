#ifndef LOG_H
#define LOG_H

#include <avr/io.h>

// TODO: abschaffen - ist obsolet

void send_log(uint8_t data, uint8_t data2);
void send_log2(uint8_t data, uint16_t data2);

#endif
