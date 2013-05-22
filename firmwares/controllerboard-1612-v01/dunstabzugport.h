#ifndef DUNSTABZUGPORT_H
#define DUNSTABZUGPORT_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

typedef struct
{
    uint8_t type;
    eds_dunstabzugport_block_t config;
    uint16_t counter;
    uint8_t state_reed0;
    uint8_t state_reed1;
    uint8_t state_dunst;
} device_data_dunstabzugport;

void dunstabzugport_init(device_data_dunstabzugport *p, eds_block_p it);
void dunstabzugport_can_callback(device_data_dunstabzugport *p,
        const canix_frame *frame);
void dunstabzugport_timer_handler(device_data_dunstabzugport *p);

#endif
