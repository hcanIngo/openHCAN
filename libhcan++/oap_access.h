#ifndef OAP_ACCESS_H
#define OAP_ACCESS_H

#include <libhcan++/board_connection.h>
#include <libxml++/parsers/domparser.h>

namespace hcan
{
	typedef struct
	{
		uint8_t hour;
		uint8_t minute;
	} oap_time;

	class oap_field
	{
		private:
			board_connection *m_bcon;
			mutable uint8_t m_oid;
			mutable uint8_t m_ocid;
			mutable uint8_t m_fid;
			mutable uint8_t m_fcid; // Field Class / Datentyp des Feldes
			mutable uint8_t m_flags;
			mutable uint8_t m_arraysize;
		private:
			mutable uint8_t m_details_loaded;
			void load_details() const; // muss const sein wegen caller-methoden
		public:
			oap_field(board_connection *bcon, uint8_t oid, uint8_t ocid,
					uint8_t fid);
			oap_field();
			~oap_field();
			string name() const;
			uint8_t oid() const;
			uint8_t fid() const;
			uint8_t fcid() const;
			uint8_t arraysize() const;
			uint16_t value(uint8_t idx = 0) const;
			oap_time value_as_time(uint8_t idx = 0) const;
			string value_as_string(uint8_t idx = 0) const;
			string class_as_string() const;
			void set_value(uint16_t value, uint8_t idx = 0);
			void set_value(oap_time &time, uint8_t idx = 0);
			void set_value(const string &value, uint8_t idx = 0);
			const board_connection &bcon() const;
	};

	class oap_object
	{
		private:
			board_connection *m_bcon;
			vector<oap_field> m_fields;
			uint8_t m_oid;
			mutable uint8_t m_cid;
			mutable uint8_t m_param1;
			mutable uint8_t m_param2;
			mutable uint8_t m_param3;
			mutable uint8_t m_param4;
		public:
			oap_object(board_connection *bcon, uint8_t oid);
			~oap_object();
			string class_name() const;
			uint8_t field_count() const;
			const oap_field &field(uint8_t fid) const;
			uint8_t oid() const;
			uint8_t cid() const;
			const oap_field &field_by_name(const string &name) const;
	};

	class oap_access
	{
		private:
			board_connection &m_bcon;
			vector<oap_object> m_objects;
			xmlpp::DomParser m_oap_xml;
		public:
			oap_access(board_connection &bcon);
			virtual ~oap_access();
			uint8_t object_count();
			oap_object &object(uint8_t oid);
	};

}

#endif
