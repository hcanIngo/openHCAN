#ifndef HEIZUNG_COMMAND_H
#define HEIZUNG_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

#define HEIZUNG_MODE_OFF 0
#define HEIZUNG_MODE_MANUELL 1
#define HEIZUNG_MODE_THERMOSTAT 2
#define HEIZUNG_MODE_AUTO 3

typedef struct
{
	int mode; // 0 = aus, 1 = manuell, 2 = thermostat, 3 = auto
	int dauer; // in minuten
	int dauer_min, dauer_sec;
	std::string dauer_str;
	float soll_temp, ist_temp;
	int stellgrad;
	int zz_id; // Zeitzone ID
} heizung_info_t;

class heizung_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	private:
		void print_heizung(uint8_t id);
		void print_heizungen();
		float get_ist_temp(uint8_t id);
		heizung_info_t get_details(uint8_t id);
		void print_header();
	public:
		heizung_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~heizung_command();
		virtual bool exec(const string &line);
};

#endif
