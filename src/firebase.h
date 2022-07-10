#ifndef _firebase_h_
#define _firebase_h_

#define API_KEY "AIzaSyBP_iz79fpA7izAG4Kq1n7PUPM_zSHJCzQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smarthomeapp-d2e21-default-rtdb.firebaseio.com/" 

#define DHT11_PIN 15
#define DHTTYPE DHT11
#define BUZZER_PIN 18
#define INFRARED_PIN 34


void connectFirebase();
void push(const float &temp, const float &hum, char *quserid);
void initLcd();

void initDHT();

void lcdPrint(const float &temp, const float &hum);
void initInfrared(void *param);
void readTemp(void *param);
void TurnLight(void *param);
void createTask();
#endif