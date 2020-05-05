import time
import subprocess
import os
import paho.mqtt.client as mqtt
import json
import paho.mqtt.subscribe as subscribe
sensor_data = {'temperature': 0, 'humidity': 0}
THINGSBOARD_HOST = '192.168.1.172'
ACCESS_TOKEN = 'testdevice2'
def on_message_print(client, userdata, message):
    print(message.payload.decode('utf8'))
try:
	while True:
		#nhietdo=subscribe.callback(on_message_print, "temp", hostname="192.168.1.177")
		msg = subscribe.simple("hum", hostname="192.168.1.8")
		nhietdo=msg.payload.decode('utf8')
		client=mqtt.Client();
		client.username_pw_set(ACCESS_TOKEN)
		client.connect(THINGSBOARD_HOST,1883)
		#nhietdo=os.popen("mosquitto_sub -h localhost -t temp")
		#t=nhietdo.readline()
		#doam=os.popen("mosquitto_sub -h localhost -t hum")
		#h=doam.readline()
		sensor_data['temperature']=nhietdo
		#sensor_data['humidity']=h
		print("nhiet do la: ",nhietdo)
		#print(h)
		client.publish('v1/devices/me/telemetry',json.dumps(sensor_data))
		time.sleep(1)
except KeyboardInterrupt:
	pass
