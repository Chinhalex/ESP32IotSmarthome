
#ifndef _Firebase_h_
#define _Firebase_h_

#define DHT11_PIN 15
#define DHTTYPE DHT11
#define BUZZER_PIN 18


#define API_KEY "AIzaSyBP_iz79fpA7izAG4Kq1n7PUPM_zSHJCzQ"

#define DATABASE_URL "https://smarthomeapp-d2e21-default-rtdb.firebaseio.com/" 


void initLcd();
void connectFirebase();
void initDHT();
void readTemp(const char* quserid);
void TurnLight(void * param);

#endif 