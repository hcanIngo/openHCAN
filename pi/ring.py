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
    #client.subscribe("$SYS/#") # alle System-Msg. vom Broker
    #client.subscribe("hc/#")
    client.subscribe("hc/light/37/-->/state") # DG, aber nur die 81 setzt Trafo unter Spg. 
    client.subscribe("hc/switch/81/-->/state") # EG Audiohinweis

def on_message(client, userdata, msg):
  payload = msg.payload.decode('utf-8') # bin-str 2 str  (oder: 'ascii')
  if payload == "ON":
    print("\nklingeln")
    print(msg.topic+" "+payload)
  else:
    print("\n--aus--")
    print(msg.topic+" "+payload)
  syslog.syslog(syslog.LOG_INFO, msg.topic+" "+str(payload))
  myCmd = "mplayer /etc/hcan/doorbell.mp3"
  os.system(myCmd)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message


#syslog.openlog("ring", LOG_CONS|LOG_NDELAY|LOG_PERROR|LOG_PID, LOG_LOCAL7)

client.username_pw_set(username="hcan",password="n_A_c")
client.connect("localhost", 1883, 60) # "192.168.1.30" "mqtt.eclipseprojects.io"
client.loop_forever()
