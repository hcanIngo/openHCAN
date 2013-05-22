#include "data_file_writer.h"
#include <libhcan++/traceable_error.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libhcandata/file_format_v1.h>


using namespace std;
using namespace hcan;

data_file_writer::data_file_writer(const string &filename)
{
	// Datei zum Lesen + Schreiben oeffnen
	
	int result = open(filename.c_str(), O_CREAT | O_RDWR, 0640);
	if (result == -1)
		throw traceable_error(strerror(errno));

	m_fd = result;

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
		cerr << "file exists and is valid" << endl;
	}
	else
	{
		// Datei exisitiert noch nicht - einen Header und
		// einen Datenblock anlegen und zum Schreiben
		// vorbereiten:
		
		setup_file();
	}
}

void data_file_writer::write_frame(
					uint16_t src,
					uint16_t dst,
					uint16_t proto,
					uint16_t size,
					uint8_t data[8])
{
	data_file_frame_entry frame;

	frame.timestamp = time(0);
	frame.src = src;
	frame.dst = dst;
	frame.proto = proto;
	frame.size = size;
	for (int i = 0; i < 8; i++)
		frame.data[i] = data[i];

	write(m_fd,&frame,sizeof(frame));
}


void data_file_writer::setup_file()
{
	// 1. Header Block schreiben:
	
	data_file_header_block_v1_t header;
	header.type = data_file_type_header;
	header.size = sizeof(header);
	header.magic = DATA_FILE_MAGIC_1;
	header.magic2 = DATA_FILE_MAGIC_2;
	write(m_fd,&header,sizeof(header));

	// 2. Data Block anlegen:
	
	data_file_data_block_t datablock;
	datablock.type = data_file_type_data;
	datablock.size = sizeof(datablock);
	datablock.first_timestamp = time(0);
	datablock.last_timestamp = 0;
	write(m_fd,&datablock,sizeof(datablock));
}

void data_file_writer::write_data_block(int data_pos,
		const data_file_data_block_t *data)
{
	// an Anfang des Daten Blocks spulen
	off_t pos = lseek(m_fd,data_pos, SEEK_SET);
	if (pos == 1)
		throw traceable_error(strerror(errno));

	// Daten Block zurueckschreiben
	write(m_fd,data,sizeof(data_file_header_block_v1_t));
}

data_file_frame_entry data_file_writer::read_frame()
{
	data_file_frame_entry frame;
	read(m_fd,&frame,sizeof(frame));
	return frame;
}

void data_file_writer::sync()
{
	// 1. DataBlock Size schreiben:
	
	// ans Ende fahren, Position merken:
	off_t end_pos = lseek(m_fd, 0, SEEK_END);
	if (end_pos == -1)
		throw traceable_error(strerror(errno));

	// Zum Anfang spulen
	off_t pos = lseek(m_fd, 0, SEEK_SET);
	if (pos == -1)
		throw traceable_error(strerror(errno));
	
	// Header Block einlesen
	data_file_header_block_v1_t header;
	read(m_fd,&header,sizeof(header));

	// An den Anfang des Data Blocks spulen
	off_t data_pos = lseek(m_fd, header.size, SEEK_SET);
	if (data_pos == -1)
		throw traceable_error(strerror(errno));

	// erste Bytes des Data Blocks einlesen
	data_file_data_block_t data;
	read(m_fd,&data,sizeof(data));

	// Groesse des Datablocks berechnen
	size_t size = end_pos - data_pos;
	data.size = size;
	cerr << "data block size: " << size << endl;

	// Wenn mindestens 1 Frame im datenblock steht:
	if (size > sizeof(data))
	{
		// 1. Frame lesen - das ist das aelteste Frame:
		data_file_frame_entry oldest_frame = read_frame();
		data.first_timestamp = oldest_frame.timestamp;

		pos = lseek(m_fd, -24, SEEK_END);
		if (pos == -1)
			throw traceable_error(strerror(errno));

		// letztes Frame lesen - das ist das neuste Frame:
		data_file_frame_entry newest_frame = read_frame();
		data.last_timestamp = newest_frame.timestamp;
	}
	else
	{
		data.first_timestamp = 0;
		data.last_timestamp = 0;
	}

	// Datenblock schreiben
	write_data_block(data_pos, &data);

	// Zum Ende spulen
	pos = lseek(m_fd, 0, SEEK_END);
	if (pos == -1)
		throw traceable_error(strerror(errno));
}

void data_file_writer::check_if_file_valid()
{
	// Wir akzeptieren nur Format 1:
	// 1. Header Block
	// 2. Data Block
	//
	
	// an den Anfang spulen
	off_t pos = lseek(m_fd, 0, SEEK_SET);
	if (pos == -1)
		throw traceable_error(strerror(errno));

	// Header Block einlesen und pruefen:
	data_file_header_block_v1_t header;
	read(m_fd,&header,sizeof(header));

	if (header.magic != DATA_FILE_MAGIC_1)
		throw traceable_error("wrong DATA_FILE_MAGIC_1 value");

	if (header.magic2 != DATA_FILE_MAGIC_2)
		throw traceable_error("wrong DATA_FILE_MAGIC_2 value");

	if (header.type != data_file_type_header)
		throw traceable_error("wrong header block type");

	// Zum Datenblock spulen
	pos = lseek(m_fd, header.size, SEEK_SET);
	if (pos == -1)
		throw traceable_error(strerror(errno));

	// Data Block einlesen und pruefen:
	data_file_data_block_t data;
	read(m_fd,&data,sizeof(data));

	if (data.type != data_file_type_data)
		throw traceable_error("wrong data block type");

	// Zum Ende spulen
	pos = lseek(m_fd, 0, SEEK_END);
	if (pos == -1)
		throw traceable_error(strerror(errno));
}


data_file_writer::~data_file_writer()
{
	sync();
	close(m_fd);
}
