
#ifndef HCAN_DEF_H
#define HCAN_DEF_H

// ------- AUTOMATICLY GENERATED FROM mk_hcan-def_h.xsl -----------------


// Service: HMS

// Zum Testen, ob ein Geraet auf diese Adresse horcht
#define HCAN_HMS_PING_REQUEST   1

// Anwort des Geraetes
#define HCAN_HMS_PING_REPLAY   2

// Fragt ein Geraet nach seinem aktuellen Zustand
#define HCAN_HMS_DEVICE_STATE_REQUEST   3

// Liefert den aktuellen Zustand zurueck
// state : 0 = booting, 1 = Bootloader, 2 = Application
#define HCAN_HMS_DEVICE_STATE_REPLAY   4

// Loest einen Watchdog Reset aus
#define HCAN_HMS_DEVICE_RESET   5

// stoppt den Bootvorgang; Geraet geht in den Bootloader-State
#define HCAN_HMS_DEVICE_BOOT_STOP   6

// Wenn Bootvorgang gestoppt, sendet das Geraet diese Bestaetigung
#define HCAN_HMS_DEVICE_BOOT_STOP_ACK   7

// Laed die Applikation, sofern das Geraet im Bootloader State ist
#define HCAN_HMS_DEVICE_LOAD_APP   8

// Fragt den Geraetetyp ab
#define HCAN_HMS_DEVICE_TYPE_REQUEST   9

// Liefert den Geraete-Typ
// arch : 0 = Atmega8, 1 = Atmega32, 2 = Atmega644p
// type : Hardware-Board-Typ
#define HCAN_HMS_DEVICE_TYPE_REPLAY   10

// Sendet 4 Bytes zum Geraet. Details stehen in der Dokumentation zum
// Flashen
// index : definiert die Position von d[0] im Zielbuffer auf dem Device
// d0 : Databyte
// d1 : Databyte
// d2 : Databyte
// d3 : Databyte
#define HCAN_HMS_FLASH_BUFFER_FILL   16

// Geraet bestaetigt damit den Erhalt einer Zeile
#define HCAN_HMS_FLASH_BUFFER_FILL_ACK   17

// Geraet schreibt den Puffer an die gegebene Adresse in das Flash
// addr_lsb : LSB der Adresse im Flash
// addr_msb : MSB der Adresse im Flash
#define HCAN_HMS_FLASH_BUFFER_WRITE   18

// Geraet bestaetigt das Schreiben
#define HCAN_HMS_FLASH_BUFFER_WRITE_ACK   19

// Beschreibt eine Speicherzelle des internen EEPROMs (ohne Pruefung!)
// addr_lsb : LSB der Adresse im EEPROM
// addr_msb : MSB der Adresse im EEPROM
// value : der zu schreibende Wert
#define HCAN_HMS_INTERNAL_EEPROM_WRITE   20

// Geraet bestaetigt das Schreiben (ohne Pruefung!)
#define HCAN_HMS_INTERNAL_EEPROM_WRITE_ACK   21

// Leseanforderung fuer eine Speicherzelle des internen EEPROMs
// addr_lsb : LSB der Adresse im EEPROM
// addr_msb : MSB der Adresse im EEPROM
#define HCAN_HMS_INTERNAL_EEPROM_READ_REQUEST   22

// Geraet sendet den gelesenen Wert
// value : der Wert aus dem EEPROM
#define HCAN_HMS_INTERNAL_EEPROM_READ_REPLAY   23

// Leseanforderung fuer eine Speicherzelle des internen EEPROMs,
// Bulkversion
// addr_lsb : LSB der Adresse im EEPROM
// addr_msb : MSB der Adresse im EEPROM
#define HCAN_HMS_INTERNAL_EEPROM_READ_BULK_REQUEST   24

// Geraet sendet 6 gelesene Bytes (Bulk) ab addr_msb:addr_lsb
// d0 : Byte an Offset 0
// d1 : Byte an Offset 1
// d2 : Byte an Offset 2
// d3 : Byte an Offset 3
// d4 : Byte an Offset 4
// d5 : Byte an Offset 5
#define HCAN_HMS_INTERNAL_EEPROM_READ_BULK_REPLAY   25

