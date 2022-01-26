socketCAN - MQTT-Paho-Client: HCAN-socketCAN <-> mqtt   incl. discovery


Installationsabhaengigkeiten: 
-----------------------------
1. Der MQTTPaho-Client (Embedded MQTTPacket) benoetigt keine Shared-Lib. 

2. mosquitto MQTT-Broker (Datenvermittlungszentrale):
sudo apt-get install mosquitto
-> mosquitto startet automatisch.  Kontrolle: systemctl status mosquitto
-----------------------------

Testmoeglichkeit z.B. per MQTTLens, MQTTfx oder MQTT-Explorer
1. subscribe topic="ha"
2. publish   topic="ha", Msg=



============================================================
https://www.home-assistant.io/docs/mqtt/discovery/



...
tt@bananapi:~/openHCAN/hcan4mqttha$ ./hcan4mqttha -D

top -u tt
...
