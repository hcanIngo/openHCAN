/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2013 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 *  based on hcan:
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */
#include <iostream>
#include <fstream>
#include <string>

#include <string.h> // fuer strcpy
#include <stdlib.h> // fuer EXIT_SUCCESS

#include <../shared_hms_interface.h>

using namespace std;

int main (int argc, char *argv[])
{
	try
	{
		t_query query;
		char * str = '\0'; // dummy, fuer eine einheitlich find_key_val()-Signatur
		getAjaxInput (str, &query);
		processCmd (&query);
		processQuery (&query);

		cout << "Content-Type: application/javascript" << endl << endl;
		cout << "myjp({" << query.JSONbuf << "})"; // mit JSONbuf = "id":state, "id":state, ...

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e) // irgendein Fehler?
	{
		//cout << "Content-Type: text/html" << endl << endl;
		cout << "Content-Type: application/javascript" << endl << endl;
		string strJSON;
		strJSON += "\"";
		strJSON += "255"; // id = 255 damit es in der Selektionsbox angezeigt wird
		strJSON += "\":\"";
		strJSON += "exception - "; // state
		strJSON += e.what(); // state
		strJSON += "\"";
		cout << "myjp({" << strJSON << "})"; // mit strJSON = "id":state, "id":state, ...
		return EXIT_FAILURE;
	}
}
