
// ------- AUTOMATICLY GENERATED FROM mk_dump_data_h.xsl -----------------


#include <string>
#include <vector>
#include <frame_message_description.h>

using namespace std;

vector < frame_message_description_t > frame_message_description;

void
init_frame_message_description()
{


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 1;
	msg.command_name = string("PING_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 2;
	msg.command_name = string("PING_REPLAY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 3;
	msg.command_name = string("DEVICE_STATE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 4;
	msg.command_name = string("DEVICE_STATE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("state");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 5;
	msg.command_name = string("DEVICE_RESET");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 6;
	msg.command_name = string("DEVICE_BOOT_STOP");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 7;
	msg.command_name = string("DEVICE_BOOT_STOP_ACK");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 8;
	msg.command_name = string("DEVICE_LOAD_APP");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 9;
	msg.command_name = string("DEVICE_TYPE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 10;
	msg.command_name = string("DEVICE_TYPE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("arch");

	msg.param_names.push_back("type");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 16;
	msg.command_name = string("FLASH_BUFFER_FILL");
	msg.details = string("");


	msg.param_names.push_back("index");

	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 17;
	msg.command_name = string("FLASH_BUFFER_FILL_ACK");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 18;
	msg.command_name = string("FLASH_BUFFER_WRITE");
	msg.details = string("");


	msg.param_names.push_back("addr_lsb");

	msg.param_names.push_back("addr_msb");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 19;
	msg.command_name = string("FLASH_BUFFER_WRITE_ACK");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 20;
	msg.command_name = string("INTERNAL_EEPROM_WRITE");
	msg.details = string("");


	msg.param_names.push_back("addr_lsb");

	msg.param_names.push_back("addr_msb");

	msg.param_names.push_back("value");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 21;
	msg.command_name = string("INTERNAL_EEPROM_WRITE_ACK");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 22;
	msg.command_name = string("INTERNAL_EEPROM_READ_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("addr_lsb");

	msg.param_names.push_back("addr_msb");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 23;
	msg.command_name = string("INTERNAL_EEPROM_READ_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("value");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 24;
	msg.command_name = string("INTERNAL_EEPROM_READ_BULK_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("addr_lsb");

	msg.param_names.push_back("addr_msb");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 25;
	msg.command_name = string("INTERNAL_EEPROM_READ_BULK_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	msg.param_names.push_back("d4");

	msg.param_names.push_back("d5");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 30;
	msg.command_name = string("UPTIME_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 31;
	msg.command_name = string("UPTIME_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("u3");

	msg.param_names.push_back("u2");

	msg.param_names.push_back("u1");

	msg.param_names.push_back("u0");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 32;
	msg.command_name = string("SYSLOG_LEVEL_SET");
	msg.details = string("");


	msg.param_names.push_back("level");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 33;
	msg.command_name = string("BUILD_VERSION_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 34;
	msg.command_name = string("BUILD_VERSION_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("hi");

	msg.param_names.push_back("lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 35;
	msg.command_name = string("CAN_EC_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 36;
	msg.command_name = string("CAN_EC_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("REC");

	msg.param_names.push_back("TEC");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 37;
	msg.command_name = string("FLASH_CRC16_QUERY");
	msg.details = string("");


	msg.param_names.push_back("hi");

	msg.param_names.push_back("lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 38;
	msg.command_name = string("FLASH_CRC16_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("hi");

	msg.param_names.push_back("lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 39;
	msg.command_name = string("LOOP_THROUGHPUT_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 40;
	msg.command_name = string("LOOP_THROUGHPUT_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("hi");

	msg.param_names.push_back("lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 41;
	msg.command_name = string("RX_RECEIVED_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 42;
	msg.command_name = string("RX_RECEIVED_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 43;
	msg.command_name = string("RX_LOST_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 44;
	msg.command_name = string("RX_LOST_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 45;
	msg.command_name = string("TX_SENT_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 46;
	msg.command_name = string("TX_SENT_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 47;
	msg.command_name = string("TX_DROPPED_QUERY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 48;
	msg.command_name = string("TX_DROPPED_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 1;
	msg.service_name = string("HMS");
	msg.command = 49;
	msg.command_name = string("RX_TX_STATS_RESET");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 4;
	msg.service_name = string("SLS");
	msg.command = 1;
	msg.command_name = string("BOOT_RESETFLAG_LOG");
	msg.details =
	    string
	    ("1: Power-on Reset 2: External Reset 4: Brown-Out Reset 8: Watchdog Reset");


	msg.param_names.push_back("flag");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 4;
	msg.service_name = string("SLS");
	msg.command = 2;
	msg.command_name = string("FIRMWARE_CRC16_ERROR");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 4;
	msg.service_name = string("SLS");
	msg.command = 3;
	msg.command_name = string("FIRMWARE_CONFIG_COMPAT_ERROR");
	msg.details = string("");


	msg.param_names.push_back("eds_block_type");

	msg.param_names.push_back("eds_addr_hi");

	msg.param_names.push_back("eds_addr_lo");

	msg.param_names.push_back("size_in_config");

	msg.param_names.push_back("size_in_firmware");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 1;
	msg.command_name = string("TASTER_DOWN");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("index");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 2;
	msg.command_name = string("TASTER_UP");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("index");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 10;
	msg.command_name = string("POWER_GROUP_ON");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 11;
	msg.command_name = string("POWER_GROUP_OFF");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 12;
	msg.command_name = string("POWER_GROUP_STATE_QUERY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 13;
	msg.command_name = string("POWER_GROUP_STATE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("status");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 14;
	msg.command_name = string("POWER_GROUP_TIMER_QUERY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 15;
	msg.command_name = string("POWER_GROUP_TIMER_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("timer_hi");

	msg.param_names.push_back("timer_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 16;
	msg.command_name = string("POWER_GROUP_SET_TIMER");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("timer_hi");

	msg.param_names.push_back("timer_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 17;
	msg.command_name = string("POWER_GROUP_STATE_INFO");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("status");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 20;
	msg.command_name = string("ROLLADEN_POSITION_SET");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("position");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 21;
	msg.command_name = string("ROLLADEN_POSITION_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 22;
	msg.command_name = string("ROLLADEN_POSITION_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("position");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 23;
	msg.command_name = string("ROLLADEN_DEFINE_POSITION");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("position");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 30;
	msg.command_name = string("1WIRE_DISCOVER");
	msg.details = string("");


	msg.param_names.push_back("pin");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 31;
	msg.command_name = string("1WIRE_DISCOVERED_PART_1");
	msg.details = string("");


	msg.param_names.push_back("id0");

	msg.param_names.push_back("id1");

	msg.param_names.push_back("id2");

	msg.param_names.push_back("id3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 32;
	msg.command_name = string("1WIRE_DISCOVERED_PART_2");
	msg.details = string("");


	msg.param_names.push_back("id4");

	msg.param_names.push_back("id5");

	msg.param_names.push_back("id6");

	msg.param_names.push_back("id7");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 33;
	msg.command_name = string("1WIRE_ERROR");
	msg.details = string("");


	msg.param_names.push_back("error");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 34;
	msg.command_name = string("1WIRE_TEMPERATURE");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 35;
	msg.command_name = string("1WIRE_TEMPERATURE_QUERY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 36;
	msg.command_name = string("1WIRE_TEMPERATURE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 40;
	msg.command_name = string("REEDKONTAKT_OFFEN");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 41;
	msg.command_name = string("REEDKONTAKT_STATE_QUERY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 42;
	msg.command_name = string("REEDKONTAKT_STATE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("state");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 43;
	msg.command_name = string("REEDKONTAKT_STATE_CHANGE");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("state");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 50;
	msg.command_name = string("HEIZUNG_DETAILS_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 51;
	msg.command_name = string("HEIZUNG_MODE_OFF_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 52;
	msg.command_name = string("HEIZUNG_SET_MODE_OFF");
	msg.details = string("");


	msg.param_names.push_back("id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 53;
	msg.command_name = string("HEIZUNG_MODE_MANUAL_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("rate");

	msg.param_names.push_back("duration_hi");

	msg.param_names.push_back("duration_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 54;
	msg.command_name = string("HEIZUNG_SET_MODE_MANUAL");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("rate");

	msg.param_names.push_back("duration_hi");

	msg.param_names.push_back("duration_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 55;
	msg.command_name = string("HEIZUNG_MODE_THERMOSTAT_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	msg.param_names.push_back("duration_hi");

	msg.param_names.push_back("duration_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 56;
	msg.command_name = string("HEIZUNG_SET_MODE_THERMOSTAT_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	msg.param_names.push_back("duration_hi");

	msg.param_names.push_back("duration_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 57;
	msg.command_name = string("HEIZUNG_MODE_AUTOMATIK_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	msg.param_names.push_back("timerange_id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 58;
	msg.command_name = string("HEIZUNG_SET_MODE_AUTOMATIK");
	msg.details = string("");


	msg.param_names.push_back("id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 59;
	msg.command_name = string("HEIZUNG_TIST_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 60;
	msg.command_name = string("HEIZUNG_TIST_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 61;
	msg.command_name = string("HEIZUNG_WAERMEBEDARF_INFO");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("value");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 62;
	msg.command_name = string("HEIZSTEUERUNG_STATE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 63;
	msg.command_name = string("HEIZSTEUERUNG_STATE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("VL_soll");

	msg.param_names.push_back("VL_ist");

	msg.param_names.push_back("RL_ist");

	msg.param_names.push_back("BF");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 64;
	msg.command_name = string("HEIZSTEUERUNG_STATE_INFO");
	msg.details = string("");


	msg.param_names.push_back("VL_soll");

	msg.param_names.push_back("GWB_hi");

	msg.param_names.push_back("GWB_lo");

	msg.param_names.push_back("BF");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 65;
	msg.command_name = string("HEIZSTEUERUNG_SET_VL");
	msg.details = string("");


	msg.param_names.push_back("VL_soll");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 66;
	msg.command_name = string("HEIZUNG_SOLLTEMP_LINE_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("idx");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 67;
	msg.command_name = string("HEIZUNG_SOLLTEMP_LINE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("idx");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	msg.param_names.push_back("zeitzone_id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 68;
	msg.command_name = string("HEIZUNG_SOLLTEMP_WRITE_LINE");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("idx");

	msg.param_names.push_back("temp_hi");

	msg.param_names.push_back("temp_lo");

	msg.param_names.push_back("zeitzone_id");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 69;
	msg.command_name = string("HEIZUNG_SOLLTEMP_WRITE_LINE_ACK");
	msg.details = string("");


	msg.param_names.push_back("id");

	msg.param_names.push_back("idx");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 100;
	msg.command_name = string("CONFIG_RELOAD");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 101;
	msg.command_name = string("CONFIG_RAM_USAGE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 102;
	msg.command_name = string("CONFIG_RAM_USAGE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("usage_hi");

	msg.param_names.push_back("usage_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 110;
	msg.command_name = string("ZEITZONE_DETAILS_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 111;
	msg.command_name = string("ZEITZONE_DETAILS_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("day_pattern");

	msg.param_names.push_back("from_hour");

	msg.param_names.push_back("from_minute");

	msg.param_names.push_back("to_hour");

	msg.param_names.push_back("to_minute");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 112;
	msg.command_name = string("ZEITZONE_WRITE_DETAILS");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("day_pattern");

	msg.param_names.push_back("from_hour");

	msg.param_names.push_back("from_minute");

	msg.param_names.push_back("to_hour");

	msg.param_names.push_back("to_minute");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 113;
	msg.command_name = string("ZEITZONE_WRITE_DETAILS_ACK");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 114;
	msg.command_name = string("ZEITZONE_TEST_MATCH_REQUERST");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 5;
	msg.service_name = string("HES");
	msg.command = 115;
	msg.command_name = string("ZEITZONE_TEST_MATCH_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("gruppe");

	msg.param_names.push_back("match");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 6;
	msg.service_name = string("RTS");
	msg.command = 1;
	msg.command_name = string("TIME_INFO");
	msg.details = string("");


	msg.param_names.push_back("level");

	msg.param_names.push_back("day_of_week");

	msg.param_names.push_back("hour");

	msg.param_names.push_back("minute");

	msg.param_names.push_back("second");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 6;
	msg.service_name = string("RTS");
	msg.command = 2;
	msg.command_name = string("DATE_INFO");
	msg.details = string("");


	msg.param_names.push_back("level");

	msg.param_names.push_back("day_of_month");

	msg.param_names.push_back("month_of_year");

	msg.param_names.push_back("year");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 6;
	msg.service_name = string("RTS");
	msg.command = 3;
	msg.command_name = string("TIME_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 6;
	msg.service_name = string("RTS");
	msg.command = 4;
	msg.command_name = string("DATE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 1;
	msg.command_name = string("GET_NEXT_BLOCK");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 2;
	msg.command_name = string("GET_NEXT_BLOCK_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	msg.param_names.push_back("type");

	msg.param_names.push_back("size");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 3;
	msg.command_name = string("DEFRAGMENT");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 4;
	msg.command_name = string("ALLOCATE_BLOCK");
	msg.details = string("");


	msg.param_names.push_back("type");

	msg.param_names.push_back("size");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 5;
	msg.command_name = string("ALLOCATE_BLOCK_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 6;
	msg.command_name = string("FREE_BLOCK");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 7;
	msg.command_name = string("FREE_BLOCK_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("result");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 8;
	msg.command_name = string("READ_FROM_BLOCK");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	msg.param_names.push_back("index");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 9;
	msg.command_name = string("READ_FROM_BLOCK_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("d0");

	msg.param_names.push_back("d1");

	msg.param_names.push_back("d2");

	msg.param_names.push_back("d3");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 10;
	msg.command_name = string("WRITE_TO_BLOCK");
	msg.details = string("");


	msg.param_names.push_back("handle_hi");

	msg.param_names.push_back("handle_lo");

	msg.param_names.push_back("index");

	msg.param_names.push_back("d0");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 11;
	msg.command_name = string("WRITE_TO_BLOCK_REPLAY");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 7;
	msg.service_name = string("EDS");
	msg.command = 12;
	msg.command_name = string("FORMAT");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 1;
	msg.command_name = string("STATE_REQUEST");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 2;
	msg.command_name = string("STATE_INFO");
	msg.details = string("");


	msg.param_names.push_back("state");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 3;
	msg.command_name = string("VOLTAGE_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("Ux");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 4;
	msg.command_name = string("VOLTAGE_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("Ux_hi");

	msg.param_names.push_back("Ux_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 5;
	msg.command_name = string("VOLTAGE_STATS_REQUEST");
	msg.details = string("");


	msg.param_names.push_back("Ux");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 6;
	msg.command_name = string("VOLTAGE_STATS_REPLAY");
	msg.details = string("");


	msg.param_names.push_back("U");

	msg.param_names.push_back("min_hi");

	msg.param_names.push_back("min_lo");

	msg.param_names.push_back("max_hi");

	msg.param_names.push_back("max_lo");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 8;
	msg.service_name = string("USVS");
	msg.command = 7;
	msg.command_name = string("VOLTAGE_STATS_RESET");
	msg.details = string("");


	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 9;
	msg.service_name = string("EBUS");
	msg.command = 1;
	msg.command_name = string("FA_BLOCK1_INFO");
	msg.details = string("");


	msg.param_names.push_back("status");

	msg.param_names.push_back("stellgrad");

	msg.param_names.push_back("KT");

	msg.param_names.push_back("RT");

	msg.param_names.push_back("BT");

	msg.param_names.push_back("AT");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 9;
	msg.service_name = string("EBUS");
	msg.command = 2;
	msg.command_name = string("REGLER_DATEN_INFO_FRG1");
	msg.details = string("");


	msg.param_names.push_back("status");

	msg.param_names.push_back("aktion");

	msg.param_names.push_back("kesseltemp_soll_lo");

	msg.param_names.push_back("kesseltemp_soll_hi");

	msg.param_names.push_back("kesseldr_soll_lo");

	msg.param_names.push_back("kesseldr_soll_hi");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 9;
	msg.service_name = string("EBUS");
	msg.command = 3;
	msg.command_name = string("REGLER_DATEN_INFO_FRG2");
	msg.details = string("");


	msg.param_names.push_back("stellgrad");

	msg.param_names.push_back("ww_soll");

	msg.param_names.push_back("brennstoff");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 10;
	msg.service_name = string("WS");
	msg.command = 1;
	msg.command_name = string("WETTER_INFO");
	msg.details = string("");


	msg.param_names.push_back("licht");

	msg.param_names.push_back("niederschlag");

	msg.param_names.push_back("wind");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 10;
	msg.service_name = string("WS");
	msg.command = 2;
	msg.command_name = string("LICHT_STATS_5MIN");
	msg.details = string("");


	msg.param_names.push_back("licht");

	frame_message_description.push_back(msg);
    };


    {
	frame_message_description_t msg;
	msg.proto = 1;
	msg.service = 10;
	msg.service_name = string("WS");
	msg.command = 3;
	msg.command_name = string("WIND_STATS_5MIN");
	msg.details = string("");


	msg.param_names.push_back("wind");

	frame_message_description.push_back(msg);
    };


};
