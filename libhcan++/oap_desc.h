#ifndef OAP_DESC_H
#define OAP_DESC_H

#include <libxml++/parsers/domparser.h>
#include <libxml++/nodes/node.h>
#include <string>
#include <stdint.h> //fuer g++-4.4: uint8_t

using namespace xmlpp;
using namespace std;

#define OAP_XML "/usr/share/hcan/oap.xml"

namespace hcan
{
	class oap_desc
	{
		private:
			DomParser m_oap_xml;
		public:
			oap_desc();
			~oap_desc();

			string class_name(uint8_t cid);
			string field_name(uint8_t cid, uint8_t fid);

	};

}

extern hcan::oap_desc oap_description;

#endif
