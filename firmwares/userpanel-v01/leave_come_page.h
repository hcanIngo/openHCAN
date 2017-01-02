#ifndef LEAVE_COME_PAGE_H
#define LEAVE_COME_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

#define LEAVE_HEATED 0 // bei aktiver Heizung
#define LEAVE_HEATING_IS_OFF 1 // Heizung ist abgeschaltet
#define COME_HEATED 2 // bei aktiver Heizung
#define COME_HEATING_IS_OFF 3 // Heizung ist abgeschaltet
#define NO_ACTION 4

#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ      1

void leave_come_page_handle_key_down_event(eds_leave_come_page_block_t *p, uint8_t key);
void leave_come_page_print_page(eds_leave_come_page_block_t *p);
uint8_t leave_come_page_get_ignore(uint8_t gruppe);

#endif
