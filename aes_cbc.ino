#include <AESLib.h>
#include "DHT.h"
#include <PubSubClient.h>
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include "AESLib.h"
#include <Base64.h>
 int status = WL_IDLE_STATUS;
#define DHTPIN 5
#define mqtt_server "192.168.1.117"
#define humidity_topic "hum"
#define temperature_topic "temp"
#define wifi_ssid "Minh Cuong"
#define wifi_password "huynhcuong"
#define sleepTime 1
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial soft(2, 3);
WiFiEspClient espClient;
PubSubClient client(espClient);
int h;
int t;
int f;
char data[17];
const char TEMP_HUMI_COMMAND[] = "ID=1, t=%2d, h=%2d";
void setup_wifi() {
  soft.begin(9600);
  WiFi.init(&soft);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  Serial.println("Connecting to AP ...");
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(wifi_ssid);
    status = WiFi.begin(wifi_ssid, wifi_password);
    delay(500);
  }
  Serial.println("Connected to AP");
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}
 
void publishTempHumid() {
  uint8_t key[] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
  uint8_t iv[] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
  //char data[] = "0123456789012345"; //16 chars == 16 bytes
  sprintf(data,TEMP_HUMI_COMMAND,t,h);
  Serial.println(data);
  uint16_t data_len=sizeof(data)-1;
  unsigned long time1= micros();
  aes128_cbc_enc(key,iv, data,data_len);
  unsigned long time2= micros();
  unsigned long time_result=time2-time1;
  Serial.println(time_result);
  Serial.print("encrypted:");
  int inputLen = sizeof(data);
  int encodedLen = base64_enc_len(inputLen);
  char encoded[encodedLen];
  Serial.println(data); 
  base64_encode(encoded, data, inputLen); 
  Serial.println(encoded);
  client.publish(temperature_topic, encoded, true);
  
//  aes128_dec_single(key, data);
//  Serial.print("decrypted:");
//  Serial.println(data);
  
//    client.publish(humidity_topic, String(h).c_str(), true);
}
 
void updateTempHumid() {
  h = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(h);
  t = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(t);
  f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed DHT");
    return;
  }
}
 
void setup(){
 
  Serial.begin(9600);
  while(!Serial) {;}
  setup_wifi();
  client.setServer(mqtt_server, 1883);
 
  dht.begin(); // initialize dht
  
}
 
void loop(){
 
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  updateTempHumid();
  publishTempHumid();
  client.disconnect();
  delay(sleepTime * 1000);
}
