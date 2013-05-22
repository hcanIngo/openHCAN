#include <board_driver.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <frame.h>
#include <ihexfile.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <hcan_eeprom.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace hcan;
using namespace std;

bool incremental_flash = false;

board_driver::board_driver(board_connection &bcon) :
	m_bcon(bcon),
	m_tcon(bcon.get_transport_connection()),
	m_page_size(0)
{
}

board_driver::~board_driver ()
{
}

bool board_driver::show_help ()
{
	cout << endl <<
		"	send <d0> <d1> ...                 sends this raw frame to peer\n" <<
		"	show system                        prints info about peer\n" <<
		"	show uptime                        prints the uptime\n" <<
		"	show time                          prints the time/date\n" <<
		"	show address                       get the stored hcan address\n" <<
		"	show state                         bootloader, booting, app\n" <<
		"	show ee <address>                  shows a eeprom value\n" <<
		"	dump ee <address>                  dumps a eeprom range\n" <<
		"	set ee <address> <value>           save value to eeprom\n" <<
		"	set address <hcan-address>         save a new hcan address\n" <<
		"	set sysloglevel <level>            set the syslog level\n" <<
		"	bootloader                         boot into bootloader\n" <<
		"	loadapp                            load the application\n" <<
		"	reset                              generated a reset\n" << endl;

	return true;
}

vector<uint8_t> parse_raw_send_list(const string &list)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(" ");
	tokenizer tokens(list, sep);
	vector<uint8_t> result;

	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
	{
		string s = *tok_iter;
		boost::trim(s);

		stringstream ss(s);
		uint16_t i; // not uint8_t otherwise converion to char happens
		ss >> i;
		result.push_back(i);
	}

	return result;
}


bool board_driver::exec_command (context &c, const string &command)
{
	// Falls es eine Leerzeile ist, ignorieren
	if (command == "")
		return true;

	// Falls es eine Kommentarzeile ist, ignorieren
	if (command[0] == '#')
		return true;

	istringstream sin(command);

	string s;
	sin >> s;

	if (s == "show")
	{
		sin >> s;

		if (s == "address")
		{
			cout << (uint16_t)cmd_getaddress() << endl;
			return true;
		}

		if (s == "state")
		{
			switch (cmd_getstate())
			{
				case 0 : cout << "device is booting..." << endl; break;
				case 1 : cout << "bootloader is active." << endl; break;
				case 2 : cout << "application is active." << endl; break;
			}
			return true;
		}

		if (s == "uptime")
		{
			uint32_t uptime = cmd_getuptime();

			int days = uptime / (3600*24);
			uptime -= days * (3600*24);

			int hours = uptime / 3600;
			uptime -= hours * 3600;

			int minutes = uptime / 60;
			uptime -= minutes * 60;

			cout << days << " days, "
				<< hours << ":" << setw(2) << setfill ('0')
				<< minutes << endl;

			return true;
		}

		if (s == "ee")
		{
			uint16_t address;
			sin >> address;
			cout << (uint16_t)read_eeprom_byte(address) << endl;
			return true;
		}

		if (s == "system")
		{
			cmd_show_system();
			return true;
		}

		if (s == "time")
		{
			cmd_show_time();
			return true;
		}
	}

	if (s == "set")
	{
		sin >> s;

		if (s == "ee")
		{
			uint16_t address, value;
			sin >> address >> value;
			write_eeprom_byte(address, value);
			cout << (uint16_t)read_eeprom_byte(address) << endl;
			return true;
		}

		if (s == "address")
		{
			uint16_t value;
			sin >> value;
			cmd_setaddress(value);
			return true;
		}

		if (s == "sysloglevel")
		{
			uint16_t level;
			sin >> level;

			cmd_setsysloglevel(level);
			return true;
		}
	}

	if (s == "reset")
	{
		cmd_reset();
		return true;
	}

	if (s == "dump")
	{
		sin >> s;

		if (s == "ee")
		{
			uint16_t address;
			sin >> address;
			cmd_dump_ee(address);
			return true;
		}
	}

	if (s == "loadapp")
	{
		cmd_loadapp();
		return true;
	}

	if (s == "bootloader")
	{
		cmd_bootloader();
		return true;
	}

	if (s == "send")
	{
		string s;
		getline(sin,s);
		vector<uint8_t> params = parse_raw_send_list(s);
		m_tcon.send(frame(m_bcon.src(), m_bcon.dst(), 1, // SFP
					params));
		try
		{
			frame f = m_tcon.recv_frame(m_bcon.src());

			cout << "message: ";
			for (uint8_t i = 0; i < f.size(); i++)
				cout << (uint16_t)f.data(i) << " ";
			cout << endl;
		}
		catch (const transport_error &e)
		{
			cout << e << endl;
		}
		return true;
	}

	if (s == "help")
		return show_help();

	return false;
}

