#include "oap_desc.h"
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <libhcan++/traceable_error.h>

using namespace hcan;
using namespace boost;

oap_desc oap_description;

oap_desc::oap_desc()
{
	m_oap_xml.parse_file(OAP_XML);
}

oap_desc::~oap_desc()
{
}

string oap_desc::class_name(uint8_t cid)
{
	NodeSet ns = m_oap_xml.get_document()->get_root_node()->
		find("/classes/class[@id='" + lexical_cast<string>((int)cid) + "']");

	for (NodeSet::iterator i = ns.begin(); i != ns.end(); i++)
	{
		Element *e = (Element*)(*i);
		assert(e);
		assert(e->get_attribute("name"));
		return e->get_attribute("name")->get_value();
	}

	throw traceable_error("oap_desc::class_name: cid " + 
			lexical_cast<string>((int)cid) + " not found");
}

string oap_desc::field_name(uint8_t cid, uint8_t fid)
{
	NodeSet ns = m_oap_xml.get_document()->get_root_node()->
		find("/classes/class[@id='" + lexical_cast<string>((int)cid) 
				+ "']/field[@id='" 
				+ lexical_cast<string>((int)fid) + "']");

	for (NodeSet::iterator i = ns.begin(); i != ns.end(); i++)
	{
		Element *e = (Element*)(*i);
		assert(e);
		assert(e->get_attribute("name"));
		return e->get_attribute("name")->get_value();
	}

	throw traceable_error("oap_desc::field_name: field " + 
			lexical_cast<string>((int)cid) + " not found");
}

