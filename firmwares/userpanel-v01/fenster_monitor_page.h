#ifndef FENSTER_MONITOR_PAGE_H
#define FENSTER_MONITOR_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void fenster_monitor_page_handle_key_down_event(
		eds_fenster_monitor_page_block_t *p, uint8_t key);
void fenster_monitor_page_print_page(eds_fenster_monitor_page_block_t *p);
void fenster_monitor_page_can_callback(eds_fenster_monitor_page_block_t *p,
		const canix_frame *frame);

#endif
