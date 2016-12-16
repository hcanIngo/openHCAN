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
#define ERROR_CONF_REEDKONTAKTE_MISSING    7
#define ERROR_CONF_LAMPEN_MISSING    	   8
#define ERROR_CONF_SONSTIGE_MISSING        9
#define ERROR_CONF_MUTE_MISSING        10
#define ERROR_CONF_HEIZUNG_MISSING        11

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

typedef struct
{
	uint8_t reed[24];
} userpanel_reedkontakte_t;

typedef struct
{
	uint8_t lampe[24];
} userpanel_lampen_t;

typedef struct
{
	uint8_t sonstiges[24];
} userpanel_sonstige_t;

typedef struct
{
	uint8_t mute[24];
} userpanel_mute_t;

typedef struct
{
	uint8_t heizung[24];
} userpanel_heizungen_t;

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

void print_screensaver_page(void);

#endif
