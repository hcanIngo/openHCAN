[![logo_open](https://cloud.githubusercontent.com/assets/4243148/11915822/1885c376-a6b8-11e5-8410-8ad30d1e1734.png)](https://github.com/hcanIngo/openHCAN/wiki)

openHCAN ist eine Hausautomatisierungsloesung auf CAN-Bus Basis. Zielgruppe ist der versierte Anwender oder Entwickler.

Die Hausautomatisierung besteht aus einer Reihe von Modulen, die ueber den CAN Bus miteinander kommunizieren. Die eigentliche Arbeit erledigen die Controller-Module: hier sind Relais zum Schalten von 230V-Lasten und Sensoren wie Taster, Reedkontakte, Temperatur-Sensoren etc angeschlossen.

Bedient wird die Hausautomatisierung ueber eine Weboberflaeche, die Bedienfelder, kleine Unterputz- Module mit einen LCD-Display und ein paar Tastern. Hier kann man z.B. die Raumtemperatur einstellen, Stati abfragen etc. Triviale Dinge wie Licht einschalten o.ae. kann natuerlich weiterhin per Taster gemacht werden.

Zur Wartung und komfortableren Bedienung ist der CAN-Bus ueber das oben bereits erwaehnte Hostinterface an einen Host-Rechner angebunden. Mit telican existiert ein Werkzeug zur Wartung und Konfiguration.

Fuer den eigentlichen Busbetrieb ist aber kein PC noetig - die Module sind voellig autark funktionsfaehig. Sollte also der Rechner ausfallen, so funktionieren Licht, Rollaeden, Heizung etc weiterhin.

openHCAN wurde aus der Motivation heraus entwickelt, eine freie (im Gegensatz zu einer proprietaeren) Hausautomatisierung zu realisieren. Dem OpenSource Gedanken folgendend ist openHCAN unter der GNU Public License veroeffentlicht.

Den Code direkt aufs eigene System:  git clone https://github.com/hcanIngo/openHCAN.git 

[wiki -> Entwicklungsumgebung einrichten](https://github.com/hcanIngo/openHCAN/wiki/entwicklungsumgebung_installieren)

[wiki -> Systemueberblick](https://github.com/hcanIngo/openHCAN/wiki/systemueberblick)


[![logo](https://cloud.githubusercontent.com/assets/4243148/11906546/aa26f62e-a5cd-11e5-9f51-9ae772fecefe.jpg)](http://hcaningo.github.io/openHCAN/)

[Mailingliste](https://groups.google.com/forum/?hl=de#!forum/openhcan) | 
[Forum](http://openhcan.org) | 
[Wiki](https://github.com/hcanIngo/openHCAN/wiki) | 
[Quellen](https://github.com/hcanIngo/openHCAN) | 
[Webseite](http://hcaningo.github.io/openHCAN/)
