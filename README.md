## Life Clock
### Code for displaying custom time periods on analog clock

The project uses an ESP8266 NodeMCU board to control the internal motor of an analog wall clock. The ESP8266 needs a wifi connection to recieve time data. The project requires the Adafruit_NeoPixel, NTPClient, and EEPROM libraries. 

If you make this, just follow the comments in main.cpp to include your WiFi credentials. You'll also need to go through the serial menu the first time you turn on the clock, but after that it only needs to be used to calibrate the clock or update the time period. 
The clock's internal coil should be connected to GPIO 4 and 5 on the ESP8266 devboard. The led strip can be attached to pin 13 (or any other, if you edit the code). 

### If you do not want to include LEDs, just comment out all of the "pixels...." lines. 
