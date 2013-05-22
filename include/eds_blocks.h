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
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */

#ifndef EDS_BLOCKS_H
#define EDS_BLOCKS_H

// Definitionen der EDS Block-Typen
#define EDS_BLOCK_EMPTY               0
#define EDS_BLOCK_STRING              1
#define EDS_BLOCK_DIRECTORY           2

/**
 *  * Aufbau eines Directory Eintrags:
 *  - Typ: 2
 *  - Size: 4
 *  - parent address (word)
 *  - name address (word)
 */
#define EDS_BLOCK_DIRECTORY_SIZE      4



#endif
