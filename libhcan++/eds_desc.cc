#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <traceable_error.h>
#include <frame_message_description.h>
#include <libxml++/parsers/domparser.h>
#include <libxml++/nodes/node.h>
#include <stdexcept>
#include <assert.h>
#include <inttypes.h>
#include <eds_desc.h>
#include <utils.h>


using namespace std;
using namespace xmlpp;

vector < eds_block_desc_t > eds_get_block_descriptions()
{
	vector < eds_block_desc_t > blocks;

	DomParser p;
	p.parse_file("/usr/share/hcan/eds.xml");

	if (p)
	{
		Element *rn = p.get_document()->get_root_node();

		Node::NodeList nl = rn->get_children();
		for (Node::NodeList::const_iterator block = nl.begin();
				block != nl.end(); block++)
		{
			const Element *block_e = (const Element *)(*block);
			assert(block_e);
			if ( block_e->get_name() == "block")
			{


				const uint16_t type = glib_ustring2uint16(
					block_e->get_attribute("type")->get_value());
				const string name = block_e->get_attribute("name")->get_value();
				const uint16_t size = glib_ustring2uint16(
					block_e->get_attribute("size")->get_value());

				eds_block_desc_t block(type,name,size);

				Node::NodeList nl = block_e->get_children();
				for (Node::NodeList::const_iterator field = nl.begin();
						field != nl.end(); field++)
				{
					const Element *field_e= (const Element *)(*field);
					assert(field_e);
					if ( field_e->get_name() == "field")
					{
						const string datatype = 
							field_e->get_attribute("datatype")->get_value();
						const string name = 
							field_e->get_attribute("name")->get_value();
						const uint16_t pos = glib_ustring2uint16(
								field_e->get_attribute("pos")->get_value());

						uint16_t size = 0;
						if (datatype == "char")
						{
							size = glib_ustring2uint16(field_e->
									get_attribute("size")->get_value());
						}

						block.fields.push_back(eds_block_field_desc_t(
									datatype, name, pos,size));

					}
				}

				blocks.push_back(block);
			}
		}
	}
	else
		throw traceable_error(
			"could not parse /usr/share/hcan/eds-local.xml");

    return blocks;
}
