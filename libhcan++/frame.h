#ifndef HCAN_FRAME_H
#define HCAN_FRAME_H

#include <inttypes.h>
#include <libhcan++/traceable_error.h>
#include <vector>
#include <frame_message_description.h>
#include <hcan_multicast.h>
#include <hcan.h>
#include <colors.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using namespace std;

namespace hcan
{

	class io_error : public traceable_error
	{
		public:
			io_error (const string &s);
	};

	class frame
	{
		typedef struct
		{
			uint32_t id;
			uint32_t  size;
			uint8_t  data[8];
		} CANFrame;

		public:
			enum prio { prio_hi = 0, prio_lo = 3 }; 
			/** 
			 * Wenn im Hoeflichkeits-Modus, dann die Sendefrequenz
			 * reduzieren. Somit kommen andere Busteilnehmer auch noch
			 * zum Zuge
			 */
			static bool polite_mode;
			/**
			 * Das ist die Wartezeit im Polite-Mode, d.h. die Zeitspanne
			 * in usec, die mit usleep() vor jedem Frame Senden gewartet
			 * wird.
			 */
			static int polite_wait_time;
		private:
			/** HCAN Source Address (1..1023) */
			uint16_t m_src;
			/** HCAN Destination Address (1..1023) */
			uint16_t m_dst;
			/** HCAN Protocol (0..7) */
			uint8_t m_proto;
			/** HCAN Bus Arbitierungsprioritaet */
			uint8_t m_prio;
			/** CAN Frames haben max 8 Bytes Payload */
			uint8_t m_data[8];
			/** size (count of data bytes); maximal 8  */
			uint32_t m_size;
		public:
			frame();
			/** constuctor for normal use */
			frame(const uint16_t src, const uint16_t dst, 
					const uint8_t proto, const uint8_t data[8],
					const uint32_t size);
			frame(const uint16_t src, const uint16_t dst, 
					const uint8_t proto, const vector<uint8_t> &data);
			/** Copy constuctor */
			frame(const frame &f);
			/** Assign operator */
			frame &operator = (const frame &f);
			virtual ~frame() throw ();
			void write_to(int fd, const in_addr_t hcand_ip) const;
			static frame read_from(int fd);
			uint8_t proto() const { return m_proto; };
			void set_proto(uint8_t proto) { m_proto = proto; };
			uint16_t src() const { return m_src; };
			void set_src(uint16_t src) { m_src = src; };
			uint16_t dst() const { return m_dst; };
			void set_dst(uint16_t dst) { m_dst = dst; };
			uint8_t data(const int i) const;
			void set_data(const int i, const uint8_t data);
			const string data_as_string() const;
			uint32_t size() const { return m_size; };
			void set_size(uint32_t size) { m_size = size; };
			void set_prio(uint8_t prio);
			void print(bool numeric, bool color, 
					const std::string &prefix = "") const;
	};
};

#endif
