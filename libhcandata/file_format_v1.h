#ifndef FILE_FORMAT_V1_H
#define FILE_FORMAT_V1_H

#include <iostream>
#include <string>
#include <inttypes.h>

/*
 * Aufbau des HCAN Data File, Version 1
 *
 * 1 header block
 * 1 data blocks
 */

namespace hcan
{
#define DATA_FILE_MAGIC_1 0x12345678
#define DATA_FILE_MAGIC_2 0x20080404 // heute ;-)

	enum data_file_type { data_file_type_header = 1, data_file_type_data = 2 };

	/** data_file_block_t kann zum Casten auf unbekannte
	 * Bloecke verwendet werden; alle Blocks muessen dann
	 * korrekte Werte fuer type und size an ihrem Anfang
	 * stehen haben. Damit kann auch ein unbekannter Block
	 * uebersprungen werden.
	 *
	 * Alle Groessenangaben der size-Felder beziehen sich auf
	 * den GESAMTEN Block inklusive dem type-Feld.
	 */

	typedef struct
	{
		uint32_t type;            // data_file_type
		uint32_t size;            // in bytes
	} data_file_block_t;

	typedef struct
	{
		uint32_t type;            // data_file_type
		uint32_t size;            // in bytes
		uint32_t magic;           // = DATA_FILE_MAGIC_1
		uint32_t magic2;          // = DATA_FILE_MAGIC_2
	} data_file_header_block_v1_t;

	typedef struct
	{
		uint32_t type;            // data_file_type 
		uint32_t size;            // in bytes
		uint64_t first_timestamp; // unix epoch 64bit
		uint64_t last_timestamp;  // unix epoch 64bit
	} data_file_data_block_t;

	typedef struct
	{
		uint64_t timestamp;
		uint16_t src;
		uint16_t dst;
		uint16_t proto;
		uint16_t size;
		uint8_t  data[8];
	} data_file_frame_entry; // 22 Bytes

	class file_format_v1
	{
		public:
			static bool check(const std::string &filename);
	};

}

#endif
