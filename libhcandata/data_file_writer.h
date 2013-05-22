#ifndef DATA_FILE_WRITER_H
#define DATA_FILE_WRITER_H

#include <string>
#include <libhcandata/file_format_v1.h>

namespace hcan
{

	class data_file_writer
	{
		private:
			int m_fd;
		private:
			void setup_file();
			void check_if_file_valid();
			void sync();
			void write_data_block(int data_pos,
					const data_file_data_block_t *data);
			data_file_frame_entry read_frame();
		public:
			data_file_writer(const std::string &filename);
			virtual ~data_file_writer();
			void write_frame(
					uint16_t src,
					uint16_t dst,
					uint16_t proto,
					uint16_t size,
					uint8_t data[8]);
	};

}

#endif
