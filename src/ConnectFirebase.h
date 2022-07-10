#include <Firebase_ESP_Client.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


LiquidCrystal_I2C lcd(0x27,16,2);

#define DHT11_PIN 15
#define DHTTYPE DHT11
#define BUZZER_PIN 18
#define INFRARED_PIN 34

DHT dht(DHT11_PIN, DHTTYPE);

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Insert Firebase project API Key
#define API_KEY "AIzaSyBP_iz79fpA7izAG4Kq1n7PUPM_zSHJCzQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthomeapp-d2e21-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int valueInfrared = 0;
String led_state = "";// LED State
int led_gpio = 0;
bool signupOK = false;

void initLcd()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}

void initInfrared()
{
  valueInfrared = analogRead(INFRARED_PIN);
  Serial.println (valueInfrared); //debug
  if(valueInfrared <= 100)
  {
    digitalWrite (BUILTIN_LED, LOW); //led off
    //delay(5000);
  }
  digitalWrite (BUILTIN_LED, HIGH); //led on - default
  
}
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

void initDHT() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DHT11_PIN, INPUT);
  dht.begin();

}
void readTemp(String quserid)
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
}
void TurnLight(String quserid)
{
     if (Firebase.ready())
      {
        int i;
        for (i = 0; i < 5; i++)
        { 
          led_gpio = i;
          pinMode(led_gpio, OUTPUT);
          String urlDevice = String("Users/") + quserid + String("/device/") + i + String ("/status");
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
      
    }
}