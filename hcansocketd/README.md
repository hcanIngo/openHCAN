[hcansocketd: Hostinterface](https://github.com/hcanIngo/openHCAN/wiki/hcanhid)


hcansocketd bildet den Softwareanteil des Hostinterfaces. Geeignet fuer CAN-Bus-Adapter die SocketCAN unterstuetzen. Z.B.:
- "A20"-Bananapi    oder 
- MCP2515-Adapter fuer den Raspberry Pi


Bei Verwendung eines MCP2515-Adapters muss auf dem Pi das SPI-Modul geladen werden, da der MCP2515 via SPI verbunden ist: 

/sbin/modprobe spi_bcm2708

CAN-Interface-Konfiguration:
/sbin/ip link set can0 up type can bitrate 125000




Test:
-----
./hcansocketd -D
initHcan
initSocketcan
can0 at index 4
tx2hcan
tx2cb
recv canfr 2201954376 8
tx2cb
recv canfr 2201954376 8
tx2cb
...

