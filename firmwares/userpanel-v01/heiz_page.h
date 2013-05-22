#ifndef HEIZ_PAGE_H
#define HEIZ_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void heiz_page_handle_key_down_event(eds_heiz_page_block_t *p, 
		uint8_t key);
void heiz_page_print_page(eds_heiz_page_block_t *p);
void heiz_page_can_callback(eds_heiz_page_block_t *p,
		const canix_frame *frame);

#endif
