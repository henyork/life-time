#include <Arduino.h>
bool pinVal = 1;
unsigned long counter = 0;
void setup(){
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  for(int i=0; i<60; i++){

    for(int p=0; p<32768*2; p++){
      pinVal = ! pinVal;
      digitalWrite(4, pinVal);
      delayMicroseconds(80);
      counter ++;
      yield();
    }
    Serial.println(counter);
  }
}

void loop(){
;
}