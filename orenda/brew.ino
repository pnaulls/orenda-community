
/**
 * Core brew logic
 */


#include "orenda.h"


void brewSetup(void) {
      Particle.function("brew", brewControl);
}


/**
 * The goal here is to heat the water in the chamber to between 94 and 96 degrees
 * Celcius, which appears to be an ideal brewing temperature.  Hotter, and it 
 * will burn the coffee.  Too cold, and extraction won't happen properly. 
 * 
 * Note that we can only run the heater when the water sensor says there is 
 * enough water, otherwise we risk damage and fire. 
 */ 


void brewHeat(bool chamberF, double temperature) {
    
    if (!chamberF) {
       Particle.publish("heating", "water level low", 0, PRIVATE);
       powerDown();
       return;
    }
    
}


static int brewControl(String command) {
    if (command == "heat") {
        runState = orendaHeat;
        return 1;
    }
    
    return -1;
}
