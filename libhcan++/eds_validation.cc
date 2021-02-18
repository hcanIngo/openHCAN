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
#include <eds_validation.h>
#include <utils.h>


using namespace std;
using namespace xmlpp;

void eds_check_block_descriptions()
{
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
				const string block_name = block_e->get_attribute("name")->get_value();
				const uint16_t size = glib_ustring2uint16(
					block_e->get_attribute("size")->get_value());

				int8_t lastpos = -1;
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
						const string field_name = 
							field_e->get_attribute("name")->get_value();
						const uint16_t pos = glib_ustring2uint16(
								field_e->get_attribute("pos")->get_value());

						uint16_t size = 0;
						if (datatype == "char")
						{
							size = glib_ustring2uint16(field_e->
									get_attribute("size")->get_value());
						}

						if (lastpos == -1 && pos != 0) throw traceable_error("/usr/share/hcan/eds.xml: Attribut pos startet nicht bei 0. (Block: " +  block_name + " Field: " + field_name + ")");
						if (lastpos != -1 && lastpos != pos-1)
						{
							throw traceable_error("/usr/share/hcan/eds.xml: Attribut pos nicht aufsteigend. Bzw. Groeße von vorherigem Field stimmt nicht. (Block: " +  block_name + " Field: " + field_name + ")");
						}
						else
						{
							if (datatype == "uint16_t")
							{
								lastpos = pos+1;
							}
							else if (datatype == "char")
							{
								lastpos = pos+size-1;
							}
							else
							{
								lastpos = pos;
							}
						}

						Node::NodeList nlbits = field_e->get_children();
						if(nlbits.size() != 0)
						{
							int8_t lastposbits = -1;
							for (Node::NodeList::const_iterator bit = nlbits.begin(); bit != nlbits.end(); bit++)
							{
								const Element *field_e2 = (const Element *)(*bit);
								assert(field_e2);
								if ( field_e2->get_name() == "bit")
								{
									const uint8_t no = glib_ustring2uint16(field_e2->get_attribute("no")->get_value());
									const string description = field_e2->get_attribute("description")->get_value();
									if (lastposbits == -1 && no != 0) throw traceable_error("/usr/share/hcan/eds.xml: Attribut no startet nicht bei 0. (Block: " +  block_name + " Field: " + field_name + " Bit: " + (std::to_string(no)) + ")");;
									if (lastposbits != -1 && lastposbits != no-1)
									{
										throw traceable_error("/usr/share/hcan/eds.xml: Attribut no nicht aufsteigend. (Block: " +  block_name + " Field: " + field_name + " Bit: " + (std::to_string(no)) + ")");
									}
									else
									{
										lastposbits = no;
									}
								}
							}
						}
					}
				}
				if (lastpos != size-1) throw traceable_error("/usr/share/hcan/eds.xml: Block Groeße und Felder Anzahl bzw. Groeßen stimmen nicht ueberein. (Block: " +  block_name + ")");
			}
		}
	}
	else
	{
		throw traceable_error(
			"could not parse /usr/share/hcan/eds.xml");
	}
}
