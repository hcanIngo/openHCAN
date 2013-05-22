#include "oap_access.h"
#include <oap-def.h>
#include <assert.h>
#include <libhcan++/transport_connection.h>
#include <libhcan++/board_connection.h>
#include <libhcan++/oap_desc.h>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using namespace hcan;
using namespace boost;
using namespace std;

oap_field::oap_field() :
	m_bcon(0), m_oid(0),m_fid(0)
{
}

oap_field::oap_field(board_connection *bcon, uint8_t oid, uint8_t ocid,
		uint8_t fid) :
	m_bcon(bcon), m_oid(oid),m_ocid(ocid), m_fid(fid)
{
	m_details_loaded = false; // Lazy fetching
	m_fcid = 255;
	m_flags = 255;
	m_arraysize = 0; // Default, d.h. im Normalfall ist es kein array
}

oap_field::~oap_field()
{
}

string oap_field::name() const
{
	load_details();
	return oap_description.field_name(m_ocid, m_fid);
}

void oap_field::load_details() const
{
	if (! m_details_loaded)
	{
		assert(m_bcon);

		m_bcon->tcon().send_FIELD_ATTRIBUTE_REQUEST(
				m_bcon->src(), m_bcon->dst(), m_oid, m_fid);

		uint8_t dummy;
		m_bcon->tcon().recv_FIELD_ATTRIBUTE_REPLAY(
				m_bcon->dst(), m_bcon->src(), &dummy, &dummy, 
				&m_fcid, &m_flags, &m_arraysize);

		m_details_loaded = true;
	}
}

uint8_t oap_field::oid() const
{
	return m_oid;
}

uint8_t oap_field::fid() const
{
	return m_fid;
}

uint8_t oap_field::fcid() const
{
	load_details();
	return m_fcid;
}

uint8_t oap_field::arraysize() const
{
	load_details();
	return m_arraysize;
}

uint16_t oap_field::value(uint8_t idx) const
{
	assert(m_bcon);

	m_bcon->tcon().send_FIELD_READ_REQUEST(
			m_bcon->src(), m_bcon->dst(), m_oid, m_fid, idx);

	uint8_t dummy, hi, lo;
	m_bcon->tcon().recv_FIELD_READ_REPLAY(
			m_bcon->dst(), m_bcon->src(), &dummy, &dummy, 
			&dummy, &hi, &lo);

	return (hi << 8) | lo;
}

oap_time oap_field::value_as_time(uint8_t idx) const
{
	oap_time t;
	uint16_t v = value(idx);

	t.hour = v >> 8;
	t.minute = v & 0xff;

	return t;
}

string oap_field::value_as_string(uint8_t idx) const
{
	switch (m_fcid)
	{
		case OAP_TYPE_BOOL :
			return value(idx) > 0 ? "true" : "false";
		case OAP_TYPE_INT :
			return lexical_cast<string>((int)value(idx));
		case OAP_TYPE_TIME :
			{
				oap_time t = value_as_time(idx);
				stringstream ss;
				ss <<  setw(2) << setfill('0') << (int)t.hour << ":" << setw(2)
					<< setfill('0') << (int)t.minute;
				return ss.str();
			}
	}

	throw traceable_error("oap_field::value_as_string: unknown field class");
}

string oap_field::class_as_string() const
{
	switch (m_fcid)
	{
		case OAP_TYPE_BOOL :
			return "bool";
		case OAP_TYPE_INT :
			return "int";
		case OAP_TYPE_TIME :
			return "time";
	}

	throw traceable_error("oap_field::class_as_string: unknown field class");
}

void oap_field::set_value(uint16_t value, uint8_t idx)
{
	assert(m_bcon);

	uint8_t dummy, hi, lo;

	hi = value >> 8;
	lo = value & 0xff;

	m_bcon->tcon().send_FIELD_WRITE_REQUEST(
			m_bcon->src(), m_bcon->dst(), m_oid, m_fid, idx, hi, lo);

	m_bcon->tcon().recv_FIELD_WRITE_REPLAY(
			m_bcon->dst(), m_bcon->src(), &dummy, &dummy, 
			&dummy);
}

void oap_field::set_value(oap_time &time, uint8_t idx)
{
	uint16_t v;
	v = (time.hour << 8) | (time.minute & 0xff);
	set_value(v);
}

