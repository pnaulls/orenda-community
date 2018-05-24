

/**
 * LED control.  There's the standard P1 LED as well as two 
 * additional NeoPixel LEDs
 */ 

#include <neopixel.h>
#include "orenda.h"


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

static Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);


int ledSetColor(unsigned int led, int col) {
   
  if (led == 0) {
    // System LED
        
    if (col == -1) {
      // Return to system
      RGB.control(false);
         
    } else {
      RGB.control(true);
      RGB.color(col >> 16, (col >> 8) & 0xff, col & 0xff);
    }
        
  } else if (led == 1 || led == 2) {
    ledStrip.setPixelColor(led - 1, col);
    ledStrip.show();
        
  } else {
    return -2;
  }
    
  return 0;  
}



static int setColor(String command) {
    int comma = command.indexOf(",");
    
    if (comma == -1) return -1;
    
    String lNum  = command.substring(0, comma);
    String value = command.substring(comma + 1);

    int led = lNum.toInt();    
    int col;
    
    if (value.substring(0, 2) == "0x") {
      col = parseHex(value);
    } else {
      col = value.toInt();    
    }

    return ledSetColor(led, col);
}


void ledSetup(void) {
    Particle.function("setLEDs", setColor);  
    ledStrip.begin();    
    
    ledStrip.setPixelColor(1, 0);
    ledStrip.setPixelColor(2, 0);
    
    ledStrip.show(); 
}    