// Abfrage der Uptime. Das ist die vergangene Zeit seit dem letzten
// Reboot/Reset
#define HCAN_HMS_UPTIME_QUERY   30

// Geraet sendet die Uptime als 32Bit Wert in Sekunden
// u3 : MSB der 32 Bit Uptime
// u2 : 
// u1 : 
// u0 : LSB der 32 Bit Uptime
#define HCAN_HMS_UPTIME_REPLAY   31

// Setzt den Debug-Level fuer die Syslog Botschaften
// level : 1 = CRITICAL, 2 = ERROR, 3 = WARNING, 4 = DEBUG 
#define HCAN_HMS_SYSLOG_LEVEL_SET   32

// Fragt nach der Build Version
#define HCAN_HMS_BUILD_VERSION_QUERY   33

// Liefert die Build Version
// hi : Build Nummer, Hi Teil
// lo : Build Nummer, Lo Teil
#define HCAN_HMS_BUILD_VERSION_REPLAY   34

// Fragt nach den CAN Error Counter Staenden
#define HCAN_HMS_CAN_EC_QUERY   35

// Liefert die CAN Error Counter Staende
// REC : RX Error Counter
// TEC : TX Error Counter
#define HCAN_HMS_CAN_EC_REPLAY   36

// Errechnet die CRC16 Pruefsumme ueber den Flash bis zur angegebenen
// Adresse
// hi : Adresse hi
// lo : Adresse lo
#define HCAN_HMS_FLASH_CRC16_QUERY   37

// Liefert die errechnete CRC16 Pruefsumme ueber den Flash
// hi : CRC16 hi
// lo : CRC16 lo
#define HCAN_HMS_FLASH_CRC16_REPLAY   38

// Fragt nach der Anzahl der Schleifen/sec
#define HCAN_HMS_LOOP_THROUGHPUT_QUERY   39

// Liefert die Anzahl der Schleifen/sec
// hi : MSB
// lo : LSB
#define HCAN_HMS_LOOP_THROUGHPUT_REPLAY   40

// Fragt nach der Anzahl der bisher empfangenen Frames
#define HCAN_HMS_RX_RECEIVED_QUERY   41

// Liefert die Anzahl der bisher empfangenen Frames (32bit)
// d0 : MSB
// d1 : 
// d2 : 
// d3 : LSB
#define HCAN_HMS_RX_RECEIVED_REPLAY   42

// Fragt nach der Anzahl der bisher verlorenen Frames
#define HCAN_HMS_RX_LOST_QUERY   43

// Liefert die Anzahl der bisher verlorenen Frames (32bit)
// d0 : MSB
// d1 : 
// d2 : 
// d3 : LSB
#define HCAN_HMS_RX_LOST_REPLAY   44

// Fragt nach der Anzahl der bisher gesendeten Frames
#define HCAN_HMS_TX_SENT_QUERY   45

// Liefert die Anzahl der bisher gesendeten Frames (32bit)
// d0 : MSB
// d1 : 
// d2 : 
// d3 : LSB
#define HCAN_HMS_TX_SENT_REPLAY   46

// Fragt nach der Anzahl der bisher verworfenen Frames
#define HCAN_HMS_TX_DROPPED_QUERY   47

// Liefert die Anzahl der bisher verworfenen Frames (32bit)
// d0 : MSB
// d1 : 
// d2 : 
// d3 : LSB
#define HCAN_HMS_TX_DROPPED_REPLAY   48

// Setzt alle RX/TX Stats Counter zurueck
#define HCAN_HMS_RX_TX_STATS_RESET   49
// Service: SLS

// Meldet den Wert des Resetflag-Registers direkt nach dem Booten
// flag : siehe Atmega8 Referenz-Handbuch, S.39
#define HCAN_SLS_BOOT_RESETFLAG_LOG   1

// Meldet eine fehlerhafte CRC16 Firmware Pruefsumme
#define HCAN_SLS_FIRMWARE_CRC16_ERROR   2

