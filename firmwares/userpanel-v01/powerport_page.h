#ifndef POWERPORT_PAGE_H
#define POWERPORT_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void powerport_page_handle_key_down_event(eds_powerport_page_block_t *p, 
		uint8_t key);
void powerport_page_print_page(eds_powerport_page_block_t *p);
void powerport_page_can_callback(eds_powerport_page_block_t *p,
		const canix_frame *frame);

#endif
