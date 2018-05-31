

/**
 * LED control.  There's the standard P1 LED as well as two 
 * additional NeoPixel LEDs
 */ 

#include <neopixel.h>
#include "orenda.h"


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812
//#define PIXEL_TYPE TM1803

static Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);

static int led2col = 0;
static int led3col = 0;


int ledSetColor(unsigned int led, int col) {
  
  if (led == 1) {
    // System LED
        
    if (col == -1) {
      // Return to system
      RGB.control(false);
         
    } else {
      RGB.control(true);
      RGB.color(col >> 16, (col >> 8) & 0xff, col & 0xff);
    }
        
  } else if (led == 2) {
    ledStrip.setPixelColor(0, col);
    
    ledStrip.show();
    
  } else if (led == 3) {
    //led3col = col;
    
    ledStrip.setPixelColor(1, col);
    
    ledStrip.show();
        
  } else {
    return -2;
  }
    
  delay(100);  // Seems to be needed or the other LED can become unset
    
  return 1;  
}


/**
 * Set LED colors.  
 * 
 * Note that LED 1 is the system LED, and setting a value
 * will override the default LED handling including the normal
 * "breathing".  This might be useful for the app to set this
 * for users who don't like this.  Set to -1 to return to the
 * system mode.
 * 
 * LEDs 2 and 3 are the same model and are brighter than LED 1
 * 
 * The colors are in RGB format, and can get decimal or hex.
 * 
 * 2,0xff00ff - LED 2 to magenta. 
 */


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



