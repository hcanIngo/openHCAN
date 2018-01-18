socketCAN - MQTT-Paho-Client: HCAN-socketCAN <-> mqtt


Installationsabhaengigkeiten: 
-----------------------------
1. Der MQTTPaho-Client (Embedded MQTTPacket) benoetigt keine Shared-Lib. 

2. mosquitto MQTT-Broker (Datenvermittlungszentrale):
sudo apt-get install mosquitto
-> mosquitto startet automatisch.  Kontrolle: systemctl status mosquitto
-----------------------------

Testmoeglichkeit z.B. per MQTTLens 
1. subscribe a topic="cb>"
2. publish   a topic="cb<", Msg=ON/2


...
tt@bananapi:~/openHCAN/hcan4mqttcp$ ./hcan4mqttpc -D

top -u tt
...