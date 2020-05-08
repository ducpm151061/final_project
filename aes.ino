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
#define mqtt_server "192.168.0.103"
#define humidity_topic "hum"
#define temperature_topic "temp"
#define wifi_ssid "wifi"
#define wifi_password "bothoi123"
#define sleepTime 1
#define DHTTYPE DHT11 
AESLib aesLib;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial soft(2, 3);
WiFiEspClient espClient;
PubSubClient client(espClient);
int h;
int t;
int f;
char cleartext[17];
char ciphertext[34];
const char TEMP_HUMI_COMMAND[] = "ID=1, t=%2d, h=%2d";
byte aes_key[] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
byte aes_iv[N_BLOCK] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
void aes_init() {
  aesLib.gen_iv(aes_iv);
  // workaround for incorrect B64 functionality on first run...
  encryption("HELLO WORLD!", aes_iv);
}
String encryption(char * msg, byte iv[]) {  
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen];
  aesLib.encrypt(msg, msgLen, encrypted, aes_key, sizeof(aes_key), iv);
  return String(encrypted).substring(0,24);
}
String decryption(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt(msg, msgLen, decrypted, aes_key, sizeof(aes_key), iv);
  
  return String(decrypted);
}
void setup_wifi() {
  soft.begin(9600);
  aes_init();
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
      delay(5000);
    }
  }
}
 
void publishTempHumid() {
  sprintf(cleartext,TEMP_HUMI_COMMAND,t,h);
  Serial.println(cleartext);  
  //byte cleartext[]={0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
  // Encrypt
  byte enc_iv[N_BLOCK] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61}; // iv_block gets written to, provide own fresh copy...
  String encrypted = encryption(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  Serial.print("Ciphertext: ");
  Serial.println(encrypted);
  client.publish(temperature_topic, encrypted.c_str(), true);
//    client.publish(humidity_topic, String(h).c_str(), true);
// Decrypt
  byte dec_iv[N_BLOCK] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61}; // iv_block gets written to, provide own fresh copy...
  String decrypted = decryption(ciphertext, dec_iv);  
  Serial.print("Cleartext: ");
  Serial.println(decrypted);  
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
