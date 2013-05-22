#ifndef KEYS_H
#define KEYS_H

#include <inttypes.h>

#define KEY_MASK     0x3fcf

// this are the bit positions of the keys
#define KEY_F1       8
#define KEY_F2       0
#define KEY_F3       9
#define KEY_F4       1

#define KEY_OK       3
#define KEY_ESC      2
#define KEY_UP       7
#define KEY_DOWN     6

void keys_init(void);
void keys_handle_pressed(void);

#endif
