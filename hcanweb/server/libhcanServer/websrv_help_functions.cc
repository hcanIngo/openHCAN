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
 *  and based on tuxgraphics (http://tuxgraphics.org/electronics/):
 *  (c) 2013 by Guido Socher
 */
#include <websrv_help_functions.h>

#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

#include <string.h> // fuer strcpy

using namespace cgicc;

// eq. Fkt.-Signatur zum C1612-Webserver:
// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in strbuf. You must allocate
// enough storage for strbuf, maxlen is the size of strbuf.
// I.e the value it is declared with: strbuf[5]-> maxlen=5
uint8_t find_key_val(const char *str, char *strbuf, uint8_t maxlen,const char *key)
{
	Cgicc cgi;
	const_form_iterator iter;
	uint8_t found=0;

	iter = cgi.getElement(key); // z.B.: key="cmd"
	if (iter != (*cgi).end())
	{
		strcpy(strbuf, (*iter).getValue().c_str());
		found=1;
	}

	return found;
}

// convert a single hex digit character to its integer value
unsigned char h2int(char c)
{
        if (c >= '0' && c <='9'){
                return((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
                return((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
                return((unsigned char)c - 'A' + 10);
        }
        return(0);
}

// decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe"
void urldecode(char *urlbuf)
{
        char c;
        char *dst;
        dst=urlbuf;
        while ((c = *urlbuf)) {
                if (c == '+') c = ' ';
                if (c == '%') {
                        urlbuf++;
                        c = *urlbuf;
                        urlbuf++;
                        c = (h2int(c) << 4) | h2int(*urlbuf);
                }
                *dst = c;
                dst++;
                urlbuf++;
        }
        *dst = '\0';
}
