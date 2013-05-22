#ifndef LCDSTATEMACHINE_H
#define LCDSTATEMACHINE_H

#include <inttypes.h>
#include <canix/canix.h>

#define EDS_PAGE_ID_START                100
#define EDS_PAGE_ID_END                  149

#define ERROR_PAGE_SIZE_TOO_LARGE          1
#define ERROR_PAGE_PAGE_NOT_FOUND          2
#define ERROR_PAGE_CHILD_PAGE_NOT_FOUND    3
#define ERROR_PAGE_PAGE_STACK_OVERFLOW     4
#define ERROR_PAGE_PAGE_STACK_UNDERFLOW    5
#define ERROR_PAGE_SHORTCUTS_MISSING       6

typedef struct
{
	uint8_t block_type; // EDS block type
	uint8_t block_size; // EDS block size

	// Page Daten
	uint8_t page_id;
	uint8_t parent_id;
	uint8_t next_id;
	uint8_t prev_id;

	// Dummy Data, nur zum Reservieren von Platz
	uint8_t data[192];
} lcdpage_t;

typedef struct
{
	uint8_t state;
	uint8_t value;
	uint8_t temp_value;
	lcdpage_t active_page;
} lcdstate_t;

typedef struct
{
	//uint8_t block_type; // EDS block type
	//uint8_t block_size; // EDS block size

	uint8_t F[8];
} userpanel_shortcuts_t;

extern lcdstate_t lcdstate;

#define STATE_VIEW                         0
#define STATE_EDIT                         1

void key_down_event(uint8_t key);
void key_up_event(uint8_t key);
void lcdstatemachine_init(void);
void goto_page_up(void);
void goto_next_page(void);
void goto_prev_page(void);
void goto_page(uint8_t page);
void load_error_page(uint8_t code);

// wird im Sekundentakt aufgerufen
void lcdstatemachine_timer_handler(void);
void lcdstatemachine_can_callback(const canix_frame *frame);

#endif