uint8_t board_driver::read_eeprom_byte(const uint16_t address)
{
	uint8_t value;

	m_tcon.send_INTERNAL_EEPROM_READ_REQUEST(
			m_bcon.src(), m_bcon.dst(), address & 0xff, address >> 8);
	m_tcon.recv_INTERNAL_EEPROM_READ_REPLAY(
			m_bcon.dst(), m_bcon.src(), &value);

	return value;
}

uint16_t board_driver::read_eeprom_word_le(const uint16_t address)
{
	return 
		(read_eeprom_byte(address)) | 
		(read_eeprom_byte(address + 1) << 8);
}

uint16_t board_driver::read_eeprom_word_be(const uint16_t address)
{
	return 
		(read_eeprom_byte(address) << 8) | 
		(read_eeprom_byte(address + 1));
}

void board_driver::write_eeprom_byte(const uint16_t address, 
	const uint8_t data)
{
	m_tcon.send_INTERNAL_EEPROM_WRITE(
			m_bcon.src(), m_bcon.dst(), address & 0xff, address >> 8, data);
	m_tcon.recv_INTERNAL_EEPROM_WRITE_ACK(
			m_bcon.dst(), m_bcon.src());
}

void board_driver::write_eeprom_word_le(const uint16_t address, 
	const uint16_t data)
{
	write_eeprom_byte(address, data);
	write_eeprom_byte(address + 1, data >> 8);
}

void board_driver::write_eeprom_word_be(const uint16_t address, 
	const uint16_t data)
{
	write_eeprom_byte(address, data >> 8);
	write_eeprom_byte(address + 1, data);
}

const std::string board_driver::cached_filename() const
{
	const char *homedir = getenv("HOME");

	stringstream ss;

	if (homedir)
	{
		ss << string(homedir) << "/.hcan/";

		// Verzeichnis anlegen (falls es schon existiert, schlaegt
		// der Befehl fehl:
		int res = mkdir(ss.str().c_str(), 0755);
		if ((res != 0) && (errno != EEXIST))
			throw traceable_error("error while creating $HOME/.hcan directory");
	}
	else
	{
		ss << "/tmp/";
	}

	ss << ".cached-firmware-" << (int)m_bcon.dst() << ".hex";

	return ss.str();
}


/**
 *
 * Inkrementelles Flashen:
 *
 * Es werden nur die seit dem letzten Flashen geaenderten Bloecke uebertragen
 * und in den Flash geschrieben:
 *
 * - nach jedem erfolgreichen Flashen wird das Hexfile im Flash-Cache
 *   gespeichert
 * - vor jedem Flashen wird die CRC16 Checksumme des Cache-Files mit
 *   der CRC16 Checksumme der Firmware auf dem Board, ebenso die
 *   (padded) Groessen verglichen
 * - stimmen sie ueberein, so ist die Wahrscheinlichkeit sehr sehr gross,
 *   dass die Firmware auf dem Board die gecachete Firmware ist
 * - beim Flashen wird nun jeder Block vor der Uebertragung geprueft;
 *   ist er gleich, wird er uebersprungen
 * - am Ende werden nochmals die CRC16 Summen verglichen
 *
 */

