#!/bin/sh

echo Starte die HCAN-Dienste
/usr/sbin/hcand &
/usr/sbin/hcanaddressd & 
/usr/sbin/hcansocketd &
/usr/bin/telican --timed &
/usr/sbin/hcan4mqttpc &

