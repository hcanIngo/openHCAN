openHAB2 für openHCAN verwenden - [hcanhab](https://github.com/hcanIngo/openHCAN/wiki/hcanhab)
--------------------------------

1. Den Bananapi einrichten (siehe [pi](https://github.com/hcanIngo/openHCAN/tree/master/pi/Systemd)).


2. conf-Ordner (=BeispielKonfiguration) in Verzeichnis xy legen und mindestens folgende Dateien anpassen:
	- Openhab-Installationsdatei "openhab-2.4.0.zip" besorgen (https://www.openhab.org/download/)
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

6.  - auf dem Bananapi openhab manuell starten:
	  -> ssh tt@bananapi
	  -> ./myOpenHabInstallation/start.sh  
	   
	  -> 1. PaperUI: Configuration -> System -> Include Lagacy 1.x Bindings -> save
	     2. PaperUI: Add-ons -> Bindings -> MQTT Binding (1.x) -> install    (binding-mqtt1 - 1.14.0)
    