void board_driver::cmd_flash (const string &filename)
{
	// Verschiedene Atmega MCUs haben unterschiedliche Page-Sizes
	assert(m_page_size);

	// Das zu flashende Hexfile laden
	ihexfile hexfile;
	hexfile.load(filename,m_page_size);
	size_t size = hexfile.size();

	vector<uint8_t> data = hexfile.data();
	cout << filename << " read; padded size = "  << hexfile.size() 
		<< " bytes." << endl;
	cout << "crc16 is " << hexfile.crc16() << endl;

	bool incremental_flash_active = false;

	ihexfile cached_hexfile;
	if (incremental_flash)
	{
		try
		{
			cmd_reset();
			sleep(1);

			m_tcon.send_DEVICE_BOOT_STOP(m_bcon.src(), m_bcon.dst());
			m_tcon.recv_DEVICE_BOOT_STOP_ACK(m_bcon.dst(), m_bcon.src());

			assert(cmd_getstate() == 1);

			// gecachete Firmware laden
			cached_hexfile.load(cached_filename(), m_page_size);

			// Pruefsumme der Firmware des Target-Boards besorgen...
			uint16_t board_crc = load_crc16_from_board(size);

			// Pruefsumme mit der des gecachten Files vergleichen;
			// stimmen sie, so kann ein inkrementelles Flashen stattfinden:
			if (board_crc == cached_hexfile.crc16())
			{
				incremental_flash_active = true;
				cout << "cached file crc16 is correct, incremental mode enabled" << endl;
			}
			else
			{
				incremental_flash_active = false;
				cout << "cached file crc16 is not correct, incremental mode disabled" << endl;
			}
		}
		catch (ihexfile_error &e)
		{
			cout << "no cache file found. Using normal flash method..." << endl;
		}
	}

	
	// 3 Versuche; falls ein Timeout auftritt, wird nochmals versucht...
	int n_trys = 3;

	while (n_trys--)
	{
		try
		{
			/*
			 * Flash-Strategie:
			 *
			 * Der gesamte zu flashende Bereich (size) wird in
			 * Bloecke aufgeteilt. Jeder Block ist eine Groesse von m_page_size
			 * und ist vom Typ der MCU abhaengig (z.Z. nur Atmega32).
			 *
			 * Jeder Block muss auf der MCU "auf einen Rutsch" ins Flash
			 * geschrieben werden. Dazu wird der Block bei der Uebertragung
			 * in sogenannte Lines zu je 16 Bytes aufgeteilt und uebertragen.
			 * Jede Line wird mit 4 FLASH_BUFFER_FILL Frames uebertragen und
			 * mit einem FLASH_BUFFER_FILL_ACK vom Board bestaetigt.
			 *
			 * Ist der komplette Block, bestehend aus mehreren Lines, uebertragen,
			 * so wird er auf der MCU mit FLASH_BUFFER_WRITE ins Flash
			 * geschrieben. Die MCU bestaetigt mit FLASH_BUFFER_WRITE_ACK.
			 *
			 * Inkrementeller Modus:
			 *
			 * Hier wird nun jeder Block vor der Uebertragung mit der
			 * gecachten Version verglichen; unterscheidet sich auch nur ein
			 * Byte, so wird er komplett uebertragen.
			 *
			 */
			for (uint32_t block = 0; block < (size / m_page_size); block++)
			{
				int offset = block * m_page_size;

				if (incremental_flash_active)
				{
					// Block mit der gecachten Version vergleichen:

					bool blocks_equal = true;
					for (int i = 0; i < m_page_size; i++)
					{
						if (data[offset+i] != cached_hexfile.data()[offset+i])
						{
							blocks_equal = false;
							break;
						}
					}

					if (blocks_equal)
					{
						cout << "O";
						cout.flush();

						// diesen Block auslassen:
						continue;
					}

				}

				uint8_t index = 0;

				// send X * 16 = m_page_size bytes en block 
				for (int line = 0; line < (m_page_size / 16); line++)
				{
					// send 4 buffers a 4 bytes (one line) = 16 bytes
					for (uint32_t i = 0; i < 4; i++)
					{
						m_tcon.send_FLASH_BUFFER_FILL(
								m_bcon.src(), m_bcon.dst(), index, 
								data[offset + index],
								data[offset + index + 1],
								data[offset + index + 2],
								data[offset + index + 3]);

						index += 4;
					}

					// afer each complete line, wait for ACK
					m_tcon.recv_FLASH_BUFFER_FILL_ACK(m_bcon.dst(), 
							m_bcon.src());
				}

				const uint8_t lo = (block * m_page_size);
				const uint8_t hi = (block * m_page_size) >> 8;

				m_tcon.send_FLASH_BUFFER_WRITE(m_bcon.src(), m_bcon.dst(), 
						lo, hi);

				// wait for write ACK

				m_tcon.recv_FLASH_BUFFER_WRITE_ACK(m_bcon.dst(), m_bcon.src());

				cout << "*"; 
				cout.flush();
			}

			cout << endl;

			// Nun das Board die CRC16 Checksumme berechnen lassen und
			// vergleichen
			try
			{
				uint16_t board_crc = load_crc16_from_board(size);
				save_crc16_to_board(board_crc,size);
				cout << "written data crc16 is " << board_crc << endl;

				if (board_crc != hexfile.crc16())
				{
					throw traceable_error(
							"Critical error: crc16 NOT identical: update bootloader or maybe flash error?");
				}

			}
			catch (transport_error &e)
			{
				cerr << endl;
				cerr << "warning: no firmware crc16 check support "
					"(update bootloader!)" << endl;
			}

			// Firmware laden
			m_tcon.send_DEVICE_LOAD_APP(m_bcon.src(), m_bcon.dst());


			// Hexfile in den Cache kopieren, damit das inkrementelle Flashen
			// beim naechsen Mal moeglich ist:
			hexfile.copy_original_to(cached_filename());

			cout << endl;
			return;
		}
		catch (transport_error &e)
		{
			// usually, a timeout happened; so the retry loop will get
			// another try...
			cerr << endl;
			cerr << "warning: timeout while waiting for ack, retrying..."
				<< endl;

			cmd_reset();
			sleep(1);

			m_tcon.send_DEVICE_BOOT_STOP(m_bcon.src(), m_bcon.dst());
			m_tcon.recv_DEVICE_BOOT_STOP_ACK(m_bcon.dst(), m_bcon.src());

			assert(cmd_getstate() == 1);

		}
	}
	cerr << "error: flash failed: connection to board lost" << endl;
}

