#ifndef FRAME_H
#define FRAME_H

typedef struct
{
	uint32_t id;
	uint32_t  size;
	uint8_t  data[8];
} CANFrame; // = HCAN-Frame

#define HCAN_PROTO(id) (id >> 20)


#endif
