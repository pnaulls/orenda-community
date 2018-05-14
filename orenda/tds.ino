

/**
 * Unknown model of load cell.  
 * 
 * However,reported gaps between pulses on pin 25:
 * 
 * No water:
 *    low  147
 *    high 152
 *
 * Tap water (Assume 0 ppm, although that may be way off for local water)
 *    low  97
 *    high 103
 *
 * 
 * 1200 ppm salt solution
 *    low  13
 *    high 19
 * 
 */

#include "orenda.h"





static double readTDS(void) {
    int total = 0;
    int runs = 16;
    int direction;
    
//    if (command == "1")
//        direction = HIGH;
//    else
        direction = LOW;
    
    for (int count = 0; count < runs; count++) {
        int value = pulseIn(tds, direction);
        
        total += value;
    }
    
 //   String message = "min:" + String(min) + " max: " + String(max);
 //   Particle.publish("tds", message);
    
    return (double)total / runs;
}


int getTDS(String command) {
  double value = readTDS();  
    
  // Indicate no water
  if (value > 97) return -1;
  
  value = 84 - (value - 13);  // 0 - 84
  value = value * 1200 / 84;  // 0 - 1200
  
  if (value < 0) value = 0;  // Don't confuse with no water values
  
  return value;  
}






