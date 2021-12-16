/*****************************************************************
simple-get-nodemcu.ino
Original Creation Date: December 15, 2021

This sketch uses an NodeMCU to send a request to NodeRed server (http://IP_NodeRed:1880) whenever pin 3 is connected to ground.

Hardware Hookup:
  * These components are connected to the NodeMCU I/O pins:
    * D3 - Active-low momentary button (pulled high internally)
    
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
char server[] = " IP_Server_nodeRed:1880"; // Remote host site
char serverIP[] = "IP_Server_nodeRed";     // host IP

//////////////////////
// Input Pins, Misc //
//////////////////////
const int triggerPin = D3;
String name = "NodeMCU-001"; // Thing name
const int port = 1880;

void setup() {
  Serial.begin(115200);

  // Setup Input Pins:
  pinMode(triggerPin, INPUT_PULLUP);  

  // Set up WiFi:
  setupWiFi();

  Serial.println(F("=========== Ready to Stream ==========="));
  Serial.println(F("Press the button (D3) to send an update"));
}

void loop(){
  // If the trigger pin (3) goes low, send the data.
  if (!digitalRead(triggerPin)) {   
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
  Serial.printf("\n[Connecting to %s ... ", server);
  if (client.connect(serverIP, port)) {
    Serial.println(F("Connected"));
    
    // Post the data! Request should look a little something like:
    // GET /hello/
    // Host: 127.0.0.1:1880
    // Connection: close\n
    // \n
    client.print("GET /hello");        
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
