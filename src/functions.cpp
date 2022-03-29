#include <extern.h>

#define STEPPIN1 4
#define STEPPIN2 5


unsigned long lifeLength(unsigned long lifeStart, unsigned long lifeEnd){ 
    unsigned long life = lifeEnd - lifeStart;
    return life;
}

unsigned long initialStepsLeft(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd){ //gets inital number of steps left based on ratio of passed time in timeframe
    long double timeLeft = lifeEnd - epoch;
    unsigned long life = lifeEnd - lifeStart;
    long double ratio = timeLeft/life;
    
    unsigned long stepsLeft =  345600*ratio;
    
    return stepsLeft;
}


long double timeLeft(unsigned long epoch, unsigned long lifeEnd){ //calculates amount of time left in timeframe, in real seconds
    long double timeLeft = lifeEnd - epoch;
    return timeLeft;
}

double stepTime(double timeLeft, unsigned long stepsLeft){//calculates length of pulse for clock
    
    double ST = 1000*(timeLeft / stepsLeft);
    return ST;
}

void clockTime(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd, int time[]){
    if(epoch<lifeEnd){
        double lifeLength = lifeEnd - lifeStart;
    unsigned long timePassed = epoch - lifeStart;
    double lifeRatio = timePassed / lifeLength;
    int clockSeconds = lifeRatio * 43200;
    time[0] = clockSeconds / 3600; //number of hours in clockseconds
    time[1] = (clockSeconds % 3600)/60;
    time[2] = clockSeconds % 60;
    }else{ //if timeframe has ended
        time[0] = 12;
        time[1] = 0;
        time[2] = 0;
    }
    
   

}


void printClockTime(int time[]){
    for( int i = 0; i<=2; i++){
        Serial.print(time[i]);
        if(i < 2){Serial.print(":");}
        
    }
    Serial.print("\n");
}

