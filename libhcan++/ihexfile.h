#ifndef IHEXFILE_H
#define IHEXFILE_H

#include <string>
#include <vector>
#include <inttypes.h>
#include <libhcan++/traceable_error.h>

using namespace std;

class ihexfile_error : public traceable_error
{
	public:
		ihexfile_error(const string &s);
		virtual ~ihexfile_error() throw () {};
};

class ihexfile
{
	private:
		string m_filename;
		vector<uint8_t> m_data;
		uint16_t _crc16_update(uint16_t crc, uint8_t a) const;
	public:
		ihexfile();
		/** 
		 * page_size: fuellt den Rest mit 0xff auf, so dass
		 * die Block-Groesse ein Vielfaches von page_size ergibt.
		 *
		 * Das ist noetig, um in ganzen Bloecken zu flashen
		 */
		void load(const string &filename, uint16_t page_size);
		/**
		 * kopiert das (geladene) Hexfile. Wird beim Anlegen der
		 * cached Hexfiles verwendet...
		 */
		void copy_original_to(const string &filename) const;
		virtual ~ihexfile();
		vector<uint8_t> data() const;
		const uint16_t size() const;
		const uint16_t crc16() const;
};

#endif
