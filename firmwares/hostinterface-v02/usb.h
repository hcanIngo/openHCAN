#ifndef USB_H
#define USB_H

#include <inttypes.h>

uint8_t usb_writable(void);
void usb_putc(unsigned char t);
uint8_t usb_readable(void);
uint8_t usb_getc(void);

#endif