uint16_t board_driver::load_crc16_from_board(uint16_t size) const
{
	uint8_t hi,lo;
	m_tcon.send_FLASH_CRC16_QUERY(m_bcon.src(), m_bcon.dst(),
			size >> 8, size & 0xff);
	m_tcon.recv_FLASH_CRC16_REPLAY(m_bcon.dst(), m_bcon.src(),
			&hi, &lo);

	return (uint16_t)((hi<<8)|lo);
}

void board_driver::save_crc16_to_board(uint16_t crc16, uint16_t size)
{
	// save the checksum and the firmware size to the eeprom
	write_eeprom_word_le(EEPR_FIRMWARE_CRC16, crc16);
	write_eeprom_word_le(EEPR_FIRMWARE_SIZE, (uint16_t)size);
}

uint16_t board_driver::cmd_getaddress ()
{
	// the hcan address is located @eeprom[2]
	return read_eeprom_word_le(2);
}

void board_driver::cmd_setaddress (const uint16_t address)
{
	write_eeprom_word_le(2, address);

	if (read_eeprom_word_le(2) != address)
		cerr << "error: value written to eeprom does not match read";
}


void board_driver::cmd_setsysloglevel(uint16_t level)
{
	m_tcon.send_SYSLOG_LEVEL_SET(m_bcon.src(), m_bcon.dst(), level);
}

void board_driver::cmd_reset()
{
	m_tcon.send_DEVICE_RESET(
			m_bcon.src(), m_bcon.dst());
}

uint32_t board_driver::cmd_getuptime()
{
	uint8_t u0,u1,u2,u3;

	m_tcon.send_UPTIME_QUERY( m_bcon.src(), m_bcon.dst());
	m_tcon.recv_UPTIME_REPLAY( m_bcon.dst(), m_bcon.src(), &u3, &u2, &u1, &u0);

	return (u0) | (u1 << 8) | (u2 << 16) | (u3 << 24);
}