void oap_field::set_value(const string &value, uint8_t idx)
{
	// Unterscheiden: bool, int oder time?
	
	boost::regex re_bool("true|false");
	boost::regex re_time("(\\d+):(\\d+)");
	boost::regex re_int("(\\d+)");
	boost::cmatch matches;

	if (boost::regex_match(value.c_str(), matches, re_bool))
	{
		if (matches[0] == "true")
			set_value(1,idx);
		else
			set_value(0,idx);

		return;
	}

	if (boost::regex_match(value.c_str(), matches, re_time))
	{
		oap_time t;
		t.hour = lexical_cast<uint16_t>(matches[1]);
		t.minute = lexical_cast<uint16_t>(matches[2]);
		set_value(t,idx);
		return;
	}

	if (boost::regex_match(value.c_str(), matches, re_int))
	{
		set_value(lexical_cast<uint16_t>(matches[0]), idx);
		return;
	}

	throw traceable_error("oap_field::set_value: invalid value format:'" +
			value +"'");

}

const board_connection &oap_field::bcon() const
{
	assert(m_bcon);
	return *m_bcon;
}

oap_object::oap_object(board_connection *bcon, uint8_t oid) :
	m_bcon(bcon),
	m_oid(oid)
{
	assert(m_bcon);

	m_bcon->tcon().send_OBJECT_ATTRIBUTE_REQUEST(
			m_bcon->src(), m_bcon->dst(), m_oid);

	uint8_t dummy;
	m_bcon->tcon().recv_OBJECT_ATTRIBUTE_REPLAY(
			m_bcon->dst(), m_bcon->src(), &dummy, &m_cid, 
			&m_param1,
			&m_param2,
			&m_param3,
			&m_param4);

	m_bcon->tcon().send_FIELD_COUNT_REQUEST(
			m_bcon->src(), m_bcon->dst(), m_oid);

	uint8_t count;
	m_bcon->tcon().recv_FIELD_COUNT_REPLAY(
			m_bcon->dst(), m_bcon->src(), &dummy, &count);

	for (uint8_t i = 0; i < count; i++)
	{
		m_fields.push_back(oap_field(m_bcon, m_oid, m_cid, i));
	}
}

oap_object::~oap_object()
{
}

string oap_object::class_name() const
{
	return oap_description.class_name(m_cid);
}

uint8_t oap_object::field_count() const
{
	return m_fields.size();
}

const oap_field &oap_object::field(uint8_t fid) const
{
	if (fid > m_fields.size() -1)
		throw traceable_error("oap_object::field: index > size -1");

	return m_fields[fid];
}

uint8_t oap_object::oid() const
{
	return m_oid;
}

uint8_t oap_object::cid() const
{
	return m_cid;
}

const oap_field &oap_object::field_by_name(const string &name) const
{
	for (size_t i = 0; i < m_fields.size(); i++)
	{
		if (m_fields[i].name() == name)
			return m_fields[i];
	}

	throw traceable_error("oap_object::field_by_name: field by name '" +
			name + "' not found");
}


oap_access::oap_access(board_connection &bcon) :
	m_bcon(bcon)
{
	// Falls das Board kein OAP unterstuetzt, so ist
	// m_objects.size() immer 0; damit sollte sich
	// das oap_access Objekt so verhalten, als ob keine OAP
	// Objekte auf dem Board vorhanden sind.
	try
	{
		m_bcon.tcon().send_OBJECT_COUNT_REQUEST(
				m_bcon.src(), m_bcon.dst());

		uint8_t count;
		m_bcon.tcon().recv_OBJECT_COUNT_REPLAY(
				m_bcon.dst(), m_bcon.src(), &count);
		for (uint8_t i = 0; i < count; i++)
		{
			m_objects.push_back(oap_object(&m_bcon,i));
		}
	}
	catch (transport_error &e)
	{
	}
}

oap_access::~oap_access()
{
}

uint8_t oap_access::object_count()
{
	return m_objects.size();
}

oap_object &oap_access::object(uint8_t oid)
{
	if (oid > m_objects.size() -1)
		throw traceable_error("oap_access::object: index > size -1");

	return m_objects[oid];
}

