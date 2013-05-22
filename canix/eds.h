#ifndef EDS_H
#define EDS_H

#ifndef CANIX_EDS
#error "CANIX_EDS muss definiert werden"
#endif

#include <inttypes.h>
#include <eds_types.h>
#include <eds_blocks.h>
#include <canix/canix.h>

#define EDS_START ((uint8_t *) 32)
#define EDS_DATA_START (EDS_START + 2)

#ifdef MCU_atmega32
	#define EDS_DATA_END 1023 //0x3ff
#elif MCU_atmega644 //mit doppelter EEPROM- und SRAM-Groesse
	#define EDS_DATA_END 2047 //0x7ff
#endif
#define EDS_END   ((uint8_t *) EDS_DATA_END)

// Rueckgabewerte des EDS Routinen:
#define EDS_OK                        0
#define EDS_NOT_FORMATED              1
#define EDS_INVALID_POINTER           2

// Hilfsmacros
#define EDS_BLOCK_TYPE(p) (eeprom_read_byte(p))
#define EDS_BLOCK_IS_FREE(p) (eeprom_read_byte(p) == 0)
#define EDS_BLOCK_SIZE(p) (eeprom_read_byte(p + 1))
#define EDS_SET_BLOCK_SIZE(p,t) (eeprom_write_byte(p + 1, t))
#define EDS_SET_BLOCK_TYPE(p,t) (eeprom_write_byte(p,t))
#define EDS_BLOCK_DATA(p,i) (eeprom_read_byte(p + 2 + i))
#define EDS_SET_BLOCK_DATA(p,i,v) (eeprom_write_byte(p + 2 + i, v))

// Iteriert ueber den EDS Space und setzt it jedes Mal auf den Block,
// wenn die Block-ID zwischen a und b liegt.
// Beispiel siehe firmwares/userpanel-v01/lcdstatemachine.c:97
#define EDS_foreach_block_between(it,a,b) \
	for (it = eds_find_next_block_between(0, \
		a, b); it != 0; it = eds_find_next_block_between(it,a,b))

// Hilfsmacros fuer page blocks
#define EDS_PAGE_ID(p) (eeprom_read_byte(p + 2))
#define EDS_PAGE_PARENT(p) (eeprom_read_byte(p + 3))
#define EDS_PAGE_NEXT(p) (eeprom_read_byte(p + 4))
#define EDS_PAGE_PREV(p) (eeprom_read_byte(p + 5))


uint8_t eds_init(void);
void eds_defragment(void);

uint8_t *eds_alloc(uint8_t size);
uint8_t eds_free(uint8_t *block);

void eds_read_string(uint8_t *block, char *s, uint8_t len);
uint8_t *eds_find_next_block(uint8_t *p, uint8_t type);
uint8_t *eds_find_next_block_between(uint8_t *p, uint8_t type1, uint8_t type2);

void eds_can_callback(const canix_frame *frame);

#endif
