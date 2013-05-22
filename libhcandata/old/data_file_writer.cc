#include "data_file_writer.h"
#include <libhcan++/traceable_error.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <libhcandata/file_format_v1.h>

using namespace std;
using namespace hcan;

data_file_writer::data_file_writer(const string &filename)
{
	// Datei zum Lesen + Schreiben oeffnen
	
	FILE *m_file = fopen(filename.c_str(), "r+");
	if (m_file == 0)
		throw traceable_error(strerror(errno));

	// 2 Moeglichkeiten:
	//
	// 1. Die Datei ist eben erzeugt worden; dann ist sie 0 bytes
	// gross
	//
	// 2. Die Datei exisitiert bereits; dann ist die size > 0 bytes.
	
	struct stat statbuf;
	result = fstat(m_fd, &statbuf);
	if (result == -1)
		throw traceable_error(strerror(errno));

	if (statbuf.st_size > 0)
	{
		// Datei exisitiert bereits; nun pruefen, ob sie
		// valide ist:

		check_if_file_valid();
	}
	else
	{
		// Datei exisitiert noch nicht - einen Header und
		// einen Datenblock anlegen und zum Schreiben
		// vorbereiten:
		
		setup_file();
	}
}

void data_file_writer::setup_file()
{
	// 1. Header Block schreiben:
	
	data_file_header_block_v1_t header;
	header.type = (data_file_type) hcan::data_file_type_header;
	header.size = sizeof(header);
	header.magic = DATA_FILE_MAGIC_1;
	header.magic2 = DATA_FILE_MAGIC_2;
	fwrite(m_file,&header,sizeof(header));

	// 2. Data Block anlegen:
	
	data_file_data_block_t datablock;
	datablock.type = (data_file_type) hcan::data_file_type_data;
	datablock.size = sizeof(datablock);
	datablock.first_timestamp = time(0);
	datablock.last_timestamp = 0;
	fwrite(m_file,&datablock,sizeof(datablock));
}

void data_file_writer::sync()
{
}

void data_file_writer::check_if_file_valid()
{
}


data_file_writer::~data_file_writer()
{
	fclose(m_file);
}
