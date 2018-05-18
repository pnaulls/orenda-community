
/**
 * Core brew logic
 */


#include "orenda.h"


static orendaRunState nextBrewState;
static int targetMixWeight = 350;
static int targetMixTime   = 40;
static long brewTimer;


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
       Particle.publish("heating", "waterLevelLow", 0, PRIVATE);
       powerDown();
       return;
    }
    
    if (temperature < 94) {
        // Not hot enough
        heaterAction(true);  
    } else if (temperature > 96) {
        // Let cool off
        heaterAction(false);
    } else {
        // Range achieved. 
        heaterAction(false);
        
        double tempR = round(temperature * 10) / 10;
        
        Particle.publish("heating", "finished," + String(tempR), 0, PRIVATE);
        
        runState = nextBrewState;
    }
}


void brewMixStart() {
    // Tare before mixing
    lcRead(true);
    brewTimer = millis();
    
    runState = orendaMix;
    
    // Start pump 2
    digitalWrite(pump2, HIGH);
}



/** 
 * Pump the water over the coffee grounds. 
 */ 

void brewMix(double lcValue) {
    if (lcValue >= targetMixWeight) {
        runState = orendaDispense;
        
        Particle.publish("brew", "mix complete", 0, PRIVATE);
        digitalWrite(pump2, LOW);
        return;
    }
    
    // TODO: Also check that weight is increasing
    
    long now = millis();
    
    if (now - brewTimer >= (targetMixTime * 40)) {
        runState = orendaDispense;
        
        Particle.publish("brew", "mix timeout", 0, PRIVATE);
        digitalWrite(pump2, LOW);
    }
}


/** 
 * Dispense to cup 
 */ 

void brewDispense(void) {
    powerDown();   
}




static int brewControl(String command) {
    
    if (command == "heat") {
        nextBrewState = orendaIdle;
        runState = orendaHeat;
        return 1;
        
    } else if (command == "simple") {
        nextBrewState = orendaMixStart;
        runState = orendaHeat;
        return 1;
    }
    
    return -1;
}



