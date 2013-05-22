#ifndef DATA_FILE_WRITER_H
#define DATA_FILE_WRITER_H

#include <string>

namespace hcan
{

	class data_file_writer
	{
		private:
			FILE *m_file;
		private:
			void setup_file();
			void check_if_file_valid();
			void sync();
		public:
			data_file_writer(const std::string &filename);
			virtual ~data_file_writer();
	};

}

#endif
