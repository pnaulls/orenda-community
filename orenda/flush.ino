

/**
 * Flush system.
 * 
 * Turn off heater
 * 
 * Recirculate off
 * Run pump1, 2, 3.
 */


#include "orenda.h"

#define lcThreshold 10  

typedef enum {
  lcDirectionUnknown = 0,
  lcDirectionDown    = 1,
  lcDirectionEven    = 2,
  lcDirectionUp      = 3,
} lcDirection;


static bool neverSawWater = true; // Never saw the water marker
static bool pump1Finished = false; // No more water from reservior

static lcDirection direction = lcDirectionUnknown;
static unsigned long lastWater = 0;
static double lcLastValue = 0;


void flushSetup() {
   Particle.function("flushWater", flushWater);
}

static int flushWater(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    // Stopping flush
    if (power == 0) {
        powerOff("");
        return 1;
    }
    
    // Start and setup
    
    heaterAndPumpsOff();
    neverSawWater = true;
    lastWater = millis();
     
    runState = orendaFlush;
    direction = lcDirectionUnknown;
    
    lcLastValue = loadCell("tare");
    
    return 1;
}

 
 /**
  * Called every 200ms during flush
  */ 
 
void flushProcess(bool chamberF) {
  unsigned int now = millis();
  double lcValue = loadCell(""); 
  
  if ((lcLastValue - lcValue) > lcThreshold) {
      // Decreasing
      direction = lcDirectionDown;
  } else if ((lcLastValue - lcValue) > -lcThreshold) {
      // Increasing 
      direction = lcDirectionUp;
  } else {
      direction = lcDirectionEven;
  }
  
  // Always run pump 3
  digitalWrite(pump3, HIGH);
  
  if (chamberF) {
      // Definitely water there in chamber
      neverSawWater = false;   
      lastWater = now;
  } else {
      // Run for at least 40 seconds.  
            
      if ((now - lastWater) > (40 * 1000)) {
         // Expired
         pump1Finished = true;
      }
  }
   
  digitalWrite(pump1, !pump1Finished);

  // Run pump2 if we saw water in the last 40 seconds
  
  if ((now - lastWater) > (40 * 1000)) {
     digitalWrite(pump2, LOW);  
  } else {
     digitalWrite(pump2, HIGH);   
  }    
  
}
 
 
 
 
 
