#include<ModbusMaster.h>
#include <SoftwareSerial.h>
#define MAX485_DE      3
#define MAX485_RE_NEG  2
#include <AESLib.h>
#include <Base64.h>
ModbusMaster node;
SoftwareSerial mySerial(10, 11); // RX, TX
SoftwareSerial sSerial(7,8);
String buff;
unsigned long time1 = 0;
unsigned long time2 = 0;
uint8_t result;
//char data_send[25]="\0";
char data[17];
const char TEMP_HUMI_COMMAND[] = "01,t=%4d,h=%4d";

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
//  sprintf(ciphertext, "%s", encrypted.c_str());
//  encrypted.toCharArray(data_send,25);
  mySerial.begin(9600);
//  delay(500);
  mySerial.write("P2P 0000 ");
//  mySerial.write(id_node);
  mySerial.write(encoded);
  }
}
