

/**
 * LED control.  There's the standard P1 LED as well as two 
 * additional NeoPixel LEDs
 */ 

#include <neopixel.h>
#include "orenda.h"


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

static Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);



static int setColor(String command) {
    int col = command.toInt();    

    ledStrip.setColor(0, 0, 0, 0);
    //ledStrip.setColor(1, 0, 0xff, 0);
    
    ledStrip.setPixelColor(1, col);
    
    ledStrip.show();

    return 0;
}


void ledSetup(void) {
    Particle.function("setLEDs", setColor);  
    ledStrip.begin();
    
    setColor("0");
    
    ledStrip.show(); 
}    



