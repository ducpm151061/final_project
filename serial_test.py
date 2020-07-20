
import serial
import re
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
import json
from ast import literal_eval

THINGSBOARD_HOST = '14.231.31.52'
port=1883
INTERVAL = 2
next_reading = time.time()
sensor_data = {"temp" :0,"hum": 0,"lux": 0,"CO2": 0,"VOC": 0}
token_dict = {"1": "zwave01" , "2": "9tSEvjdRz3S3nEGEeDPC" , "3": "XA4OsENuFt33zoQzTF0N"}
client = mqtt.Client()
ser = serial.Serial('/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_55838343433351113122-if00',115200)
while True:
	read=ser.readline()
	print(read)
	read_serial = ser.readline().decode('utf-8')
	print(read_serial)
	json_data1 = read_serial
	with open('json_file.json',"w") as file_write:
        	data1 = json.dump(json_data1,file_write)
	with open('/home/pi/Desktop/z_wave/nRF905/json_file.json') as file_object:
       		data2 = json.load(file_object)
	dict = data2.strip()
	print(dict)
	dict2 = literal_eval(dict)
	print(dict2)
	id = dict2['ID']
	print(id)
	tem = dict2['a']
	print(tem)
	hum = dict2['b']
	print(hum)
	lux = dict2['c']
	print(lux)
	co = dict2['d']
	print(co)
	tvoc = dict2['e']
	print(tvoc)
	ACCESS_TOKEN = token_dict[str(id)]
	client.username_pw_set(ACCESS_TOKEN)
	client.connect(THINGSBOARD_HOST,1883,60)
	client.loop_start()
	try:
		print(u"Temperature: {:g}\u00b0c, Humidity:{:g}% , Lux:{:g}lx, CO2:{:g}ppm, VOC:{:g}ppb ".format(tem, hum, lux, co, tvoc))
		sensor_data['temp']= tem
		sensor_data['hum']= hum
		sensor_data['lux'] = lux
		sensor_data['CO2'] = co
		sensor_data['VOC'] =tvoc
		client.publish("v1/devices/me/telemetry",json.dumps(sensor_data),1)
		print ("Data sent!!!")
	except:
		print ("False to send!!")