// Firmware und Config sind nicht kompatibel
// eds_block_type : betroffener EDS Block-Typ
// eds_addr_hi : EDS Adresse Hi
// eds_addr_lo : EDS Adresse Lo
// size_in_config : Groesse im EDS
// size_in_firmware : Groesse in der Firmware
#define HCAN_SLS_FIRMWARE_CONFIG_COMPAT_ERROR   3

// Geraet spuckt Debug-Infos aus
// context : 
#define HCAN_SLS_DUMP_DEBUG_INFO   4
// Service: HES

// Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an
// -> aus, aus -> an)
// gruppe : Taster-Gruppe, auf die der Taster konfiguriert ist
// index : Index des Taster-Ports; nur noetig fuer Logging-Zwecke, falls
// Gruppe nicht eindeutig
#define HCAN_HES_TASTER_DOWN   1

// Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an
// -> aus, aus -> an)
// gruppe : Taster-Gruppe
// index : Index des Taster-Ports
#define HCAN_HES_TASTER_UP   2

// Das Device meldet seinen Hochlauf.
#define HCAN_HES_BOARD_ACTIVE   4

// Reaktion: Power-Port(s) schalten sich an
// gruppe : Gruppe
#define HCAN_HES_POWER_GROUP_ON   10

// Power-Port(s) schalten sich aus
// gruppe : Gruppe
#define HCAN_HES_POWER_GROUP_OFF   11

// Abfrage des Status eines oder mehrerer Power-Ports (ob an oder aus)
// gruppe : Gruppe
#define HCAN_HES_POWER_GROUP_STATE_QUERY   12

// Antwort eines Power-Ports mit dem aktuellen Status
// gruppe : Gruppe
// status : 1 = an, 0 = aus
// timer : dekrementiert
#define HCAN_HES_POWER_GROUP_STATE_REPLAY   13

// Anfrage an eine Lichtzone o.ae. nach der Timer Stand 
// gruppe : Gruppe
#define HCAN_HES_POWER_GROUP_TIMER_QUERY   14

// Antwort einer Lichtzone mit dem Timer Stand 
// gruppe : Gruppe
// timer_hi : Timer-Stand, High Byte
// timer_lo : Timer-Stand, Low Byte
#define HCAN_HES_POWER_GROUP_TIMER_REPLAY   15

// Setzt den aktuellen Timer-Stand einer Lichtzone
// gruppe : Gruppe
// timer_hi : Timer-Stand, High Byte
// timer_lo : Timer-Stand, Low Byte
#define HCAN_HES_POWER_GROUP_SET_TIMER   16

// Power-Port meldet nach Aenderung seinen Status
// gruppe : Gruppe
// status : 1 = an, 0 = aus
#define HCAN_HES_POWER_GROUP_STATE_INFO   17

// Faehrt einen Rolladen auf die gegebene Position
// gruppe : Gruppe
// position : Position im Bereich [0..100]; 2-Taster: 200=0%, 201=100%;
// 100% = oben
#define HCAN_HES_ROLLADEN_POSITION_SET   20

// Fragt einen Rolladen nach der aktuellen Position
// gruppe : Gruppe
#define HCAN_HES_ROLLADEN_POSITION_REQUEST   21

// Der Rolladen antwortet mit der aktuellen Position
// gruppe : Gruppe
// position : Position im Bereich [0..100]; 100% = oben
#define HCAN_HES_ROLLADEN_POSITION_REPLAY   22

// Definiert die aktuelle Position eines Rolladen (!)
// gruppe : Gruppe
// position : Position im Bereich [0..100]; 100% = oben
#define HCAN_HES_ROLLADEN_DEFINE_POSITION   23

// Besagt, dass sich die Rolladen-Position geaendert hat
// gruppe : Gruppe
// position : Position im Bereich [0..100]; 100% = oben
// cmdsrc_hi : Source-Adresse des Befehls (0 = manuell ueber Taster), MSB
// cmdsrc_lo : LSB
#define HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO   24

// Schalter ist an
// gruppe : Gruppe
#define HCAN_HES_SCHALTER_GROUP_ON   25

