#!/bin/sh

echo Starte die HCAN-Dienste
/usr/bin/socat -d -d pty,link=/tmp/ttyV0,b38400,rawer,echo=0 TCP:192.168.1.92:502 &
/usr/sbin/hcand &
/usr/sbin/hcanaddressd &
/usr/sbin/hcansocketd &
/usr/bin/telican --timed &
/usr/bin/hcan4mqttha &
/usr/bin/ring.py &
/home/tt/evcc/evcc_armLinux -c /home/tt/evcc/evcc.yaml & 
