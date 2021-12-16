/*****************************************************************
thing-client.ino
Original Creation Date: December 15, 2021

This sketch uses an NodeMCU to send a request to NodeRed server (http://IP_NodeRed:1880) whenever pin 3 is connected to ground.

Hardware Hookup:
  * These components are connected to the NodeMCU I/O pins:
    * D3 - Active-low momentary button (pulled high internally)
    * A0 - Potentiometer 10k.
    * D5 - SPST switch to select either 3.3V or 0V to this pin.
    
Development environment specifics:
    IDE: Arduino 1.0.5
    Hardware Platform: NodeMCU

Original code: https://learn.sparkfun.com/tutorials/pushing-data-to-datasparkfuncom
*****************************************************************/

// Include the ESP8266WiFi library

#include <ESP8266WiFi.h>


////////////////////
// WiFi Constants //
////////////////////
char ssid[] = "SSID"; // SSID of the WiFi network
char passphrase[] = "SSID_PASSPHRASE"; // Password (if applicable)



////////////////////
// NodeRED config //
////////////////////
char server[] = "IP_nodeRed_server:1880"; // Remote host site
char serverIP[] = "IP_nodeRed_server"; // Remote IP host 

const byte NUM_FIELDS = 3;
const String fieldNames[NUM_FIELDS] = {"sensor", "switch", "name"};
String fieldData[NUM_FIELDS];

//////////////////////
// Input Pins, Misc //
//////////////////////
const int triggerPin = D3;
const int sensorPin = A0;
const int switchPin = D5;
String name = "NodeMCU-001"; // Thing name
const int port = 1880;

void setup() {
  Serial.begin(115200);

  // Setup Input Pins:
  pinMode(triggerPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(sensorPin, INPUT_PULLUP);

  // Set up WiFi:
  setupWiFi();

  Serial.println(F("=========== Ready to Stream ==========="));
  Serial.println(F("Press the button (D3) to send an update"));
}

void loop(){
  // If the trigger pin (3) goes low, send the data.
  if (!digitalRead(triggerPin)) {
    // Gather data:
    fieldData[0] = String(analogRead(sensorPin));
    fieldData[1] = String(digitalRead(switchPin));
    fieldData[2] = name;

    // Post data:
    Serial.println(F("Posting!"));
    sendData(); // the sendData() function does all the work, 
                // check it out below.
    delay(1000);
  }
}

void setupWiFi() {
  Serial.println("Initializing NodeMCU");
  Serial.println();
  Serial.print(F("Connecting to "));
  WiFi.begin(ssid, passphrase);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP: ");
  Serial.print(WiFi.localIP()); // Despliegue de la IP
  Serial.println(" Conexion establecida (OK)");  
}

void sendData() {
  WiFiClient client;
  if (client.connect(serverIP, port)) {
    Serial.println(F("Connected"));
    // Post the data! Request should look a little something like:
    // GET /input/?sensor=1024&switch=0&name=NodeMCU-001 HTTP/1.1\n
    // Host: 127.0.0.1:1880
    // Connection: close\n
    // \n
    client.print("GET /input/?");    
    for (int i=0; i<NUM_FIELDS; i++) {      
      client.print(fieldNames[i]);
      client.print("=");
      client.print(fieldData[i]);
      if(i < NUM_FIELDS - 1) {
        client.print("&");
      }      
    }
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
      
    // While we're connected, print out anything the server sends:
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.print(c);
      }
    }
    Serial.println();      
  }
  else { // If the connection failed, print a message:
    Serial.println(F("Connection failed"));
  }

  // If we're disconnected, stop the client:
  if (!client.connected()) 
  {
    Serial.println(F("Disconnecting."));
    client.stop();
  }
}
