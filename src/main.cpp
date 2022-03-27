#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <functions.h>

#define CS 43200.00
#define DEBUG 1
#define DIFF 157680 //debug value will be halfway between this*2
//unsigned long lifeStart= ***REMOVED***;
//unsigned long lifeEnd = ***REMOVED***;
unsigned long lifeStart= ***REMOVED***;
unsigned long lifeEnd = ***REMOVED***;
unsigned long oldPulse = 0;
unsigned long epoch = 0;
unsigned long timeLeft = 0;
unsigned long lastCheck = 0;
unsigned long totalPulses = 0;

double PT = 1000; //1 pulse per 1000 milliseconds
bool pinVal = 0;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 10000);







void setup(){
    pinMode(5,OUTPUT);
    const char *ssid     = "***REMOVED***";
    const char *password = "***REMOVED***";


    Serial.begin(115200);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    timeClient.begin();
    //timeClient.setTimeOffset(0);
    delay(100);
    timeClient.update();
    
    epoch = timeClient.getEpochTime();
    #if DEBUG == 1
    lifeStart = epoch - DIFF;
    lifeEnd = epoch + DIFF;
    #endif
    PT = pulseTime(epoch, lifeStart, lifeEnd);
    Serial.println(PT);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
    Serial.print("Hours: ");
    Serial.println(time[0]);
    Serial.print("Minutes: ");
    Serial.println(time[1]);
    Serial.print("Seconds: ");
    Serial.println(time[2]);
}



void loop(){


if(micros()-oldPulse > (.5*PT)){
    oldPulse = micros();
    
    pinVal = !pinVal;
    digitalWrite(5, pinVal);
    
}

if(millis()-lastCheck >= 10000){ //update PT every 10 seconds
    lastCheck = millis();
    timeClient.update();
    epoch = timeClient.getEpochTime();
    PT = pulseTime(epoch, lifeStart, lifeEnd);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
    /*Serial.print("Hours: ");
    Serial.println(time[0]);
    Serial.print("Minutes: ");
    Serial.println(time[1]);
    Serial.print("Seconds: ");*/
    Serial.println(time[2]);
}


}




