#include "utils.h"
#include <sstream>

uint16_t glib_ustring2uint16(const Glib::ustring &s)
{
	std::stringstream ss;
	uint16_t result;

	ss << s.raw();
	ss >> result;

	return result;
}

