#ifndef DATA_QUERY_H
#define DATA_QUERY_H

#include <inttypes.h>
#include <string>
#include <libhcandata/file_format_v1.h>

namespace hcan
{

#define CODE_SIZE 128
#define STACK_SIZE 16

	class data_query
	{
		private:
			uint32_t m_code[CODE_SIZE];
			uint32_t m_stack[STACK_SIZE];
			int m_stack_ptr;
		private:
			void compile_query(const std::string &query);
			void push(uint32_t data);
			uint32_t pop();
		public:
			data_query(const std::string &query);
			virtual ~data_query();
			bool match(const data_file_frame_entry &f);

	};
}

#endif
