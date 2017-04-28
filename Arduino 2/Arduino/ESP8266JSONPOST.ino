#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Time.h>
//#include <ArduinoJson.h>

WiFiClient client;
struct tm * ptm;

//StaticJsonBuffer<200> jsonBuffer;

const char* host="10.1.1.9";
//String postData = "username=gateway&iot=AWS&isDone=false&hasAttachment=false";
//String postData = "{\"username\":\"gateway\",\"timestamp\":\"123456 12:34:56\",\"iot\":\"IOT\",\"isDone\":\"false\",\"hasAttachment\":\"false\"}";
//unsigned int localPort = 2390;      // local port to listen for UDP packets
unsigned int localPort = 3000;

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
int lastConnectionTime = 0;
int failedCounter = 0;

WiFiUDP udp;
bool flag = false;



const int updateThingSpeakInterval = 30 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
char datetime[20];

void updateIOTCloud(String iotData) {
  
    if (client.connect(host, localPort)) {    
           
        client.print("POST /api/iot HTTP/1.1\n");
        client.print("Host: 10.1.1.9\n");
        client.print("Connection: close\n");
        //client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Type: application/json\n");
        client.print("Content-Length: ");
        client.print(iotData.length());
        client.print("\n\n");

        client.print(iotData);
        lastConnectionTime = millis();

        if (client.connected()) {
            Serial.println("Connecting to ThingSpeak...");
            Serial.println();
            failedCounter = 0;
        } else {
            failedCounter++;
            Serial.println("Connection to IOT cloud failed ("+String(failedCounter, DEC)+")");   
            Serial.println();
        }
    } else {
        failedCounter++;
        Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
        Serial.println();
        lastConnectionTime = millis(); 
    }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
  
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

void getNTP() {
    //get a random server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP); 
  
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
    
    int cb = udp.parsePacket();
    if (!cb) {
        Serial.println("no packet yet");
    } else {
        Serial.print("packet received, length=");
        Serial.println(cb);
        // We've received a packet, read the data from it
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  
        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:
    
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        //Serial.print("Seconds since Jan 1 1900 = " );
        //Serial.println(secsSince1900);
    
        // now convert NTP time into everyday time:
        //Serial.print("Unix time = ");
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        unsigned long epoch = secsSince1900 - seventyYears;
        // print Unix time:
        //Serial.println(epoch);
        //ptm = gmtime ( &epoch );

        setTime(epoch);
        sprintf(datetime, "%02d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());  
        Serial.println(datetime);
        
    }
    // wait ten seconds before asking for the time again
    //delay(10000);
}

void setup() {
  
    Serial.begin(115200);
    //digitalWrite(2, LOW);
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("IOTGatewayAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    udp.begin(localPort);
    
}


void loop() {
  /*

    if (syncEventTriggered) {
        processSyncEvent(ntpEvent);
        syncEventTriggered = false;
    }*/
    
    
    if ( millis()-lastConnectionTime > updateThingSpeakInterval ) {
  
        if (flag == false) { 
            getNTP(); 
            //flag = true;
        }

        /*
        String postData;
        JsonObject& root = jsonBuffer.createObject();
        root["username"] = "gateway";
        root["timestamp"] = datetime;
        root["iot"] = "IOT";
        root["isDone"] = "false";
        root["hasAttachment"] = "fasle";
        root.printTo(postData);
        */
        sprintf(datetime, "%02d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());  

        String postData = "{";
        postData += "\"username\":\"gateway\",";
        postData += "\"timestamp\":\"";
        postData += datetime;
        postData += "\",";
        postData += "\"iot\":\"IOT\",";
        postData += "\"isDone\":\"false\",";
        postData += "\"hasAttachment\":\"false\"";
        postData += "}";
        Serial.println(postData);
        
        updateIOTCloud(postData);  
        //Serial.print(NTP.getTimeDateString());
    }
}


