#include <Arduino.h>
#include "EEPROM.h"
#include "WiFi.h"
#include "connectWiFi.h"
#include "Firebase.h"
#include "Infrared.h"

char ssid[32];
char pass[32];
char quserid[32];

void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  delay(1000);
  EEPROM.begin(96);
  
  
  Serial.println("READING EEPROM SSID");
  if(EEPROM.read(0)!=0){
    strcpy(ssid,EEPROM.readString(0).c_str());
    strcpy(pass,EEPROM.readString(32).c_str());
    strcpy(quserid, EEPROM.readString(64).c_str());
  }


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);
  delay(1000);
  if(WiFi.waitForConnectResult()==WL_CONNECTED){
    Serial.println("CONNECTED");
    initLcd();
    initDHT(); //setup DHT temp 
    
    connectFirebase();
   
  }
  else{
    Serial.println("SETUPap");
    setupAP();
  }

  Serial.println();
  Serial.println("Waiting.");
  


  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }  
}

void loop() {

  initInfrared();
  runInfrared();
  readTemp(quserid);
  TurnLight(quserid);
  
}




