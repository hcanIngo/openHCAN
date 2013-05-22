#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <libhcandata/data_query.h>

namespace hcan
{

	class data_storage
	{
		public:
			virtual ~data_storage();
			//virtual void query(const data_query &q);
	};

}

#endif
