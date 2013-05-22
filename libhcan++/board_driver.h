#ifndef BOARD_DRIVER_H
#define BOARD_DRIVER_H

#include <memory>
#include <string>
#include <inttypes.h>

using namespace std;

namespace hcan
{
	class board_connection;
	class transport_connection;

	struct context
	{
		string prompt;
		string std_prompt;
		uint16_t mode;
		uint16_t eds_block_address;
		uint16_t oap_oid;
		enum { normal = 1, edit, oap };

		context ()
		{
			std_prompt = "> ";
			reset();
		}

		void reset()
		{
			prompt = std_prompt;
			mode = normal;
		}
	};

	class board_driver
	{
		protected:
			board_connection &m_bcon;
			transport_connection &m_tcon;
			uint16_t m_page_size;
		protected:
			virtual uint8_t read_eeprom_byte(const uint16_t address);
			virtual uint16_t read_eeprom_word_le(const uint16_t address);
			virtual uint16_t read_eeprom_word_be(const uint16_t address);

			virtual void write_eeprom_byte(const uint16_t address, 
					const uint8_t data);
			virtual void write_eeprom_word_le(const uint16_t address, 
					const uint16_t data);
			virtual void write_eeprom_word_be(const uint16_t address, 
					const uint16_t data);
			virtual void cmd_flash (const string &filename);
			virtual uint16_t cmd_getaddress ();
			virtual void cmd_setaddress (const uint16_t address);
			virtual void cmd_reset();
			virtual uint32_t cmd_getuptime();
			virtual uint8_t cmd_getstate();
			virtual void cmd_dump_ee(uint16_t address);
			virtual void cmd_setsysloglevel(uint16_t level);
			virtual void cmd_loadapp();
			virtual void cmd_bootloader();
			virtual void cmd_show_system();
			virtual void cmd_show_time();
			virtual bool show_help();
			const std::string cached_filename() const;
			uint16_t load_crc16_from_board(uint16_t size) const;
			void save_crc16_to_board(uint16_t crc16, uint16_t size);
		public:
			board_driver (board_connection &bcon);
			virtual ~board_driver ();
			virtual bool exec_command (context &c, const string &cmd);
	};

	typedef std::auto_ptr<board_driver> board_driver_p;
};

// Experimentelles Feature
extern bool incremental_flash;

#endif