uint8_t board_driver::cmd_getstate()
{
	uint8_t state;

	m_tcon.send_DEVICE_STATE_REQUEST(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_DEVICE_STATE_REPLAY(m_bcon.dst(), m_bcon.src(), &state);

	return state;
}

void board_driver::cmd_dump_ee(uint16_t address)
{
	cout << setbase(16) << setfill('0');
	for (int line = 0; line < 20; line++)
	{
		cout << "@" << setw(4) << address << "    ";

		for (int i = 0; i < 16; i++)
		{
			cout << setw(2) << (uint16_t)read_eeprom_byte(address++) << " ";
			if ((i % 4) == 3)
				cout << " ";
			if ((i % 8) == 7)
				cout << " ";
		}
		cout << endl;
	}
	cout << setbase(10) << setw(1) << setfill('0');
}

void board_driver::cmd_loadapp()
{
	m_tcon.send_DEVICE_LOAD_APP(m_bcon.src(), m_bcon.dst());
}

void board_driver::cmd_bootloader()
{
	cmd_reset();
	sleep(1);
	m_tcon.send_DEVICE_BOOT_STOP(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_DEVICE_BOOT_STOP_ACK(m_bcon.dst(), m_bcon.src());
}

void board_driver::cmd_show_system()
{
	uint8_t arch, type;
	m_tcon.send_DEVICE_TYPE_REQUEST(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_DEVICE_TYPE_REPLAY(m_bcon.dst(), m_bcon.src(), &arch, &type);

	cout << "Board :         ";
	switch (type)
	{
		case 1 : cout << "Versuchsboard1 v02"; break;
		case 2 : cout << "Host-Interface v01, serial"; break;
		case 3 : cout << "Controllerboard-88 v01"; break;
		case 4 : cout << "Controllerboard-1612 v01"; break;
		case 5 : cout << "Userpanel-v01"; break;
		case 6 : cout << "USV Controller"; break;
		case 7 : cout << "Heizungssteuerung"; break;
		case 8 : cout << "Wetterstation"; break;
		default : cout << "unknown";
	}
	cout << endl << "MCU :           ";
	switch (arch)
	{
		case 0 : cout << "AVR Atmega8"; break;
		case 1 : cout << "AVR Atmega32"; break;
		case 2 : cout << "AVR Atmega644"; break;
		default : cout << "unknown";
	}

	uint8_t hi,lo;
	m_tcon.send_BUILD_VERSION_QUERY(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_BUILD_VERSION_REPLAY(m_bcon.dst(), m_bcon.src(), &hi, &lo);
	cout << endl << "Build #:        " << (uint16_t)((hi << 8) | lo);
	cout << endl;

}

void board_driver::cmd_show_time()
{
	// First, print the date:
	//
	uint8_t level, day, month, year;

	m_tcon.send_DATE_REQUEST(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_DATE_INFO(m_bcon.dst(), m_bcon.src(),
			&level, &day, &month, &year);

	cout << "date: " << (uint16_t)day << "." << (uint16_t) month 
			<< "." << (uint16_t)(year + 2000) << ", ";
	
	// Now, print the time:
	//
	uint8_t hour, minute, second;

	m_tcon.send_TIME_REQUEST(m_bcon.src(), m_bcon.dst());
	m_tcon.recv_TIME_INFO(m_bcon.dst(), m_bcon.src(),
			&level, &day, &hour, &minute, &second);

	switch (day)
	{
		case 1 : cout << "Montag"; break;
		case 2 : cout << "Dienstag"; break;
		case 3 : cout << "Mittwoch"; break;
		case 4 : cout << "Donnerstag"; break;
		case 5 : cout << "Freitag"; break;
		case 6 : cout << "Samstag"; break;
		case 7 : cout << "Sonntag"; break;
		default : cout << " ? "; break;
	}

	cout << ", " 
		<< setfill('0') << setw(2) << (uint16_t)hour << ":" 
		<< setfill('0') << setw(2) << (uint16_t)minute << ":" 
		<< setfill('0') << setw(2) << (uint16_t)second << endl;
}


