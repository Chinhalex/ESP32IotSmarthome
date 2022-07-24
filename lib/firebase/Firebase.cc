#include "Firebase.h"
#include <ArduinoJson.h>
#include <algorithm>

FireBase::FireBase(char *quserid){
    m_wifi_client = new WiFiClientSecure();
    m_quserid = quserid;
}

bool FireBase::connect(const char *state){
     m_wifi_client->setInsecure();
    
    if(m_wifi_client->connect("smarthomeapp-d2e21-default-rtdb.firebaseio.com",443)){
        Serial.printf("INFO: file: %s, func: %s:%d, msg: Send POST request\n",__FILE__,__func__,__LINE__);
        //sprintf(dhtPath,"/Users/%s/DHT11/",m_quserid);
        m_wifi_client->printf("%s /Users/%s/device.json HTTP/1.1\n",state,m_quserid);
        m_wifi_client->println("host: smarthomeapp-d2e21-default-rtdb.firebaseio.com");
        m_wifi_client->println("Content-Type: application/json");
        m_wifi_client->print("Content-Length: ");
        m_wifi_client->print(json.serializedBufferLength());
        m_wifi_client->print("\n\n");
        return true;
    }
    else
        return false;
    
}


void FireBase::sendData(String path,bool val){
    json.add(path, val);
    if(connect("PATCH")){
        json.toString(*m_wifi_client);
        if(json.readFrom(*m_wifi_client)){
            Serial.println();
            json.toString(Serial, true);
            Serial.printf("INFO: file: %s, func: %s:%d, msg: Completed\n",__FILE__,__func__,__LINE__);
        }else
            Serial.printf("ERROR: file: %s, func: %s:%d, msg: Failed\n",__FILE__,__func__,__LINE__);
        m_wifi_client->stop();
    }
    json.clear();
    
}

FireBase::~FireBase(){
    delete m_wifi_client;
}


std::vector<String> FireBase::getData(){
    
    m_wifi_client->print("0\r\n");
    m_wifi_client->print("\r\n");
    // get the headers and the content length
    int status = -1;
    int content_length = 0;
    if(connect("GET")){
        while (m_wifi_client->connected())
        {
            char buffer[255];
            int read = m_wifi_client->readBytesUntil('\n', buffer, 255);
            if (read > 0)
            {
                buffer[read] = '\0';
                // blank line indicates the end of the headers
                if (buffer[0] == '\r')
                {
                    break;
                }
                if (strncmp("HTTP", buffer, 4) == 0)
                {
                    sscanf(buffer, "HTTP/1.1 %d", &status);
                }
                else if (strncmp("Content-Length:", buffer, 15) == 0)
                {
                    sscanf(buffer, "Content-Length: %d", &content_length);
                }
            }
        }
        Serial.printf("Http status is %d with content length of %d\n", status, content_length);
        if (status == 200)
        {
            StaticJsonDocument<500> filter;
            filter[0]["name"]=true;
            filter[1]["name"]=true;
            filter[2]["name"]=true;
            filter[3]["name"]=true;
            filter[4]["name"]=true;
            filter[5]["name"]=true;
            filter[6]["name"]=true;
            filter[7]["name"]=true;
            StaticJsonDocument<500> doc;
            deserializeJson(doc, *m_wifi_client,DeserializationOption::Filter(filter));
            std::vector <String> device;
            for(int i=0;i<numberOfDevice;i++){
                if(!doc[i]["name"].isNull()){
                    String d = doc[i]["name"];
                    std::transform(d.begin(),d.end(),d.begin(),::tolower);
                    device.push_back(d);
                }else device.push_back("null");
            }
            Serial.printf("Device\n");
            for(const auto &d : device)
                Serial.println(d);
            
            return device;
            
        }

        
    }

    return {};
    
 
}