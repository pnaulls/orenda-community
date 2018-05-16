

/**
 * Flush system.
 * 
 * Turn off heater
 * 
 * Recirculate off
 * Run pump1, 2, 3.
 */


#include "orenda.h"


void flushSetup() {
   Particle.function("flushWater", flushWater);
}

static int flushWater(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    if (power == 0) {
        powerOff("");
        return 1;
    }
    
    heaterAndPumpsOff();
     
    runState = orendaFlush;
    
    return 1;
}

 
 /**
  * Called every 200ms during flush
  */ 
 
 void flushProcess(void) {
     
     
     
 }
 
