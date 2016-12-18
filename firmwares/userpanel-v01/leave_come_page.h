#ifndef LEAVE_COME_PAGE_H
#define LEAVE_COME_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

#define STATE_LEAVE 0
#define STATE_COME 1
#define STATE_NO_ACTION 2

#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ      1

void leave_come_page_handle_key_down_event(eds_leave_come_page_block_t *p, uint8_t key);
void leave_come_page_print_page(eds_leave_come_page_block_t *p);

#endif
