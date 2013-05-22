#ifndef ROLLADEN_PAGE_H
#define ROLLADEN_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void rolladen_page_handle_key_down_event(eds_rolladen_page_block_t *p, 
		uint8_t key);
void rolladen_page_print_page(eds_rolladen_page_block_t *p);
void rolladen_page_can_callback(eds_rolladen_page_block_t *p,
		const canix_frame *frame);

#endif
