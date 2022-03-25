

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>



#define CLOCKLENGTH 43200
const char *ssid     = "ATTTq9BPPi";
const char *password = "yrdswvjum4gh";
const bool smooth = true;
#define CLOCK 14
#define BTN 12
#define DEBUG 0
#define STEP 1
#define DEBUGLENGTH 86400 //1 days

#define N1 0 
#define N2 4
#define N3 5
#define N4 16

unsigned long lifeStart= 1148068800;
unsigned long lifeEnd = 3988296000;

unsigned long life;
unsigned long lifeSecond;
unsigned long lastStep;
unsigned long epoch;
unsigned long lastCheck;
const unsigned int stepDiv = 1;



WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");



void setup() {
  
  pinMode(CLOCK, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(115200);
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(0);
  #if DEBUG == 1
    timeClient.update();
    lifeStart = timeClient.getEpochTime();
    lifeEnd = lifeStart + DEBUGLENGTH;
  #endif
  life = lifeEnd - lifeStart;
  Serial.print("Life length:");
  Serial.println(life);
  delay(2000);
  lifeSecond = life/CLOCKLENGTH; //a duration, in seconds

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}


void move(unsigned int length){
  digitalWrite(CLOCK,HIGH);
  delay(1000/length);
  digitalWrite(CLOCK,LOW);
}

void debugJog(){
  for(int i= 0; i<= 15; i++){
    move(1);
    delay(500);
  }
  delay(5000);
}

void lifeClock(unsigned int epoch){
  if(epoch - (lifeSecond/stepDiv) >= lastStep){
    //move second hand 1/stepDiv second
    lastStep = epoch;
    unsigned long soFar = epoch - lifeStart;
    int timeOnClock [3] = {};
    timeOnClock[0] += soFar/(3600*lifeSecond);
    timeOnClock[1] += ((soFar%(3600*lifeSecond))/60/lifeSecond);
    timeOnClock[2] += (soFar%(60*lifeSecond))/lifeSecond;
    Serial.print("Time on Clock:");
    Serial.print(" Hours:");
    Serial.print(timeOnClock[0]);
    Serial.print(" Minutes:");
    Serial.print(timeOnClock[1]);
    Serial.print(" Seconds:");
    Serial.print(timeOnClock[2]);
    move(stepDiv);
  }
}

void checkTime(){
  
  timeClient.update();
  epoch = timeClient.getEpochTime();
  lifeClock(epoch);
}

void jogSeconds(){

  while(!digitalRead(BTN)){
    digitalWrite(CLOCK,HIGH);
  }
  digitalWrite(CLOCK,LOW);

}


int checkSerial(){
    int action = 0;
    if(Serial.available() > 0){
        
        char cmd[Serial.available()];
        Serial.readBytesUntil('\n',cmd,Serial.available()); //Read the serial data and store in var
        char calib[15] = "$Calibrate";
        
        for(unsigned int i=0; i<sizeof(cmd);i++){
          if(cmd[i]==calib[i]){
            action = 1;
          }else{action = 0; break;}
        }


        
    }
  return action;
  Serial.print("Action = ");
  Serial.println(action);
}







void loop() { 
  ArduinoOTA.handle();
  //checkSerial();
  debugJog();
  //unsigned long timePassed = millis()-lastCheck;
  //timePassed = abs(timePassed); 
  /*if(millis()-lastCheck > lifeSecond/200){//wait 2 seconds
    //time to update clock
    lastCheck = millis();
    checkTime();
  }*/
  /*
  if(!digitalRead(BTN)){
    jogSeconds();
    
  }
  */



}