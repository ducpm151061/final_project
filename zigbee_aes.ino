#include<ModbusMaster.h>
#include <SoftwareSerial.h>
#define MAX485_DE      3
#define MAX485_RE_NEG  2
#include "AESLib.h"
#include <Base64.h>
AESLib aesLib;
ModbusMaster node;

SoftwareSerial mySerial(10, 11); // RX, TX
SoftwareSerial sSerial(7,8);
String buff;
unsigned long time1 = 0;
unsigned long time2 = 0;
uint8_t result;
char data_send[25]="\0";

char cleartext[17];
char ciphertext[34];
const char TEMP_HUMI_COMMAND[] = "01,t=%4d,h=%4d";
byte aes_key[] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
byte aes_iv[N_BLOCK] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
void aes_init() {
  aesLib.gen_iv(aes_iv);
  // workaround for incorrect B64 functionality on first run...
  encryption("HELLO WORLD!", aes_iv);
}
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  aes_init();
  Serial.println("Thiet lap da hoan thanh san sang gui du lieu cho Coordinator");
}
void sensor(){
   sSerial.begin(9600);
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  node.begin(1, sSerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
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
void loop() {
  mySerial.begin(9600);
  buff = mySerial.readStringUntil('#');
    Serial.print("lENH NHAN DUOC TU COORDINATOR : ");
    Serial.println(buff);
  if(buff=="Gui"){
  sensor();
  //node.clearResponseBuffer();
  result= node.readHoldingRegisters(0x40000,6);
  int t=node.getResponseBuffer(3);
  int h=node.getResponseBuffer(2);
  sprintf(cleartext,TEMP_HUMI_COMMAND,t,h);
  //Serial.println(cleartext);  
  //byte cleartext[]={0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61};
  // Encrypt
  byte enc_iv[N_BLOCK] = {0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61}; // iv_block gets written to, provide own fresh copy...
  String encrypted = encryption(cleartext, enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  encrypted.toCharArray(data_send,25);
  mySerial.begin(9600);
//  delay(500);
  mySerial.write("P2P 0000 ");
//  mySerial.write(id_node);
  mySerial.write(data_send);
  }
}
