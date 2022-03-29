#include <extern.h>
#include <functions.h>
#include <Adafruit_NeoPixel.h>
#include <secrets.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 10000);
Adafruit_NeoPixel pixels(52, 12, NEO_GRB + NEO_KHZ800);


#define DEBUG 0
//#define DIFF 157680 //debug value will be halfway between this*2
#define DIFF 43200/2
#define STEPPIN1 4
#define STEPPIN2 5


unsigned long lifeStart= ***REMOVED***;
unsigned long lifeEnd = ***REMOVED***;
unsigned long lastStep = 0;
unsigned long epoch = 0;
unsigned long lastActivity = 0;
unsigned long lastCheck = 0;
unsigned long stepsLeft = 0;
unsigned long recalibTimer = 0;
int menuLvl = 0;
double ST = 125; //1 step per 125 milliseconds
bool pinVal = 0;


//Define your WiFi credentials here
//const char *ssid     = "abcdefgh";
//const char *password = "12354678";








void setup(){
    pinMode(STEPPIN1,OUTPUT);
    pinMode(STEPPIN2,OUTPUT);
    
    

    EEPROM.begin(8);
    pixels.begin();
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
    #if DEBUG == 0
    EEPROM.get(0, lifeStart);
    EEPROM.get(4, lifeEnd);
    #endif
    #if DEBUG == 1
    lifeStart = epoch - DIFF;
    lifeEnd = epoch + DIFF;
    #endif
    Serial.print("TF start:");
    Serial.println(lifeStart);
    Serial.print("TF end:");
    Serial.println(lifeEnd);
    stepsLeft = initialStepsLeft(epoch, lifeStart, lifeEnd);
    ST = stepTime(lifeEnd-epoch,stepsLeft);
    Serial.println(ST);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
    Serial.print("Hours: ");
    Serial.println(time[0]);
    Serial.print("Minutes: ");
    Serial.println(time[1]);
    Serial.print("Seconds: ");
    Serial.println(time[2]);
    Serial.print("Press any key to enter menus... \n");

    for(int i=0; i<52; i++){
        pixels.setPixelColor(i,pixels.Color(30,30,20));
    }
    pixels.show();
}



void loop(){

if((millis()-lastStep > ST) && epoch < lifeEnd){
    lastStep = millis();
    if(millis()-lastCheck>=10000){ //special update step
    
    unsigned long timer = millis();
    int time[3];
    digitalWrite(STEPPIN1,HIGH);
    digitalWrite(STEPPIN2,LOW);
    lastCheck = millis();
    timeClient.update();
    while(millis()-timer < 54){
        ; //just do nothing
    }
    digitalWrite(STEPPIN1,LOW);
    digitalWrite(STEPPIN2,LOW);
    delay(8);
    timer = millis();
    digitalWrite(STEPPIN1,LOW);
    digitalWrite(STEPPIN2,HIGH);
    unsigned long epoch = timeClient.getEpochTime();
    ST = stepTime(lifeEnd-epoch, stepsLeft);
    Serial.println(ST);
    clockTime(epoch, lifeStart, lifeEnd, time);
    printClockTime(time);
    while(millis()-timer < 54){
        ; //just do nothing
    }
    digitalWrite(STEPPIN1,HIGH);
    digitalWrite(STEPPIN2,HIGH);
    delay(8);
    delay(1);
    }else{//normal step 
    digitalWrite(STEPPIN1,HIGH);
    digitalWrite(STEPPIN2,LOW);
    delay(54);
    digitalWrite(STEPPIN1,LOW);
    digitalWrite(STEPPIN2,LOW);
    delay(8);
    digitalWrite(STEPPIN1,LOW);
    digitalWrite(STEPPIN2,HIGH);
    delay(54);
    digitalWrite(STEPPIN1,HIGH);
    digitalWrite(STEPPIN2,HIGH);
    delay(8);
    delay(1);
    }
}




if((millis()-lastCheck >= 10000)&& epoch < lifeEnd){ //update PT every 10 seconds
    lastCheck = millis();
    timeClient.update();
    epoch = timeClient.getEpochTime();
    ST = stepTime(lifeEnd - epoch, stepsLeft);
    Serial.println(ST);
    int time[3];
    clockTime(epoch, lifeStart, lifeEnd, time);
    printClockTime(time);
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
                stepsLeft = initialStepsLeft(epoch, lifeStart, lifeEnd);
                ST = stepTime(lifeEnd-epoch,stepsLeft);
                Serial.println("New timeframe set");
                int time[3];
                clockTime(epoch+25, lifeStart, lifeEnd, time);
                Serial.print("You have 25 seconds to move hands to ");
                printClockTime(time);
                recalibTimer = millis();
                while(millis()-recalibTimer < 25000){ //block other processes for 15 seconds
                    Serial.println(25-((millis()-recalibTimer)/1000));
                    delay(1000);
                }
                Serial.print("Clock should now read ");
                printClockTime(time);
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





