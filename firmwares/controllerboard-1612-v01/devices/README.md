[Device](https://github.com/hcanIngo/openHCAN/wiki/device)

Hinweis: Devices im Verzeichnis "devicePool" werden nicht eingebunden.


Schritte zu einem neuen Device: 
-------------------------------
1. In der openHCAN/xml/eds.xml unter z.B. block type="10" eine Nummer fuer das Device vergeben, 
   die im System noch nicht verwendet wird. Und einen eindeutigen Namen, sowie die benoetigten Daten.
   
2. Ggf. in openHCAN/xml/hcan-protocol.xml neue Messages definieren.

3. In openHCAN/firmwares/controllerboard-1612-v01/devices folgende Dateien anlegen: 
   - NameDesDevice.h
   - NameDesDevice.c

4. In openHCAN/firmwares/controllerboard-1612-v01/
   make clean; make all
   ausfuehren, um die unter openHCAN/firmwares/controllerboard-1612-v01/devices
   liegenden Devices einzubinden und alles zu uebersetzen und linken
