// Include the necessary library
#include <ESP8266WiFi.h>

// Defining maximum number of clients to connect server
#define MAX_SRV_CLIENTS 3

// SSID and Password of a modem that is used should be defined.
const char* ssid = "";             // SSID  you may change it according to used modem.
const char* password = "";   // Password you may change it according to used modem.

// Defining server with specific data transfer port.
WiFiServer server(5000);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  // Trying to connect to WiFi for every 0.5 seconds.
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    // Canceling for connection.
    while(1) delay(500);
  }

  // If the WiFi is connected, server is began here.
  server.begin();
  server.setNoDelay(true);
  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
  uint8_t i;

  // CHECKING IF SERVER HAS ANY CLIENTS
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  
  // BOUNCING ALL MESSAGES RECIEVED FROM CLIENTS OVER WIFI TO ARDUINO OVER SERIAL COMMUNICATION
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
        //you can reply to the client here
      }
    }
  }

  // BOUNCING ALL MESSAGES RECIEVED FROM ARDUINO OVER SERIAL COMMUNICATION TO CLIENTS OVER WIFI
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //bello is a broadcast to all clients
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }
}
