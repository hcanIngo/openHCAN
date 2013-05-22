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
#include <utils.h>

using namespace std;
using namespace xmlpp;


vector < frame_message_description_t > frame_message_description;

void parse_message(const Element *proto_e, 
		const Element *service_e, const Element *message_e)
{
	frame_message_description_t msg;
	msg.proto = glib_ustring2uint16(
			proto_e->get_attribute("id")->get_value());


	msg.service = glib_ustring2uint16(
			service_e->get_attribute("id")->get_value());

	msg.service_name = service_e->get_attribute("name")->get_value();

	assert(message_e);
	assert(message_e->get_attribute("id"));
	msg.command = glib_ustring2uint16(
			message_e->get_attribute("id")->get_value());
	msg.command_name = message_e->get_attribute("name")->get_value();

	Node::NodeList nl = message_e->get_children();
	for (Node::NodeList::const_iterator param = nl.begin(); 
		param != nl.end(); param++)
	{
		const Element *param_e = (const Element *)(*param);
		assert(param_e);

		if (param_e->get_name() == "param")
		{
			msg.param_names.push_back(
					param_e->get_attribute("name")->get_value());
		}
	}

	frame_message_description.push_back(msg);
}

void parse_service(const Element *proto_e, const Element *service_e)
{
	Node::NodeList nl = service_e->get_children();
	for (Node::NodeList::const_iterator message = nl.begin(); 
		message != nl.end(); message++)
	{
		const Element *message_e = (const Element *)(*message);
		assert(message_e);

		if (message_e->get_name() == "message")
			parse_message(proto_e, service_e, message_e);
	}
}

void parse_protocol(const Element *proto_e)
{
	string name = proto_e->get_attribute("name")->get_value();
	string descr = proto_e->get_attribute("description")->get_value();
	string id = proto_e->get_attribute("id")->get_value();

	Node::NodeList nl = proto_e->get_children();
	for (Node::NodeList::const_iterator service = nl.begin(); 
		service != nl.end(); service++)
	{
		const Element *service_e = (const Element *)(*service);
		assert(service_e);

		if (service_e->get_name() == "service")
			parse_service(proto_e, service_e);
	}
}

void
init_frame_message_description()
{

	DomParser p;
	p.parse_file("/usr/share/hcan/hcan-protocol.xml");

	if (p)
	{
		Element *rn = p.get_document()->get_root_node();
		Node::NodeList nl = rn->get_children();
		for (Node::NodeList::const_iterator proto = nl.begin(); 
			proto != nl.end(); proto++)
		{
			const Element *e = (const Element *)(*proto);
			assert(e);
			if ( e->get_name() == "protocol")
			{
				parse_protocol(e);
			}
		}

	}
	else
		throw traceable_error(
				"could not parse /usr/share/hcan/hcan-protocol.xml");


}
