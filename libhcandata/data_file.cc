#include "data_file.h"
#include <libhcan++/traceable_error.h>
#include <libhcandata/file_format_v1.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>


using namespace hcan;

data_file::data_file(const std::string &filename)
{
	int result = open(filename.c_str(),O_RDONLY);
	if (result == -1)
		throw traceable_error(strerror(errno));

	m_fd = result;

	struct stat statbuf;
	result = fstat(m_fd, &statbuf);
	if (result == -1)
		throw traceable_error(strerror(errno));

	m_size = statbuf.st_size;

	m_data = (uint8_t *) mmap(0, m_size, PROT_READ, 
			MAP_PRIVATE, m_fd, 0);
	if (m_data == MAP_FAILED)
		throw traceable_error(strerror(errno));

	// Header Block einlesen und pruefen:
	data_file_header_block_v1_t *header = 
		(data_file_header_block_v1_t *) m_data;

	if (header->magic != DATA_FILE_MAGIC_1)
		throw traceable_error("wrong DATA_FILE_MAGIC_1 value");

	if (header->magic2 != DATA_FILE_MAGIC_2)
		throw traceable_error("wrong DATA_FILE_MAGIC_2 value");

	if (header->type != data_file_type_header)
		throw traceable_error("wrong header block type");

	// Data Block einlesen und pruefen:
	data_file_data_block_t *data = 
		(data_file_data_block_t *) (m_data + sizeof(data_file_header_block_v1_t));

	if (data->type != data_file_type_data)
		throw traceable_error("wrong data block type");
}

data_file::~data_file()
{
	if (m_data)
		munmap(m_data,m_size);
}

int data_file::count() const
{
	return (m_size - sizeof(data_file_header_block_v1_t) -
		        sizeof(data_file_data_block_t)) / 
		sizeof(data_file_frame_entry);
}

data_file_frame_entry data_file::frame(const int idx) const
{
	if ((idx < 0) || (idx > count() -1))
		throw traceable_error("frame index access out of bounds");

	data_file_frame_entry *frames = (data_file_frame_entry *)
		(m_data + sizeof(data_file_header_block_v1_t) +
			sizeof(data_file_data_block_t));

	return frames[idx];
}

void data_file::dump()
{
}