// Schalter ist aus
// gruppe : Gruppe
#define HCAN_HES_SCHALTER_GROUP_OFF   26

// Veranlasst einen 1Wire Scan an gebenen Pin
// pin : Pin ID des 1Wire Ports im Bereich [0..7]
#define HCAN_HES_1WIRE_DISCOVER   30

// ein 1Wire Device ist gefunden worden, Message Teil 1
// id0 : 1. Byte der 1Wire id[8] (Checksum dabei)
// id1 : 2. Byte
// id2 : 3. Byte
// id3 : 4. Byte
#define HCAN_HES_1WIRE_DISCOVERED_PART_1   31

// ein 1Wire Device ist gefunden worden, Message Teil 1
// id4 : 5. Byte der 1Wire id[8] (Checksum dabei)
// id5 : 6. Byte
// id6 : 7. Byte
// id7 : 8. Byte
#define HCAN_HES_1WIRE_DISCOVERED_PART_2   32

// ein 1Wire Fehler trat auf
// error : 3 = NoSensorFound, 4 = BusError
#define HCAN_HES_1WIRE_ERROR   33

// Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers
// gruppe : Gruppe
// temp_hi : MSB
// temp_lo : LSB; 4 LSB sind Nachkommabits
#define HCAN_HES_1WIRE_TEMPERATURE   34

// fragt einen Temperatursensor nach der aktuellen Temperatur
// gruppe : Gruppe
#define HCAN_HES_1WIRE_TEMPERATURE_QUERY   35

// Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers als
// Antwort auf 1WIRE_TEMPERATURE_QUERY
// gruppe : Gruppe
// temp_hi : MSB
// temp_lo : LSB; 4 LSB sind Nachkommabits
#define HCAN_HES_1WIRE_TEMPERATURE_REPLAY   36

// Meldung fuer einen offenen Reedkontakt an Fenster oder Tuer
// gruppe : Gruppe
#define HCAN_HES_REEDKONTAKT_OFFEN   40

// Fragt nach dem Status eines Reedkontakts
// gruppe : Gruppe
#define HCAN_HES_REEDKONTAKT_STATE_QUERY   41

// Status Antwort eines Reedkontakts
// gruppe : Gruppe
// state : Zustand: 0 = zu, 1 = offen
#define HCAN_HES_REEDKONTAKT_STATE_REPLAY   42

// Zustandsaenderung eines Reedkontakts
// gruppe : Gruppe
// state : Zustand: 0 = zu, 1 = offen
#define HCAN_HES_REEDKONTAKT_STATE_CHANGE   43

// Zustandsaenderung eines PV Sensors
// gruppe : Gruppe
#define HCAN_HES_PVSENSOR_STATE_REQUEST   44

// Zustandsaenderung eines PV Sensors
// gruppe : Gruppe
// state : Zustand: 0 = 0V, 1 = 230V
#define HCAN_HES_PVSENSOR_STATE_REPLAY   45

// Zustandsaenderung eines PV Sensors
// gruppe : Gruppe
// state : Zustand: 0 = 0V, 1 = 230V
#define HCAN_HES_PVSENSOR_STATE_CHANGE_INFO   46

// UserPanel fragt nach Heizungsdetails
// id : Heizungs-ID
#define HCAN_HES_HEIZUNG_DETAILS_REQUEST   50

// Heizung ist aus (Off-Mode)
// id : Heizungs-ID
#define HCAN_HES_HEIZUNG_MODE_OFF_DETAILS   51

// Heizung schaltet aus
// id : Heizungs-ID
#define HCAN_HES_HEIZUNG_SET_MODE_OFF   52

// Manueller Modus incl Parameter
// id : Heizungs-ID
// rate : Heiz-Rate
// duration_hi : Restdauer (MSB), 0 = unbegrenzt
// duration_lo : Restdauer (LSB)
#define HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS   53

// Manueller Modus incl Parameter
// id : Heizungs-ID
// rate : Heiz-Rate
// duration_hi : Restdauer (MSB), 0 = unbegrenzt
// duration_lo : Restdauer (LSB)
#define HCAN_HES_HEIZUNG_SET_MODE_MANUAL   54

