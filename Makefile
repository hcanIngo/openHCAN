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
	make all
	make install
	make tools xx="sudo make all"

clean:
	make strukturen xx="sudo make clean"
	make cDienste xx="sudo make clean"
	make cppDienste xx="sudo make clean"
	make firmware xx="sudo make clean" parm2=MCU=atmega328p
	make firmware xx="sudo make clean" parm2=MCU=atmega32
	make firmware xx="sudo make clean" parm2=MCU=atmega644p
	make firmwareOhneEds xx="sudo make clean"
	make tools xx="sudo make clean"
	@# folgendes wuerde den "Dropbox-Pfad" erzwingen:  cd hcanhab2_mqtt; make clean
	#
	@#sudo find -type f -name ".depend" | xargs rm -f

all:
	make strukturen xx="make all"
	make cDienste xx="make all"
	make cppDienste xx="make all"
	make firmware xx="make all" parm2=MCU=atmega328p;  make firmware xx="sudo make clean_part" parm2=MCU=atmega328p	
	make firmware xx="make all" parm2=MCU=atmega32;    make firmware xx="sudo make clean_part" parm2=MCU=atmega32
	make firmware xx="make all" parm2=MCU=atmega644p;  make firmware xx="sudo make clean_part" parm2=MCU=atmega644p
	make firmwareOhneEds xx="sudo make clean"; make firmwareOhneEds xx="make all"
	
install:
	make strukturen xx="sudo make install"
	make cDienste xx="sudo make install"
	make cppDienste xx="sudo make install"
	sudo chmod +x ./pi/Systemd/installBpi.sh
	##############################################################################
	# Nun kann die Firmware geladen werden (Bootloader flashen, Firmware loaden) #
	##############################################################################

staticAnalyse: 
	make cDienste xx="scan-build -o ./scanBuild make all -j4"
	make cppDienste xx="scan-build -o ./scanBuild make allSrc -j4"
	@# avr-clang notwendig:   make firmware xx="scan-build -o ./scanBuild make all -j4"
	
staticAnalyseClean:
	sudo find -type f -name "scanBuild" | xargs rm -f

strukturen:
	cd xml; $(xx)

cDienste: 
	cd hcand; $(xx)
	cd hcanaddressd; $(xx)
	cd hcansocketd; $(xx)
	cd hcan4mqttha; $(xx)

cppDienste:	
	cd libhcan++; $(xx)
	cd telican; $(xx)
	cd libhcandata; $(xx)
	cd check_hcan; $(xx)
	cd hcanswd; $(xx)
	cd hcandq; $(xx)

firmware: 
	cd hcanbl; $(xx) $(parm2)
	cd firmwares/controllerboard; $(xx) $(parm2)
	cd firmwares/userpanel-v01; $(xx) $(parm2)
	cd firmwares/ws2812-modul; $(xx) $(parm2)

firmwareOhneEds:
	cd firmwares/usv-modul; $(xx)

tools:
	cd tools; $(xx)
	cd tools/hcanextid; $(xx)
