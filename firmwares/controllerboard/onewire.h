#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <canix/canix.h>

// 1Wire Device Types:
#define OW_DS1820   1

#define MATCH_ROM   0x55
#define SKIP_ROM    0xCC
#define SEARCH_ROM  0xF0
    
#define CONVERT_T   0x44        // DS1820 commands
#define READ        0xBE
#define WRITE       0x4E
#define EE_WRITE    0x48
#define EE_RECALL   0xB8
    
#define SEARCH_FIRST    0xFF        // start new search
#define PRESENCE_ERR    0xFF
#define DATA_ERR    0xFE
#define LAST_DEVICE 0x00        // last device found

void ow_setpin(uint8_t pin);
uint8_t ow_getpin(void);

uint8_t ow_reset(void);
uint8_t ow_bit_io(uint8_t bit);
uint8_t ow_write_byte(uint8_t b);
uint8_t ow_read_byte(void);
uint8_t ow_rom_search(uint8_t diff, uint8_t *id);
void ow_command(uint8_t cmd, uint8_t *id);
void ow_discover(const canix_frame *frame);

#endif