// Thermostat Modus incl Parameter
// id : Heizungs-ID
// temp_hi : Soll-Temperatur (MSB)
// temp_lo : Soll-Temperatur (LSB)
// duration_hi : Restdauer (MSB), 0 = unbegrenzt
// duration_lo : Restdauer (LSB)
#define HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS   55

// Set Thermostat Modus incl Parameter
// id : Heizungs-ID
// temp_hi : Soll-Temperatur (MSB)
// temp_lo : Soll-Temperatur (LSB)
// duration_hi : Restdauer (MSB), 0 = unbegrenzt
// duration_lo : Restdauer (LSB)
#define HCAN_HES_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS   56

// Automatik Modus incl Parameter
// id : Heizungs-ID
// temp_hi : Soll-Temperatur (MSB)
// temp_lo : Soll-Temperatur (LSB)
// timerange_id : Zeitbereichs-ID
#define HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS   57

// Set Automatik Modus
// id : Heizungs-ID
#define HCAN_HES_HEIZUNG_SET_MODE_AUTOMATIK   58

// UserPanel fragt nach T(ist)
// id : Heizungs-ID
#define HCAN_HES_HEIZUNG_TIST_REQUEST   59

// Controllerboard liefert T(ist)
// id : Heizungs-ID
// temp_hi : Ist-Temperatur (MSB)
// temp_lo : Ist-Temperatur (LSB)
#define HCAN_HES_HEIZUNG_TIST_REPLAY   60

// Heizung braucht Waerme oder nicht
// id : Heizungs-ID
// value : [0..100]: 0 = kein Bedarf
#define HCAN_HES_HEIZUNG_WAERMEBEDARF_INFO   61

// Fragt die Heizungssteuerung nach ihrem Stand
#define HCAN_HES_HEIZSTEUERUNG_STATE_REQUEST   62

// die (einzige) Heizungssteuerung antwortet
// VL_soll : Vorlauf-Soll
// VL_ist : Vorlauf-Ist
// RL_ist : Ruecklauf-Ist
// BF : Brennerfreigabe
#define HCAN_HES_HEIZSTEUERUNG_STATE_REPLAY   63

// Heizungssteuerung liefert den aktuellen Status
// VL_soll : Vorlauf-Soll
// GWB_hi : Gesamt-Waermebedarf
// GWB_lo : Gesamt-Waermebedarf
// BF : Brennerfreigabe
#define HCAN_HES_HEIZSTEUERUNG_STATE_INFO   64

// gibt die Soll-VL Temperatur vor
// VL_soll : Vorlauf-Soll
#define HCAN_HES_HEIZSTEUERUNG_SET_VL   65

// Fragt nach einer Solltemp/Zeitzonen Config Zeile
// id : Heizungs-ID
// idx : Index der Config Zeile [0..7]
#define HCAN_HES_HEIZUNG_SOLLTEMP_LINE_REQUEST   66

// Controllerboard liefert Zeile
// id : Heizungs-ID
// idx : Index der Config Zeile
// temp_hi : Soll-Temperatur (MSB)
// temp_lo : Soll-Temperatur (LSB)
// zeitzone_id : ID der Zeitzone
#define HCAN_HES_HEIZUNG_SOLLTEMP_LINE_REPLAY   67

// schreibt neue Solltemp Zeile: ACHTUNG: zeitzone_id wird bisher NICHT
// geschrieben!
// id : Heizungs-ID
// idx : Index der Config Zeile
// temp_hi : Soll-Temperatur (MSB)
// temp_lo : Soll-Temperatur (LSB)
// zeitzone_id : ID der Zeitzone
#define HCAN_HES_HEIZUNG_SOLLTEMP_WRITE_LINE   68

// schreibt neue Solltemp Zeile
// id : Heizungs-ID
// idx : Index der Config Zeile
#define HCAN_HES_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK   69

// Wecker incl Parameter
// id : Wecker-ID
// duration_hi : Timer in x Sekunden (MSB)
// duration_lo : Timer in x Sekunden (LSB)
#define HCAN_HES_WECKER_SET   72

