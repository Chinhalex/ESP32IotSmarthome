#include <Arduino.h>
#include "WiFi.h"
#include "EEPROM.h" 
#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHT11_PIN 15
#define DHTTYPE DHT11

DHT dht(DHT11_PIN, DHTTYPE);

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Insert Firebase project API Key
#define API_KEY "AIzaSyBP_iz79fpA7izAG4Kq1n7PUPM_zSHJCzQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthomeapp-d2e21-default-rtdb.firebaseio.com/" 

// /* 4. Define the user Email and password that alreadey registerd or added in your project */
// #define USER_EMAIL "vovchinh@gmail.com"
// #define USER_PASSWORD "123456789"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
String led_state = "";// LED State
int led_gpio = 0;
bool signupOK = false;
void connectFirebase()
{
    
    
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    /* Assign the api key (required) */
    config.api_key = API_KEY;

    // /* Assign the user sign in credentials */
    // auth.user.email = USER_EMAIL;
    // auth.user.password = USER_PASSWORD;

    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;
    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
    }
    else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);

    //Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.reconnectWiFi(true);
    //  if (Firebase.ready())
    //      Firebase.RTDB.setString(&fbdo, F("/LivingRoom/Alarm"), F("ON")) ? "ok" : fbdo.errorReason().c_str();

}

void initDHT() {
  pinMode(DHT11_PIN, INPUT);
  dht.begin();

}

void readTemp(String quserid)
{
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    String urlReadTemp = String("Users/") + quserid + String("/DHT11/nhietdo");
    String urlReadHum  = String("Users/") + quserid + String("/DHT11/doam");
    if (Firebase.ready() ) {
    {
        if (Firebase.RTDB.setFloat(&fbdo, urlReadTemp , temp)){
          Serial.print("nhiet do: ");
          Serial.println(temp);
        } 
        if (Firebase.RTDB.setFloat(&fbdo, urlReadHum, hum)){
          Serial.print("do am: ");
          Serial.println(hum);
        }
        Serial.println();
      delay(1000);
    }
  }
}
void TurnLight(String quserid)
{
     if (Firebase.ready() && (millis() - sendDataPrevMillis > 1500 || sendDataPrevMillis == 0))
      {
        sendDataPrevMillis = millis();
        int i;
        for (i = 0; i < 3; i++)
        { 
          led_gpio = i;
          pinMode(led_gpio, OUTPUT);

          String urlDevice = String("Users/") + quserid + String("/device/") + i + String ("/status");
        
          Firebase.RTDB.getString(&fbdo,urlDevice);
          led_state = fbdo.to<const char *>();
          
          if (led_state == "true") {
          Serial.println(String("ESP32-GPIO ") + i + String(" is ON"));
          digitalWrite(led_gpio, HIGH);
          }
          else if (led_state == "false") {
          Serial.println(String("ESP32-GPIO ") + i + String(" is OFF"));
          digitalWrite(led_gpio, LOW);
          }
      
          else {
              Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, F("/led")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
          }
      }
      
    }
}