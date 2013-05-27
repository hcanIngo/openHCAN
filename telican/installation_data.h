#ifndef INSTALLATION_DATA_H
#define INSTALLATION_DATA_H

#include <string>
#include <libxml++/parsers/domparser.h>
#include <stdint.h> //fuer g++-4.4: uint8_t

using namespace std;
using namespace xmlpp;

#define INSTALLATION_XML "/etc/hcan/installation.xml"

class installation_data
{
	private:
		DomParser m_parser;
	private:
		vector<string> query_names(const string &xpathq);
		uint8_t query_gruppe(const string &xpathq);
		std::string query_name(const string &xpathq);
	public:
		installation_data();
		virtual ~installation_data();
		void load(const string &file);
		vector<string> taster_names();
		vector<string> lampen_names();
		vector<string> reedkontakte_names();
		vector<string> rollaeden_names();
		vector<string> heizungen_names();
		vector<string> tempsensor_names();
		vector<string> sonstige_names();
		uint8_t lampe_gruppe_by_name(const string &name);
		uint8_t reedkontakt_gruppe_by_name(const string &name);
		uint8_t rolladen_gruppe_by_name(const string &name);
		uint8_t heizung_id_by_name(const string &name);
		uint8_t tempsensor_gruppe_by_name(const string &name);
		uint8_t sonstige_gruppe_by_name(const string &name);
		std::string heizung_name_by_id(const int id);
};

extern installation_data global_installation_data;

#endif
