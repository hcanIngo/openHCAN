#ifndef TEMP_ANZEIGE_H
#define TEMP_ANZEIGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void temp_anzeige_page_handle_key_down_event(
		eds_temp_anzeige_page_block_t *p, uint8_t key);
void temp_anzeige_page_print_page(eds_temp_anzeige_page_block_t *p);
void temp_anzeige_page_can_callback(eds_temp_anzeige_page_block_t *p,
		const canix_frame *frame);

#endif
