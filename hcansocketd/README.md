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
can0 at index 15

Wrote 16 bytes

Wrote 16 bytes

received can frame: 2198900840 5

send hcan frame

received can frame: 2148569186 5

send hcan frame

received can frame: 2148569186 5
