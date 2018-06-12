

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
 * 
 * The TDS meter could be disabled during idle operation. 
 */

#include "orenda.h"


void tdsSetup(void) {
  pinMode(tds, INPUT);
  pinMode(tdsEnable, OUTPUT);
  digitalWrite(tdsEnable, HIGH);
  
  Particle.function("tds", getTDS);  
  
}



static double readTDS(void) {
  int total = 0;
  int runs = 5;
  int direction = LOW;
  
  for (int count = 0; count < runs; count++) {
    int value = pulseIn(tds, direction);
    
    //String message = "count: " + String(count) + " value : " + String(value);    
    //Particle.publish("tds", message);
    
    // Definitely no water, otherwise check over average
    if (value > 120) return value;
    
    // Timeout
    // Zero here indicates it has been disabled, D5 has been set low.
    if (value >= 500 || value == 0) return -2;
    
    total += value;
  }
  
 //   String message = "min:" + String(min) + " max: " + String(max);
 //   Particle.publish("tds", message);
  
  return (double)total / runs;
}


/**
 * TDS readings. 
 * 
 * -1 = no water
 * -2 = no clock found, TDS is disabled
 * 
 * Otherwise TDS value, which is presently poorly calibrated.
 */


int getTDS(String command) {
  double value = readTDS();  
  
  // Indicate no water
  if (value > 97) return -1;
  
  // Timeout
  if (value == -2) return -2;
  
  value = 84 - (value - 13);  // 0 - 84
  value = value * 1200 / 84;  // 0 - 1200
  
  if (value < 0) value = 0;  // Don't confuse with no water values
  
  return value;  
}






