#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define redpin D7
#define greenpin D6
const int totalVal = 5;
int values[totalVal];
int indexVal = 0;
int average = 0;
int total = 0;
char* arduinoSend = "";
int arduinoGet = 0;
const char* IP = "1192.168.43.233";
const int PORT = 3000;

const char* ssid = "Neha Sadhvi";
const char* password = "Casino007@@";

//const char* ssid = "UCInet Mobile Access";
//const char* password = "";

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

//const char* ssid = "CAEA6D";
//const char* password = "72ULAEG353691";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);

  for (int i = 0; i < totalVal; i++) {
    values[i] = 0;
  }

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void loop()
{
  total = total - values[indexVal];
  uint16_t red, green, yellow, clear;
  tcs.getRawData(&red, &green, &yellow, &clear);
  values[indexVal] = clear;
  
  total = total + values[indexVal]; 
  average = total / totalVal;

  indexVal = indexVal + 1;
  if (indexVal = totalVal) {
    indexVal = 0;
  }

  // send back a reply, to the IP address and port we got the packet from
  // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  sprintf(arduinoSend, "%d", average);
    
  Serial.printf("ESP: %s\n", arduinoSend);
  Udp.beginPacket("192.168.43.233", 3000);
  Udp.write(arduinoSend);
  Udp.endPacket();

  delay(500);
  
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
//    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, IP, PORT);
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
//    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    
    sscanf(incomingPacket,"%d",&arduinoGet);
    Serial.printf("Raspberry Pi: %d ", arduinoGet);

//    while(arduinoGet === 0) {
//        digitalWrite(redpin, HIGH);
//    }
    
    if (arduinoGet < average) {
      digitalWrite(redpin, HIGH);
      digitalWrite(greenpin, LOW);
    } else {
      digitalWrite(redpin, LOW);
      digitalWrite(greenpin, HIGH);
    }
  }
  else {
    digitalWrite(redpin, HIGH);
    digitalWrite(greenpin, LOW);
    delay(100);
    digitalWrite(redpin, LOW);
    digitalWrite(greenpin, LOW);
  }
}
