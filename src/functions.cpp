#include <Arduino.h>

unsigned long lifeLength(unsigned long lifeStart, unsigned long lifeEnd){ 
    unsigned long life = lifeEnd - lifeStart;
    return life;
}

unsigned long initialPulsesLeft(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd){ //gets inital number of pulses left based on ratio of passed time in timeframe
    long double timeLeft = lifeEnd - epoch;
    unsigned long life = lifeEnd - lifeStart;
    long double ratio = timeLeft/life;
    
    unsigned long pulsesLeft =  1415577600*ratio;
    Serial.println(pulsesLeft);
    return pulsesLeft;
}


long double timeLeft(unsigned long epoch, unsigned long lifeEnd){ //calculates amount of time left in timeframe, in real seconds
    long double timeLeft = lifeEnd - epoch;
    return timeLeft;
}

double pulseTime(double timeLeft, unsigned long pulsesLeft){//calculates length of pulse for clock
    
    double PT = 1000000*(timeLeft / pulsesLeft);
    return PT;
}

void clockTime(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd, int time[]){
    double lifeLength = lifeEnd - lifeStart;
    unsigned long timePassed = epoch - lifeStart;
    double lifeRatio = timePassed / lifeLength;
    int clockSeconds = lifeRatio * 43200;
    time[0] = clockSeconds / 3600; //number of hours in clockseconds
    time[1] = (clockSeconds % 3600)/60;
    time[2] = clockSeconds % 60;
   

}
