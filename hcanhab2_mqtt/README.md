openHAB2 für openHCAN verwenden - [hcanhab](https://github.com/hcanIngo/openHCAN/wiki/hcanhab)
--------------------------------

1. Den Bananapi einrichten (siehe [pi](https://github.com/hcanIngo/openHCAN/tree/master/pi/Systemd)).


2. conf-Ordner in Verzeichnis xy legen und mindestens folgende Dateien anpassen:
	- Openhab-Installationsdateien besorgen (http://www.openhab.org/downloads.html)
	- [myconf.inc](https://github.com/hcanIngo/openHCAN/tree/master/hcanhab2_mqtt/BeispielKonfiguration/myconf.inc)
	- [installation.xml nach vorgegebenen Schema anpassen](https://github.com/hcanIngo/openHCAN/tree/master/hcanhab2_mqtt/BeispielKonfiguration/installation.xml)


3. Im MakefileDefines.inc folgende Einstellungen vornehmen: 
	A) CONF = xy/conf  einstellen (wo die eingene "openHCAN/hcanhab2_mqtt/BeispielKonfiguration/" liegt). 
	B) ZIEL_PC = localhost   oder   ZIEL_PC = bananian  waehlen
	C) Fuer localhost den ZIEL_USER anpassen


4. Makefile
	- make all


4. Openhab starten (per Makefile ausführen):
	- make start     (mit log-Datei: make start > ~/tmp/openhabMeldungen.txt)
	- make run_webclient / oder z.B. Habdroid-App verwenden
