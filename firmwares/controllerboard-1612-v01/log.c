#include "log.h"
#include <canix/canix.h>
#include <canix/tools.h>
#include <hcan.h>

void send_log(uint8_t data, uint8_t data2)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = 40;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = 21;
	answer.data[1] = data;
	answer.data[2] = data2;
	answer.size = 3;
	canix_frame_send(&answer);
	delay(1);
}

void send_log2(uint8_t data, uint16_t data2)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = 40;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = 21;
	answer.data[1] = data;
	answer.data[2] = data2 >> 8;
	answer.data[3] = data2 ;
	answer.size = 4;
	canix_frame_send(&answer);
	delay(1);
}

