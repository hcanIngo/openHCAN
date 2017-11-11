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
##  (c) 2017 by Ingo Lages, i (dot) lages (at) gmx (dot) de
##############################################################################

include ./ARCH.inc
.PHONY: tools

alles:
	make clean
	make dep
	make all
	make install
	make tools xx="sudo make all"

clean:
	make strukturen xx="sudo make clean"
	make cDienste xx="sudo make clean"
	make cppDienste xx="sudo make clean"
	make firmware xx="sudo make clean" parm2=MCU=atmega32
	make firmware xx="sudo make clean" parm2=MCU=atmega644
	make firmwareOhneEds xx="sudo make clean"
	make tools xx="sudo make clean"
	#
	sudo find -type f -name ".depend" | xargs rm -f

dep:
	make cppDienste xx="sudo make dep"

all:
	make strukturen xx="make all"
	make cDienste xx="make all"
	make cppDienste xx="make all"
	make firmware xx="make all" parm2=MCU=atmega32
	make firmware xx="sudo make clean" MCU=atmega644; make firmware xx="make all" MCU=atmega644
	make firmwareOhneEds xx="sudo make clean"; make firmwareOhneEds xx="make all"
	
install:
	make strukturen xx="sudo make install"
	make cDienste xx="sudo make install"
	make cppDienste xx="sudo make install"
	##############################################################################
	# Nun kann die Firmware geladen werden (Bootloader flashen, Firmware loaden) #
	##############################################################################

staticAnalyse: 
	make cDienste xx="scan-build -o ./scanBuild make all -j4"

strukturen:
	cd xml; $(xx)

cDienste: 
	cd hcand; $(xx)
	cd hcanaddressd; $(xx)
	cd hcansocketd; $(xx)
	cd hcan4mqttpc; $(xx)

cppDienste:	
	cd libhcan++; test -d .depend || sudo echo "" > .depend; $(xx)
	cd telican; test -d .depend || sudo echo "" > .depend; $(xx)
	cd libhcandata; test -d .depend || sudo echo "" > .depend; $(xx)
	cd check_hcan; test -d .depend || sudo echo "" > .depend; $(xx)
	cd hcanswd; test -d .depend || sudo echo "" > .depend; $(xx)
	cd hcandq; test -d .depend || sudo echo "" > .depend; $(xx)

firmware: 
	cd hcanbl; $(xx) $(parm2)
	cd firmwares/controllerboard-1612-v01; $(xx) $(parm2)
	cd firmwares/userpanel-v01; $(xx) $(parm2)

firmwareOhneEds:
	cd firmwares/usv-modul; $(xx)

tools:
	cd tools; $(xx)
	cd tools/hcanextid; $(xx)

release: 
	cd firmwares/controllerboard-1612-v01; sudo make release MCU=atmega32
	cd firmwares/controllerboard-1612-v01; sudo make release MCU=atmega644
	cd firmwares/userpanel-v01; sudo make release MCU=atmega32
	cd firmwares/userpanel-v01; sudo make release MCU=atmega644
	cd firmwares/usv-modul; sudo make release	
