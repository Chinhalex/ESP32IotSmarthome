#include "Fire.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "FireBase.h"



//Define Firebase Data object
/* FirebaseData fbdo;
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config; */

String led_state = "";// LED State
bool signupOK = false;


LiquidCrystal_I2C lcd(0x27,16,2); 
DHT dht(DHT11_PIN, DHTTYPE); 

void initLcd()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

/* void connectFirebase()
{
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
   
    config.api_key = API_KEY;

    config.database_url = DATABASE_URL;
  
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
    }
    else 
    {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}
 */
void initDHT() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DHT11_PIN, INPUT);
  dht.begin();

}

void setTemp(const char *quserid){
    
    float temp = dht.readTemperature();
	float hum = dht.readHumidity();
    if (temp > 30 )
    {
		digitalWrite(BUZZER_PIN, HIGH); // turn on
		delay (5000);
		digitalWrite(BUZZER_PIN, LOW);
    }
}
/* void readTemp(const char* quserid)
{
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (temp > 30 )
    {
      digitalWrite(BUZZER_PIN, HIGH); // turn on
      delay (5000);
      digitalWrite(BUZZER_PIN, LOW);
    }
    String urlReadTemp = String("Users/") + quserid + String("/DHT11/nhietdo");
    String urlReadHum  = String("Users/") + quserid + String("/DHT11/doam");
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
} */
void TurnLight(void *param)
{
  char *quserid = static_cast <char *>(param);
  while(1){
    
        for (int i = 0; i < 5; i++)
        { 
          String urlDevice = String("Users/") + quserid + String("/device/") + i + String ("/status/");
          led_state = Firebase.RTDB.getString(&fbdo, urlDevice) ? fbdo.to<const char *>() : "false" ;
          
          Serial.printf("GPIO %d is %s\n",i, led_state );
          digitalWrite(i, i == 2 ? !(led_state == "true" ? true: false):(led_state == "true" ? true: false));
          /* }
          else if (  led_state == "false") {
          Serial.println(String("ESP32-GPIO ") + i + String(" is OFF"));
          digitalWrite(i, i == 2 ? HIGH : LOW);
          }
          else {
              Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, urlDevice) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
          } */
          
        }
        vTaskDelay(100);
      
    
  }
    
}