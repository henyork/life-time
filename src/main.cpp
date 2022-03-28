#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <EEPROM.h>
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
unsigned long lastActivity = 0;
unsigned long lastCheck = 0;
unsigned long pulsesLeft = 0;
unsigned long recalibTimer = 0;
int menuLvl = 0;
double PT = 1000; //1 pulse per 1000 milliseconds
bool pinVal = 0;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 10000);







void setup(){
    pinMode(4,OUTPUT);
    digitalWrite(4, HIGH);
    const char *ssid     = "***REMOVED***";
    const char *password = "***REMOVED***";

    EEPROM.begin(8);
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
    EEPROM.get(0, lifeStart);
    EEPROM.get(4, lifeEnd);
    
    Serial.print("TF start:");
    Serial.println(lifeStart);
    Serial.print("TF end:");
    Serial.println(lifeEnd);
    pulsesLeft = initialPulsesLeft(epoch, lifeStart, lifeEnd);
    PT = pulseTime(lifeEnd-epoch,pulsesLeft);
    Serial.println(PT);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
    Serial.print("Hours: ");
    Serial.println(time[0]);
    Serial.print("Minutes: ");
    Serial.println(time[1]);
    Serial.print("Seconds: ");
    Serial.println(time[2]);
    Serial.print("Press any key to enter menus... \n");
}



void loop(){


if((micros()-oldPulse > (.5*PT)) && epoch > lifeEnd){
    oldPulse = micros();
    if(pinVal){pulsesLeft--;}
    pinVal = !pinVal;
    digitalWrite(4, pinVal);
    
}

if((millis()-lastCheck >= 10000)&& epoch > lifeEnd){ //update PT every 10 seconds
    lastCheck = millis();
    timeClient.update();
    epoch = timeClient.getEpochTime();
    PT = pulseTime(lifeEnd - epoch, pulsesLeft);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
}

if(Serial.available()>0){
    lastActivity = millis();
    switch(menuLvl){
        case 0:
        {
            char cmd[Serial.available()];
            Serial.readBytes(cmd, Serial.available());
            int time[3];
            clockTime(epoch, lifeStart, lifeEnd, time);
            Serial.print("Clock should read ");
            printClockTime(time);
            Serial.println("Press 1 to recalibrate or 2 to change timeframe");
            menuLvl = 1;
            break;
        }
        case 1:
        {
            int cmd;
            cmd = Serial.parseInt();
            if(cmd == 1){
                menuLvl = 2;
            }
            if(cmd == 2)
                menuLvl = 3;
            break;
        }
        case 2:
        {
            char cmd[Serial.available()];
            Serial.readBytes(cmd, Serial.available());
            int time[3];
            clockTime(epoch+15, lifeStart, lifeEnd, time);
            Serial.print("You have 15 seconds to move hands to ");
            printClockTime(time);
            recalibTimer = millis();
            while(millis()-recalibTimer < 15000){ //block other processes for 15 seconds
                Serial.println(15-((millis()-recalibTimer)/1000));
                delay(1000);
            }
            Serial.print("Clock should now read ");
            printClockTime(time);
            menuLvl = 0;
            Serial.print("Press any key to enter menus... \n");
            break;
        }
        case 3:
        {
            Serial.println("Enter start and end time as epoch, seperated by a comma:");
            while(Serial.available() < 4){
                delay(100);
            }
            unsigned long start;
            unsigned long end;
            start = Serial.parseInt();
            end = Serial.parseInt();
            while(Serial.available()>0){
                Serial.read();
            }
            Serial.print("Values Recieved: Start =");
            Serial.print(start);
            Serial.print(", End=");
            Serial.println(end);
            Serial.println("Enter y to accept, n to cancel");
            while(Serial.available()<1){
                delay(100);
            }
            int cmd = Serial.read();
            if(cmd == 'y' || cmd == 'Y'){ //reseting time values for new timeframe
                
                lifeStart = start;
                lifeEnd = end;
                EEPROM.put(0, lifeStart);
                EEPROM.put(4, lifeEnd);
                EEPROM.commit();
                timeClient.update();
                epoch = timeClient.getEpochTime();
                pulsesLeft = initialPulsesLeft(epoch, lifeStart, lifeEnd);
                PT = pulseTime(lifeEnd-epoch,pulsesLeft);
                Serial.println("New timeframe set");
            }else if(cmd == 'n' || cmd == 'N'){
                Serial.println("Canceled");
            }else{
                Serial.println("Canceled: Unknown command");
            }
            menuLvl = 0;
            Serial.print("Press any key to enter menus... \n");
            break;
        }
    }
    while(Serial.available()>0){
        Serial.read();
    }
    }
}





