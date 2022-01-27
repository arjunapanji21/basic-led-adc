#include <ESP8266WiFi.h>

// deklarasi pin LED
#define LED1 D0
#define LED2 D1
#define LED3 D2
#define LED4 D3
#define ADC  A0

// configurasi wifi
//const char* ssid = "Andrean Maulana"; // change this ssid to suit your wifi name
//const char* password = "jambi/*_*/"; // change thispassword to suit your wifi password

const char* ssid = "nodemcu"; 
const char* password = "nodemcu123"; 

// open port 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED4, LOW);

  pinMode(ADC, INPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int value1, value2, value3, value4, value5;
  if (req.indexOf(F("/gpio1/0")) != -1) {
    value1 = 0;
  } else if (req.indexOf(F("/gpio1/1")) != -1) {
    value1 = 1;
  } else {
    Serial.println(F("invalid request"));
    value1 = digitalRead(LED1);
  }

  if (req.indexOf(F("/gpio2/0")) != -1) {
    value2 = 0;
  } else if (req.indexOf(F("/gpio2/1")) != -1) {
    value2 = 1;
  } else {
    Serial.println(F("invalid request"));
    value2 = digitalRead(LED2);
  }

  if (req.indexOf(F("/gpio3/0")) != -1) {
    value3 = 0;
  } else if (req.indexOf(F("/gpio3/1")) != -1) {
    value3 = 1;
  } else {
    Serial.println(F("invalid request"));
    value3 = digitalRead(LED3);
  }

  if (req.indexOf(F("/gpio4/0")) != -1) {
    value4 = 0;
  } else if (req.indexOf(F("/gpio4/1")) != -1) {
    value4 = 1;
  } else {
    Serial.println(F("invalid request"));
    value4 = digitalRead(LED4);
  }

  value5 = analogRead(ADC);
  Serial.println(value5);
  delay(100);

  // Set LED according to the request
  digitalWrite(LED1, value1);
  digitalWrite(LED2, value2);
  digitalWrite(LED3, value3);
  digitalWrite(LED4, value4);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><meta http-equiv='refresh' content='5'>\r\n"));
  client.print(F("<center><h1>"));

  client.print(F("<br>[IOT] LED ADC"));
  client.print(F("<br>Andrean Maulana / 8030170033</h1>"));

  client.print(F("<h2><br>LED 1 "));
  client.print((value1) ? F("ACTIVE") : F("INACTIVE"));
  client.print(F("<br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio1/1'>ON</a> | <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio1/0'>OFF</a>"));

  client.print(F("<br><br>LED 2 "));
  client.print((value2) ? F("ACTIVE") : F("INACTIVE"));
  client.print(F("<br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio2/1'>ON</a> | <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio2/0'>OFF</a>"));

  client.print(F("<br><br>LED 3 "));
  client.print((value3) ? F("ACTIVE") : F("INACTIVE"));
  client.print(F("<br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio3/1'>ON</a> | <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio3/0'>OFF</a>"));

  client.print(F("<br><br>LED 4 "));
  client.print((value4) ? F("ACTIVE") : F("INACTIVE"));
  client.print(F("<br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio4/1'>ON</a> | <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio4/0'>OFF</a>"));

  client.print(F("<br><br>Nilai ADC Potentiometer "));
  client.print(F("<br>"));
  client.print(value5);

  client.print(F("</h2></center></html>"));

  // The client will actually be flushed then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
