#!/usr/bin/env python3
# https://pypi.org/project/paho-mqtt/

# sudo apt install python3 python3-pip
# pip install paho-mqtt

# starten: python3 ring.py

import paho.mqtt.client as mqtt
import os
import syslog

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    #client.subscribe("$SYS/#") # alles vom Broker
    #client.subscribe("hc/#") # alles vom Broker
    client.subscribe("hc/light/37/-->/state") # DG
    client.subscribe("hc/switch/81/-->/state") # EG Audiohinweis

def on_message(client, userdata, msg):
  if str(msg.payload) == "ON": # TODO auf bpi (python3:    hc/light/37/-->/state b'ON'): 
    print("\nklingeln")
    print(msg.topic+" "+str(msg.payload))
  else:
    print("\n--aus--")
    print(msg.topic+" "+str(msg.payload))
  syslog.syslog(syslog.LOG_INFO, msg.topic+" "+str(msg.payload))
  myCmd = "mplayer /etc/hcan/doorbell.mp3"
  os.system(myCmd)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


#syslog.openlog("ring", LOG_CONS|LOG_NDELAY|LOG_PERROR|LOG_PID, LOG_LOCAL7)

client.username_pw_set(username="hcan",password="n_A_c")
client.connect("localhost", 1883, 60) # "192.168.1.30" "mqtt.eclipseprojects.io"
client.loop_forever()
