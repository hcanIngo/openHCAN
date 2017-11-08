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

include ./ARCH.inc
.PHONY: tools

alles:
	make clean
	make dep
	make all
	make install
	make tools xx=all


clean:
	make cDienste xx=clean
	make cppDienste xx=clean
	make firmware xx=clean parm2=MCU=atmega32
	make firmware xx=clean parm2=MCU=atmega644
	make firmwareOhneEds xx=clean
	make strukturen xx=clean
	make tools xx=clean
	#
	sudo find -type f -name ".depend" | xargs rm -f

dep:
	make cppDienste xx=dep

all:
	make strukturen xx=all
	make cDienste xx=all
	make cppDienste xx=all
	make firmware xx=all parm2=MCU=atmega32
	make firmware xx=clean MCU=atmega644; make firmware xx=all MCU=atmega644
	make firmwareOhneEds xx=clean; make firmwareOhneEds xx=all
	
install:
	make strukturen xx=install
	make cDienste xx=install
	make cppDienste xx=install
	##############################################################################
	# Nun kann die Firmware geladen werden (Bootloader flashen, Firmware loaden) #
	##############################################################################


strukturen:
	cd xml; sudo make $(xx)

cDienste: 
	cd hcand; sudo make $(xx)
	cd hcanaddressd; sudo make $(xx)
	cd hcansocketd; sudo make $(xx)
	cd hcan4mqttpc; sudo make $(xx)

cppDienste:	
	cd libhcan++; test -d .depend || sudo echo "" > .depend; sudo make $(xx)
	cd telican; test -d .depend || sudo echo "" > .depend; sudo make $(xx)
	cd libhcandata; test -d .depend || sudo echo "" > .depend; make $(xx)
	cd check_hcan; test -d .depend || sudo echo "" > .depend; sudo make $(xx)
	cd hcanswd; test -d .depend || sudo echo "" > .depend; make $(xx)
	cd hcandq; test -d .depend || sudo echo "" > .depend; make $(xx)

firmware: 
	cd hcanbl; sudo make $(xx) $(parm2)
	cd firmwares/controllerboard-1612-v01; sudo make $(xx) $(parm2)
	cd firmwares/userpanel-v01; sudo make $(xx) $(parm2)

firmwareOhneEds:
	cd firmwares/usv-modul; sudo make $(xx)

tools:
	cd tools; sudo make $(xx)
	cd tools/hcanextid; sudo make $(xx)

release: 
	cd firmwares/controllerboard-1612-v01; sudo make release MCU=atmega32
	cd firmwares/controllerboard-1612-v01; sudo make release MCU=atmega644
	cd firmwares/userpanel-v01; sudo make release MCU=atmega32
	cd firmwares/userpanel-v01; sudo make release MCU=atmega644
	cd firmwares/usv-modul; sudo make release	
