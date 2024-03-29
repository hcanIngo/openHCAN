##############################################################################
##  This file is part of the HCAN tools suite.
##
##  HCAN is free software; you can redistribute it and/or modify it under
##  the terms of the GNU General Public License as published by the Free
##  Software Foundation; either version 2 of the License, or (at your
##  option) any later version.
##
##  HCAN is distributed in the hope that it will be useful, but WITHOUT
##  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
##  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
##  for more details.
##
##  You should have received a copy of the GNU General Public License along
##  with HCAN; if not, write to the Free Software Foundation, Inc., 51
##  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
##
##  (c) 2017 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
##############################################################################

# Choose firmware architecture atmega328p, atmega32 or atmega644p (for C1612 and UI):
# see attribute "__AVR_ATmega644P__", ...
#######################
export MCU = atmega644p
#######################

# Choose CAN-Server-IP for load firmware (eg. 192.168.1.42)
###########################
export IP_HI = 192.168.1.42
###########################

# z.B. -DhcanXYZ oder -DHEIZUNG_INCL_PID_REGLER_CODE
export GLOBAL_DEFINES =
###########################

# Max Hex Size in byte when using the bootloader
###########################
MAXSIZE_ATMEGA328P = 28672
MAXSIZE_ATMEGA32   = 28672
MAXSIZE_ATMEGA644P = 61440
###########################
