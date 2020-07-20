import time
import subprocess
import os
import paho.mqtt.client as mqtt
import json
import paho.mqtt.subscribe as subscribe
from Crypto.Cipher import AES
import base64
sensor_data = {'ID': 0, 'temperature': 0, 'humidity': 0}
listnode = ['temp1', 'temp2', 'temp3']
listtoken = ['wifi01', 'wifi02', 'wifi03']
THINGSBOARD_HOST = '14.162.38.93'
#ACCESS_TOKEN = 'wifi02'


def on_message_print(client, userdata, message):
    print(message.payload.decode('utf8'))


def node(node, token):
	try:
		msg = subscribe.simple(node, hostname="192.168.0.6")
		if(len(msg.payload) > 5):
			t1 = time.time()
			cipher = bytes(msg.payload.decode('utf-8'), 'utf-8')
			print(cipher)
			key = b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
			iv = b'\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61'
			tem = base64.b64decode(cipher)
			realcipher = tem[0:16]
			aes = AES.new(key, AES.MODE_CBC, iv)
			decode = aes.decrypt(realcipher).decode('utf-8')
			print(time.time()-t1)
			print(decode)
			l = decode.split(',')
			res = [int(sub.split('=')[1]) for sub in l]
			id = res[0]
			temperature = res[1]
			humidity = res[2]
			print(id)
			print(temperature)
			print(humidity)
			sensor_data['ID'] = res[0]
			sensor_data['temperature'] = res[1]
			sensor_data['humidity'] = res[2]
			client = mqtt.Client()
			client.username_pw_set(token)
			client.connect(THINGSBOARD_HOST, 1883, 120)
			client.publish('v1/devices/me/telemetry', json.dumps(sensor_data))
			time.sleep(2)
	except (TimeoutError,OSError):
		print("ERROR CONNECTION")


try:
    while True:
        #nhietdo=subscribe.callback(on_message_print, "temp", hostname="192.168.1.177")
        node(listnode[0], listtoken[0])
        node(listnode[1], listtoken[1])
        node(listnode[2], listtoken[2])
except KeyboardInterrupt:
    pass