// Wecker incl Parameter
// id : Wecker-ID
#define HCAN_HES_WECKER_DETAILS_REQUEST   73

// Wecker incl Parameter
// id : Wecker-ID
// duration_hi : Timer in x Sekunden (MSB)
// duration_lo : Timer in x Sekunden (LSB)
#define HCAN_HES_WECKER_DETAILS   74

// Liefert die Helligkeit am ADC (Aufloesung: 10Bit)
// guppe : Helligkeits-Gruppe
// brightness_hi : hi byte der Helligkeit
// brightness_lo : lo hyte der Helligkeit
#define HCAN_HES_HELLIGKEITS_INFO   71

// Liefert die Position des Potis (Aufloesung: 10Bit)
// guppe : Poti-Gruppe
// pos_hi : hi byte der Position
// pos_lo : lo hyte der Position
#define HCAN_HES_POTI_POS_CHANGED   70

// Meldet die festgestellten Fehler der letzten Empfangsminute
// id : Identifikation des DCF77-Moduls
// error : Fehlercode
#define HCAN_HES_DCF77_ERROR   80

// Liefert die Länge des letzten Pulse in 1/100s
// id : Identifikation des DCF77-Moduls
// length : Pulslaenge
#define HCAN_HES_DCF77_PULSE_DURATION   81

// Reaktion: Controllerboard laed die Config aus dem EEPROM neu
#define HCAN_HES_CONFIG_RELOAD   100

// Fragt ab, wieviel bytes des Config RAM verwendet werden
#define HCAN_HES_CONFIG_RAM_USAGE_REQUEST   101

// Liefert die Anzahl der verwendeten Bytes im Config RAM
// usage_hi : MSB des RAM Usage
// usage_lo : LSB des RAM Usage
#define HCAN_HES_CONFIG_RAM_USAGE_REPLAY   102

// Fragt nach den Zeitzonen-Details
// gruppe : Zeitzonen-ID
#define HCAN_HES_ZEITZONE_DETAILS_REQUEST   110

// Liefert die Zeitzonen Details
// gruppe : Zeitzonen-ID
// day_pattern : Binaeres Day Pattern
// from_hour : 
// from_minute : 
// to_hour : 
// to_minute : 
#define HCAN_HES_ZEITZONE_DETAILS_REPLAY   111

// Liefert neue Zeitzonen Details, welche im EEPROM gespeichert werden
// gruppe : Zeitzonen-ID
// day_pattern : Binaeres Day Pattern
// from_hour : 
// from_minute : 
// to_hour : 
// to_minute : 
#define HCAN_HES_ZEITZONE_WRITE_DETAILS   112

// Liefert ein ACK auf den Schreibvorgang
// gruppe : Zeitzonen-ID
#define HCAN_HES_ZEITZONE_WRITE_DETAILS_ACK   113

// Testet, ob die Zeitzone matched, d.h. zutrifft
// gruppe : Zeitzonen-ID
#define HCAN_HES_ZEITZONE_TEST_MATCH_REQUERST   114

// Liefert die Info, ob die Zeitzone matched
// gruppe : Zeitzonen-ID
// match : 0 = false, 1 = true
#define HCAN_HES_ZEITZONE_TEST_MATCH_REPLAY   115

// Error message for ports Device
// address : Adresse oder ID des nicht gefundenen Expanders
#define HCAN_HES_EXPANDER_NOT_FOUND   120

// Error message for ports Device
#define HCAN_HES_ADDITIONAL_PORTS_DEVICE_IGNORED   121
// Service: RTS

// Meldet die aktuelle Zeit
// level : Time Level; regelt den Master-/Slave Betrieb
// day_of_week : Tag der Woche: [1..7] == [ Montag .. Sonntag ]
// hour : Stunde [0..23]
// minute : Minute [0..59]
// second : Sekunde [0..59]
#define HCAN_RTS_TIME_INFO   1

