

unsigned long lifeLength(unsigned long lifeStart, unsigned long lifeEnd); //calculate length of life

double pulseTime(double timeLeft, unsigned long pulsesLeft); 

void clockTime(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd, int time[]);

long double timeLeft(unsigned long epoch, unsigned long lifeEnd);

unsigned long initialPulsesLeft(unsigned long epoch, unsigned long lifeStart, unsigned long lifeEnd);