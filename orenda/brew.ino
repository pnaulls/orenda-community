
/**
 * Core brew logic
 */


#include "orenda.h"


static orendaRunState nextBrewState;

/**
 * 
 * Fill rates:
 * 
 * Pump 1:   
 *   Sensor at 40 seconds, room temperature
 *     10.8 ml/second
 *
 * Pump 2:
 *   350 ml at 28 seconds, room temperature
 *       12.5 ml/second
 *     58.3 seconds, when heated
 *       6 ml/second
 *   
 */
 

// Fill times for each pump 
// Room temperature = 21C, hot = 95C

// Heating chamber - 430ml (to sensor)
static const int pump1room = 40;
static const int pump1hot  = 60;

// Brew/Mix chamber - 350ml 
static const int pump2room = 28;
static const int pump2hot  = 60;
static int targetMixWeight  = 350;  // Target weight in load cell

// Dispense - 350ml
static const int pump3room = 50;
static const int pump3hot  = 50;


// Calculated times depending upon temperature
static int pump1time;
static int pump2time;
static int pump3time;

static long brewTimer;


void brewSetup(void) {
      Particle.function("brew", brewControl);
}


/** 
 * Fill chamber
 */

void brewFill(bool chamberF) {
    if (chamberF) {
        digitalWrite(pump1, LOW);
        setState(orendaHeat);
        return;
    }
     
    digitalWrite(pump1, HIGH);
    
    long now = millis();
    
    if (now - brewTimer >= (pump1time * 1000)) {
        setState(orendaDispenseStart);
        
        Particle.publish("brew", "fill timeout: " + String(now - brewTimer), 0, PRIVATE);
        digitalWrite(pump1, LOW);
        setState(orendaIdle);
        return;
    }
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
        
        setState(nextBrewState);
    }
}


void brewMixStart() {
    // Tare before mixing
    lcRead(true);
    brewTimer = millis();
    
    setState(orendaMix);
    
    // Start pump 2
    digitalWrite(pump2, HIGH);
}



/** 
 * Pump the water over the coffee grounds. 
 */ 

void brewMix(double lcValue) {
    unsigned long now = millis();
    unsigned long elapsed = now - brewTimer;
    
    if (lcValue >= targetMixWeight) {
        setState(orendaDispenseStart);
        
        Particle.publish("brew", "mix complete: " + String(lcValue) + " " + String(elapsed), 0, PRIVATE);
        digitalWrite(pump2, LOW);
        return;
    }
    
    // TODO: Also check that weight is increasing
    
    if (elapsed >= (pump2time * 1000)) {
        setState(orendaDispenseStart);
        
        Particle.publish("brew", "mix timeout: " + String(lcValue) + " " + String(elapsed), 0, PRIVATE);
        digitalWrite(pump2, LOW);
    }
}



/** 
 * Dispense to cup 
 */ 

void brewDispenseStart(orendaRunState nextState) {
  
  recircControl(recircBrew, false); // Recirculate brew chamber off
  
  digitalWrite(pump3, HIGH);
    
  // Load cell varies by temperature.  Reset here.
  //lcSetTare(targetMixWeight);
    
  setState(nextState);
    
  brewTimer = millis();
}


void brewDispense(double lcValue) {

    long now = millis();
    Particle.publish("brew", "dispense: " + String(lcValue) + " " + String(now - brewTimer), 0, PRIVATE);
    
    if (lcValue <= 10) {
        Particle.publish("brew", "dispense complete", 0, PRIVATE);
        digitalWrite(pump3, LOW);   
        setState(orendaIdle);
        return;
    }
    
    // TODO: Also check that weight is decreasing
    
   
    
    if (now - brewTimer >= (pump3time * 1000)) {
        Particle.publish("brew", "dispense timeout: " + String(lcValue) + " " + String(now - brewTimer), 0, PRIVATE);
        digitalWrite(pump3, LOW);
        setState(orendaIdle);
        return;
    }
}




static int brewControl(String command) {
    int comma = command.indexOf(",");
    String brewOp;
    unsigned int size = 350;
    
    if (comma == -1) {
        brewOp = command;
    } else {
        brewOp = command.substring(0, comma);
        
        String sizeCommand = command.substring(comma + 1);
        if (sizeCommand.substring(0, 5) == "size=") {
            size = sizeCommand.substring(5).toInt();
            
            if (size < 100 || size > 350) {
                return -2;
            }            
        }
    }
    
    
    brewTimer = millis();
    
    // Proportion of nominal 350ml cup.
    double proportion = (double)size / 350.0;
    
    pump1time = pump1hot; 
    pump2time = pump2hot * proportion;
    pump3time = pump3hot * proportion;
    
    targetMixWeight = size;
    
    if (brewOp == "heat") {
        nextBrewState = orendaIdle;
        setState(orendaFillChamber);
        return 1;
        
    } else if (brewOp == "simple") {
        nextBrewState = orendaMixStart;
        setState(orendaFillChamber);
        return 1;
        
    } else if (brewOp == "dispense") {
        brewDispenseStart(orendaDispense);
        return 1;
    }
    
    
    return -1;
}



