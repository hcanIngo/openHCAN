#ifndef TASTER_PAGE_H
#define TASTER_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void taster_page_handle_key_down_event(eds_taster_page_block_t *p, 
		uint8_t key);
void taster_page_print_page(eds_taster_page_block_t *p);
void taster_page_can_callback(eds_taster_page_block_t *p,
		const canix_frame *frame);

#endif
