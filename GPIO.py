#!/usr/bin/python3
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time
import json
import serial



iot_hub ="113.178.79.147"
port = 1883

username ="ble01"
password =""
topic ="v1/devices/me/telemetry"

client = mqtt.Client()
client.username_pw_set(username,password)
client.connect(iot_hub,port)
print("Connection success")

bleSer = serial.Serial('/dev/ttyUSB0', baudrate = 9600, stopbits = serial.STOPBITS_ONE)
#bluetoothSerial.open() '/dev/ttyAMA1'  /dev/ttyUSB0
setup1 = "AT"
setup2 = "AT+ROLE0"
bleSer.write(setup1.encode())
time.sleep(1)
bleSer.write(setup2.encode())
data=dict()
while True:
    #if(bleSer.inWaiting() > 0):

     info = bleSer.readline()
     data2 = info.decode()
     data2 = data2.rstrip()

     if data2.rfind('a')==0:
        data2 = data2.lstrip('a')
        print(data2)
        data["temperature"] = data2
        data_out = json.dumps(data)
        client.publish(topic,data_out,0)
        time.sleep(2)
     elif data2.rfind('b')==0:
        data2 = data2.lstrip('b')
        print(data2)
        data["ec"] = data2
        data_out = json.dumps(data)
        client.publish(topic,data_out,0)
        time.sleep(2)
     else:
        data2 = data2.lstrip('c')
        print(data2)
        data["ph"] = data2
        data_out = json.dumps(data)
        client.publish(topic,data_out,0)
        time.sleep(2)

