#!/bin/sh

echo Starte die HCAN-Dienste
/usr/sbin/hcand &
/usr/sbin/hcanaddressd & 
/usr/sbin/hcansocketd &
/usr/bin/telican --timed &
/usr/bin/hcan4mqttha &
/usr/bin/ring.py &
