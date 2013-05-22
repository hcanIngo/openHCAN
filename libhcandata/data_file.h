#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <string>
#include <libhcandata/data_storage.h>

namespace hcan
{

	class data_file : public data_storage
	{
		private:
			int m_fd;
			uint8_t *m_data;
			size_t m_size;
		public:
			data_file(const std::string &filename);
			virtual ~data_file();
			virtual void dump();
			int count() const;
			data_file_frame_entry frame(const int idx) const;
	};

}

#endif
