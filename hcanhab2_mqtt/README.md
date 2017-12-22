openHAB2 für openHCAN verwenden - [hcanhab](https://github.com/hcanIngo/openHCAN/wiki/hcanhab)
--------------------------------

1. Den Bananapi einrichten (siehe [pi](https://github.com/hcanIngo/openHCAN/tree/master/pi/Systemd)).


2. conf-Ordner (=BeispielKonfiguration) in Verzeichnis xy legen und mindestens folgende Dateien anpassen:
	- Openhab-Installationsdateien besorgen (http://www.openhab.org/downloads.html)
	- [myconf.inc](https://github.com/hcanIngo/openHCAN/tree/master/hcanhab2_mqtt/BeispielKonfiguration/myconf.inc)
	- [installation.xml nach vorgegebenen Schema anpassen](https://github.com/hcanIngo/openHCAN/tree/master/hcanhab2_mqtt/BeispielKonfiguration/installation.xml)


3. Im MakefileDefines.inc folgende Einstellungen vornehmen: 
	- CONF = xy/conf  einstellen (wo die eigene "openHCAN/hcanhab2_mqtt/BeispielKonfiguration/" liegt). 
	- ZIEL_PC = localhost   oder   ZIEL_PC = bananapi  waehlen
	- Fuer localhost: ZIEL_USER anpassen


4. Makefile
	- make all  @  openHCAN/hcanhab2_mqtt/


5. Openhab starten (per Makefile ausführen):
	- Fuer localhost: make start     (mit log-Datei: make start > ~/tmp/openhabMeldungen.txt)
	- make run_webclient   oder z.B. Habdroid-App verwenden
