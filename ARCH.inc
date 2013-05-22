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
##  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
##############################################################################

# Choose services architecture i386 or arm (arm isn't complete supported): 
##################
export ARCH = i386
##################

# Choose firmware architecture atmega32 or atmega644 (for C1612 and UI): 
######################
export MCU = atmega644
######################

# Choose HI-Interface-Server-IP for load firmware (eg. 192.168.1.24)
###########################
export IP_HI = 192.168.1.24
###########################