#include <stdio.h>
#include <iostream>
using namespace std;

int timeOnClock = 0;
unsigned long soFar = 86400;
int lifeSecond = 2; //1 life second = 2 normal seconds


int main(){

timeOnClock += soFar/(3600*lifeSecond)*10000;
cout << timeOnClock << "\n"; 
timeOnClock += ((soFar%(3600*lifeSecond))/60/lifeSecond)*100;
cout << timeOnClock << "\n"; 
timeOnClock += (soFar%(60*lifeSecond))/lifeSecond;
//timeOnClock += soFar - (((soFar/(3600*lifeSecond))*3600*lifeSecond) + (((soFar%(3600*lifeSecond))/(60*lifeSecond))*60*lifeSecond));
cout << "Final Time: " << timeOnClock << "\n"; 

}