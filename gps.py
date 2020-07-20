import serial
import pynmea2
import paho.mqtt.client as mqtt
import json
import time
sensor_data = {'latitude': 0, 'longitude': 0}
THINGSBOARD_HOST = '14.162.38.93'
ACCESS_TOKEN = 'gps01'


def parseGPS(str):
    if str.find('GNGGA') > 0:
        msg = pynmea2.parse(str)

        print("Timestamp: %s -- Lat: %s %s -- Lon: %s %s -- Altitude: %s %s" % (msg.timestamp,
                                                                                msg.lat, msg.lat_dir, msg.lon, msg.lon_dir, msg.altitude, msg.altitude_units))


def split(string):
    # raw = string.split('.')
    # natural = raw[0]
    # decimal = raw[1]
    # result = str(int(natural)/100)+decimal
    # return result
    natural=string[:-8]
    decimal=float(string[-8:])/60
    result=float(natural)+decimal
    return result

serialPort = serial.Serial(port='/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0',
                           baudrate=9600,
                           parity=serial.PARITY_NONE,
                           stopbits=serial.STOPBITS_ONE,
                           bytesize=serial.EIGHTBITS,
                           timeout=0.5)


try:
    while True:
        try:
            rawdata = (str)(serialPort.readline().decode())
            print(rawdata)
            if rawdata.find('GNGGA') > 0 and len(rawdata) > 45:
                msg = pynmea2.parse(rawdata)
                sensor_data['latitude'] = split(msg.lat)
                sensor_data['longitude'] = split(msg.lon)
                print(split(msg.lat))
                print(split(msg.lon))
                print("----------------------------")
                print("Timestamp: %s -- Lat: %s %s -- Lon: %s %s -- Altitude: %s %s" % (msg.timestamp,
                                                                                        msg.lat, msg.lat_dir, msg.lon, msg.lon_dir, msg.altitude, msg.altitude_units))
                client = mqtt.Client()
                client.username_pw_set(ACCESS_TOKEN)
                try:
                    client.connect(THINGSBOARD_HOST, 1883)
                    client.publish('v1/devices/me/telemetry',
                                   json.dumps(sensor_data))
                    #client.disconnect()
                except (TimeoutError, OSError):
                    #client.disconnect()
                    continue
                time.sleep(1)
        except UnicodeDecodeError:
            continue

except KeyboardInterrupt:
    pass

