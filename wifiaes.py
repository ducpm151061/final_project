import time
import subprocess
import os
import paho.mqtt.client as mqtt
import json
import paho.mqtt.subscribe as subscribe
from Crypto.Cipher import AES
import base64
sensor_data = {'ID':0,'temperature': 0, 'humidity': 0}
THINGSBOARD_HOST = '40.117.86.51'
ACCESS_TOKEN = 'testdevice'
def on_message_print(client, userdata, message):
    print(message.payload.decode('utf8'))
try:
	while True:
		#nhietdo=subscribe.callback(on_message_print, "temp", hostname="192.168.1.177")
		msg = subscribe.simple("temp", hostname="192.168.1.111")
		if(len(msg.payload)>5):
			t1=time.time()
			cipher=bytes(msg.payload.decode('utf-8'),'utf-8')
			print(cipher)
			key=b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
			iv=b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
			tem=base64.b64decode(cipher)
			realcipher=tem[0:16]
			aes=AES.new(key,AES.MODE_ECB)
			decode=aes.decrypt(realcipher).decode('utf-8')
			print(time.time()-t1)
			print(decode)
			l=decode.split(',')
			res = [int(sub.split('=')[1]) for sub in l]
			id=res[0]
			temperature=res[1]
			humidity=res[2]
			print(id)
			print(temperature)
			print(humidity)
			client=mqtt.Client()
			client.username_pw_set(ACCESS_TOKEN)
			client.connect(THINGSBOARD_HOST,1883)
			sensor_data['ID']=res[0]
			sensor_data['temperature']=res[1]
			sensor_data['humidity']=res[2]
			client.publish('v1/devices/me/telemetry',json.dumps(sensor_data))
		else:
			continue
		time.sleep(2)
except KeyboardInterrupt:
	pass
