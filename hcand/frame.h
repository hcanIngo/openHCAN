#ifndef FRAME_H
#define FRAME_H

#include "linuxinttypes.h"

typedef struct
{
	uint32_t id;
	uint32_t  size;
	uint8_t  data[8];
} CANFrame;

HCAN_PROTO(id) (id >> 20)


#endif
