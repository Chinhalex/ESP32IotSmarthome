#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
#include "firebase.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// Insert Firebase project API Key


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
int valueInfrared = 0;
String led_state = "";// LED State
int led_gpio = 0;
bool signupOK = false;


LiquidCrystal_I2C lcd(0x27,16,2);
DHT dht(DHT11_PIN, DHTTYPE);

void connectFirebase()
{
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    /* Assign the api key (required) */
    config.api_key = API_KEY;

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


void push(const float &temp, const float &hum, char *quserid){
    String urlReadTemp = String("Users/") + *quserid + String("/DHT11/nhietdo");
    String urlReadHum  = String("Users/") + *quserid + String("/DHT11/doam");
    if (Firebase.ready() ) 
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

void initLcd()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}



void initDHT() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DHT11_PIN, INPUT);
  dht.begin();

}

void lcdPrint(const float &temp, const float &hum){
    lcd.clear();
    // check whether the reading is successful or not
    if (isnan(temp) || isnan(hum)) {
        Serial.println("failed to lcd");
        lcd.setCursor(0, 0);
        lcd.print("Failed");
    } else {
        Serial.println("success to lcd");
        lcd.setCursor(0, 0);  // display position
        lcd.print("Temp: ");
        lcd.print(temp);    // display the temperature
        lcd.print(" C");
        lcd.setCursor(0, 1);  // display position
        lcd.print("Humi: ");
        lcd.print(hum);      // display the humidity
        lcd.print(" %");
    }
}
void initInfrared(void *param)
{
    while(1){
        Serial.println("Task hong ngoai");
        valueInfrared = analogRead(INFRARED_PIN);
        Serial.println (valueInfrared); //debug
        if(valueInfrared <= 100)
        {
            digitalWrite(BUILTIN_LED, LOW); //led on
            //delay(5000);
        }
        else digitalWrite(BUILTIN_LED, HIGH); //led off - default
        vTaskDelay(100);
    }
  

}
void createTask(){
    xTaskCreate(readTemp,"DHT 11",1024,NULL,1,NULL);
	xTaskCreate(TurnLight,"Turn Light",1024,NULL,2,NULL);
	xTaskCreate(initInfrared,"Hong Ngoai",1024,NULL,3,NULL);
}
void readTemp(void *param)
{
    char *quserid = static_cast<char*>(param);
    float hum;
    float temp;
    while(1){
        Serial.println("task dht11");
        temp = dht.readTemperature();
        hum = dht.readHumidity();
        if (temp > 30 )
        {
        digitalWrite(BUZZER_PIN, HIGH); // turn on
        delay (5000);
        digitalWrite(BUZZER_PIN, LOW);
        }
        push(temp,hum,quserid);
        lcdPrint(temp,hum);
        vTaskDelay(1000*60);
    }
    
    
}
void TurnLight(void *param)
{
    char *quserid = static_cast<char*>(param);
    while(1){
        Serial.println("Task turn light");
        if (Firebase.ready())
        {
            int i;
            for (i = 0; i < 5; i++)
            { 
                led_gpio = i;
                pinMode(led_gpio, OUTPUT);
                String urlDevice = String("Users/") + *quserid + String("/device/") + i + String ("/status");
                led_state = Firebase.RTDB.getString(&fbdo, urlDevice) ? fbdo.to<const char *>() : "false" ;
                if (led_gpio == i && led_state == "true") {
                    Serial.println(String("ESP32-GPIO ") + i + String(" is ON"));
                    digitalWrite(led_gpio, HIGH);
                }
                else if (led_gpio == i && led_state == "false") {
                    Serial.println(String("ESP32-GPIO ") + i + String(" is OFF"));
                    digitalWrite(led_gpio, LOW);
                }
                else {
                    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, urlDevice) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
                }
            }
            vTaskDelay(100);
        }
    }
     
}
