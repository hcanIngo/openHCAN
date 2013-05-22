#include "installation_data.h"
#include <assert.h>
#include <libhcan++/traceable_error.h>
#include <boost/lexical_cast.hpp>
#include <libxml++/parsers/domparser.h>
#include <libxml++/nodes/node.h>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;
using namespace xmlpp;

installation_data global_installation_data;

installation_data::installation_data()
{
}

installation_data::~installation_data()
{
}

void installation_data::load(const string &file)
{
	m_parser.parse_file(file);
}

vector<string> installation_data::query_names(const string &xpathq)
{
	Element *rn = m_parser.get_document()->get_root_node();

	NodeSet ns = rn->find(xpathq);

	vector<string> result;

	for (NodeSet::iterator i = ns.begin(); i != ns.end(); i++)
	{
		Element *e = (Element*)(*i);
		assert(e);
		assert(e->get_attribute("name"));
		result.push_back(e->get_attribute("name")->get_value());
	}

	return result;
}

vector<string> installation_data::taster_names()
{
	return query_names("//taster");
}

vector<string> installation_data::lampen_names()
{
	return query_names("//powerport[@typ='lampe']");
}

vector<string> installation_data::sonstige_names()
{
	return query_names("//powerport[@typ='sonstige']");
}

vector<string> installation_data::reedkontakte_names()
{
	return query_names("//reedkontakt");
}

vector<string> installation_data::rollaeden_names()
{
	return query_names("//rolladen");
}

vector<string> installation_data::heizungen_names()
{
	return query_names("//heizung");
}

vector<string> installation_data::tempsensor_names()
{
  return query_names("//tempsensor");
}

uint8_t installation_data::query_gruppe(const string &xpathq)
{
	Element *rn = m_parser.get_document()->get_root_node();

	NodeSet ns = rn->find(xpathq);

	for (NodeSet::iterator i = ns.begin(); i != ns.end(); i++)
	{
		Element *e = (Element*)(*i);
		assert(e);
		assert(e->get_attribute("gruppe"));
		return boost::lexical_cast<uint16_t>(
				e->get_attribute("gruppe")->get_value());
	}

	throw traceable_error("query_gruppe: gruppe not found!");
}

std::string installation_data::query_name(const string &xpathq)
{
	Element *rn = m_parser.get_document()->get_root_node();

	NodeSet ns = rn->find(xpathq);

	for (NodeSet::iterator i = ns.begin(); i != ns.end(); i++)
	{
		Element *e = (Element*)(*i);
		assert(e);
		assert(e->get_attribute("name"));
		return e->get_attribute("name")->get_value();
	}

	throw traceable_error("query_name: gruppe not found!");
}

uint8_t installation_data::lampe_gruppe_by_name(const string &name)
{
	return query_gruppe("//powerport[(@typ='lampe') and (@name='" 
			+ name + "')]");
}

// TODO sonstige_gruppe_by_name muesste es in lampe_command.cc geben
uint8_t installation_data::sonstige_gruppe_by_name(const string &name)
{
	return query_gruppe("//powerport[(@typ='sonstige') and (@name='"
			+ name + "')]");
}

uint8_t installation_data::reedkontakt_gruppe_by_name(const string &name)
{
	return query_gruppe("//reedkontakt[@name='" + name + "']");
}

uint8_t installation_data::rolladen_gruppe_by_name(const string &name)
{
	return query_gruppe("//rolladen[@name='" + name + "']");
}

uint8_t installation_data::heizung_id_by_name(const string &name)
{
	return query_gruppe("//heizung[@name='" + name + "']");
}

string installation_data::heizung_name_by_id(const int id)
{
	return query_name("//heizung[@gruppe='" + boost::lexical_cast<string>(id) + "']");
}

uint8_t installation_data::tempsensor_gruppe_by_name(const string &name)
{
  return query_gruppe("//tempsensor[@name='" + name + "']");
}