// Meldet das aktuelle Datum
// level : Date Level; regelt den Master-/Slave Betrieb
// day_of_month : Tag des Monats [1..31] 
// month_of_year : Monat [1..12] 
// year : Jahr 2000 + [1..255] = [ 2001..2255 ]
#define HCAN_RTS_DATE_INFO   2

// Fragt die aktuelle Zeit: Replay ist TIME_INFO
#define HCAN_RTS_TIME_REQUEST   3

// Fragt das aktuelle Datum: Replay ist DATE_INFO
#define HCAN_RTS_DATE_REQUEST   4
// Service: EDS

// Fragt nach dem naechsten Block
// handle_hi : Iterator, hi byte
// handle_lo : Iterator, lo byte
#define HCAN_EDS_GET_NEXT_BLOCK   1

// Liefert den naechsten Block
// handle_hi : Iterator, hi byte
// handle_lo : Iterator, lo byte
// type : Typ des Blocks
// size : Groesse des Blocks
#define HCAN_EDS_GET_NEXT_BLOCK_REPLAY   2

// Fasst alle freien Bloecke zusammen
#define HCAN_EDS_DEFRAGMENT   3

// Alloziert einen Block
// type : Typ des Blocks
// size : Groesse des Blocks
#define HCAN_EDS_ALLOCATE_BLOCK   4

// Liefert ein Handle auf den allozierten Block, oder 0, falls
// fehlgeschlagen
// handle_hi : hi byte
// handle_lo : lo byte
#define HCAN_EDS_ALLOCATE_BLOCK_REPLAY   5

// Gibt einen Block frei
// handle_hi : hi byte
// handle_lo : lo byte
#define HCAN_EDS_FREE_BLOCK   6

// Gibt einen Block frei
// result : 0 = ok, 1 = error
#define HCAN_EDS_FREE_BLOCK_REPLAY   7

// Fordert 4 Bytes aus einem Block an
// handle_hi : hi byte
// handle_lo : lo byte
// index : lo byte
#define HCAN_EDS_READ_FROM_BLOCK   8

// Liefert 4 Bytes aus einem Block an
// d0 : Byte 0
// d1 : Byte 1
// d2 : Byte 2
// d3 : Byte 3
#define HCAN_EDS_READ_FROM_BLOCK_REPLAY   9

// Schreibt ein Byte in einem Block
// handle_hi : hi byte
// handle_lo : lo byte
// index : lo byte
// d0 : Byte
#define HCAN_EDS_WRITE_TO_BLOCK   10

// Anwort auf ein geschriebenes Byte
#define HCAN_EDS_WRITE_TO_BLOCK_REPLAY   11

// Formatiert den EDS Bereich des EEPROM
#define HCAN_EDS_FORMAT   12
// Service: USVS

// Fragt den aktuellen USV Zustand
#define HCAN_USVS_STATE_REQUEST   1

// Meldet den aktuellen USV Zustand
// state : 0 = Netzbetrieb, 1 = Batterie
#define HCAN_USVS_STATE_INFO   2

// Fragt nach einer USV Spannung
// Ux : 1-5
#define HCAN_USVS_VOLTAGE_REQUEST   3

// Meldet die USV Spannung
// Ux_hi : hi teil
// Ux_lo : lo teil
#define HCAN_USVS_VOLTAGE_REPLAY   4

// Fragt nach den Spannungs-Statistiken
// Ux : 1-5
#define HCAN_USVS_VOLTAGE_STATS_REQUEST   5

// Fragt nach den Spannungs-Statistiken
// U : 1-5
// min_hi : Minimum
// min_lo : Minimum
// max_hi : Maximum
// max_lo : Maximum
#define HCAN_USVS_VOLTAGE_STATS_REPLAY   6

// Setzt alle Spannungs-Statistiken zurueck
#define HCAN_USVS_VOLTAGE_STATS_RESET   7
// Service: EBUS

// Feuerungsautomat Block 1
// status : LSB bis MSB: LDW,GDW,WS,Flamme,Ventil1,Ventil2,UWP,Alarm
// stellgrad : 0-100%
// KT : 0-100 Grad, (DATA1c)
// RT : 0-100 Grad
// BT : 0-100 Grad
// AT : -30-50 Grad (SIGN CHAR)
#define HCAN_EBUS_FA_BLOCK1_INFO   1

