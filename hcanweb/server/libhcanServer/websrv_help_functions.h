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
#ifndef WEBSRV_HELP_FUNCTIONS_H
#define WEBSRV_HELP_FUNCTIONS_H

#include <stdint.h> // fuer uint8_t...

uint8_t find_key_val(const char *str, char *strbuf, uint8_t maxlen,const char *key);
void urldecode(char *urlbuf);

#endif
