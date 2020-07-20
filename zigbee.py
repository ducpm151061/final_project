import time
import sys
import paho.mqtt.client as mqtt
import json
import random
from time import sleep
import serial
import os
import threading
import itertools
import json
from Crypto.Cipher import AES
import base64
import re
global listnode
global listtoken
listnode=['F761','4047','C752']
listtoken=['zigbee01','zigbee02','zigbee03']
THINGSBOARD_HOST ='14.162.38.93'

#ACCESS_TOKEN = 'testdevice'

sensor_data = {'ID':0,'temperature': 0,'humidity':0}
sensor_data2={'lux':0}
ser=serial.Serial(
        port='/dev/serial/by-id/usb-Prolific_Technology_Inc._USB-Serial_Controller-if00-port0',
        baudrate=9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=0.5
)
def node(diachinode,token): 
        lenhgui='P2P '+diachinode+' Gui#'
        print(lenhgui)
        ser.write(lenhgui.encode())
        ser.flush()
        time_vaovonglap=time.time()
        while((time.time()-time_vaovonglap)<3.0):
                s=ser.readline().decode('utf-8')
                print(s)
                if len(s)>6:
                        cipher=bytes(s,'utf-8')
                        key=b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
                        iv=b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
                        tem=base64.b64decode(cipher)
                        realcipher=tem[0:16]
                        aes=AES.new(key,AES.MODE_CBC,iv)
                        decode=aes.decrypt(realcipher).decode('utf-8')
                        print(decode)
                        temp = re.findall(r'\d+', decode) 
                        res = list(map(int, temp))
                        sensor_data['ID']=res[0] 
                        sensor_data['temperature'] = res[1]/10
                        sensor_data['humidity']=res[2]/10
                        client = mqtt.Client()
                        client.username_pw_set(token)
                        try:
                            client.connect(THINGSBOARD_HOST, 1883)
                        #client.loop_start()
                            client.publish('v1/devices/me/telemetry',json.dumps(sensor_data))
                            #client.disconnect()
                        except (TimeoutError,OSError):
                            continue
                        #client.loop_stop()
                        time.sleep(1)
                else :
                    continue
def lux(diachinode,token):
        lenhgui='P2P '+diachinode+' Gui#'
        print(lenhgui)
        ser.write(lenhgui.encode())
        ser.flush()
        time_vaovonglap=time.time()
        while((time.time()-time_vaovonglap)<3.0):
                s=ser.readline().decode('utf-8')
                print(s)
                if len(s)>1:
                        sensor_data2['lux']=s
                        client=mqtt.Client()
                        client.username_pw_set(token)
                        try:
                            client.connect(THINGSBOARD_HOST,1883)
                            client.publish('v1/devices/me/telemetry',json.dumps(sensor_data2))
                            client.disconnect()
                        except (TimeoutError,OSError):
                            continue
                else:
                    continue

try:
        while True:
                node(listnode[0],listtoken[0])
                node(listnode[1],listtoken[1])
                lux(listnode[2],listtoken[2])
except KeyboardInterrupt:
    pass