// Regler Daten an FA Fragment 1
// status : Waermeanforderung (s.S.24)
// aktion : Ein/Ausschalten etc. (s.S.24)
// kesseltemp_soll_lo : Grad, DATA2c, lo Byte
// kesseltemp_soll_hi : Grad, DATA2c, hi Byte
// kesseldr_soll_lo : bar, DATA2b, lo Byte
// kesseldr_soll_hi : bar, DATA2b, hi Byte
#define HCAN_EBUS_REGLER_DATEN_INFO_FRG1   2

// Regler Daten an FA Fragment 2
// stellgrad : Modulationsgrad
// ww_soll : Brauchwasser-Sollwert
// brennstoff : 00=?, 01=Gas, 10=Oel, 11=?
#define HCAN_EBUS_REGLER_DATEN_INFO_FRG2   3
// Service: WS

// Wetterdaten
// niederschlag : 1 = Niederschlag
// wind : Impulse pro sec
// licht_ost : Sonne/Helligkeit
// licht_sued : Sonne/Helligkeit
// licht_west : Sonne/Helligkeit
#define HCAN_WS_WETTER_INFO   1

// Licht, ueber 5min gemittelt
// licht_ost : Helligkeit
// licht_sued : Helligkeit
// licht_west : Helligkeit
#define HCAN_WS_LICHT_STATS_5MIN   2

// Wind, ueber 5min gemittelt
// wind : Impulse pro sec
#define HCAN_WS_WIND_STATS_5MIN   3
// Service: OAPS

// Fragt nach der Anzahl der Objekte
#define HCAN_OAPS_OBJECT_COUNT_REQUEST   1

// Anzahl der Objekte
// count : Anzahl der Objekte
#define HCAN_OAPS_OBJECT_COUNT_REPLAY   2

// Anfrage der Attribute eines Objekts
// oid : Objekt ID
#define HCAN_OAPS_OBJECT_ATTRIBUTE_REQUEST   3

// Attribute eines Knotens
// oid : Objekt ID
// oclass : Klasse des Objekts
// p1 : opt. Parameter
// p2 : opt. Parameter
// p3 : opt. Parameter
// p4 : opt. Parameter
#define HCAN_OAPS_OBJECT_ATTRIBUTE_REPLAY   4

// Fragt nach der Anzahl der Felder eines Objekts
// oid : Objekt ID
#define HCAN_OAPS_FIELD_COUNT_REQUEST   5

// Anzahl der Objekte
// oid : Objekt ID
// count : Anzahl der Felder
#define HCAN_OAPS_FIELD_COUNT_REPLAY   6

// Anfrage der Attribute eines Feldes
// oid : Objekt ID
// fid : Field ID
#define HCAN_OAPS_FIELD_ATTRIBUTE_REQUEST   7

// Attribute eines Feldes
// oid : Objekt ID
// fid : Field ID
// fclass : Variablen-Typ bzw. Klasse des Feldes
// flags : bit0 = rw, bit1 = persistent, bit2 = array
// arraysize : 
#define HCAN_OAPS_FIELD_ATTRIBUTE_REPLAY   8

// Fragt nach dem aktuellen Wert des Feldes
// oid : Objekt ID
// fid : Field ID
// idx : opt. Array Index
#define HCAN_OAPS_FIELD_READ_REQUEST   9

// Liefert den Wert des Feldes
// oid : Objekt ID
// fid : Field ID
// idx : opt. Array Index
// value_hi : 
// value_lo : 
#define HCAN_OAPS_FIELD_READ_REPLAY   10

// 
// oid : Objekt ID
// fid : Field ID
// idx : opt. Array Index
// value_hi : 
// value_lo : 
#define HCAN_OAPS_FIELD_WRITE_REQUEST   11

// 
// oid : Objekt ID
// fid : Field ID
// idx : opt. Array Index
#define HCAN_OAPS_FIELD_WRITE_REPLAY   12

#endif
