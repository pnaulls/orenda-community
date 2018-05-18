
/**
 * Core brew logic
 */


#include "orenda.h"


static orendaRunState nextBrewState;

static int targetFillTime       = 45;   // Max time to fill chamber
static int targetMixWeight      = 350;  // Target weight in load cell
static int targetMixTime        = 90;   // Max time to mix water with coffee
static int targetDispenseTime   = 55;   // Max time to dispense
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
    
    if (now - brewTimer >= (targetFillTime * 1000)) {
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
    
    if (elapsed >= (targetMixTime * 1000)) {
        setState(orendaDispenseStart);
        
        Particle.publish("brew", "mix timeout: " + String(lcValue) + " " + String(elapsed), 0, PRIVATE);
        digitalWrite(pump2, LOW);
    }
}



/** 
 * Dispense to cup 
 */ 

void brewDispenseStart(void) {
    digitalWrite(recircBrew, HIGH);  // Recirculate brew chamber off
    digitalWrite(pump3, HIGH);
    
    setState(orendaDispense);
    
    brewTimer = millis();
}


void brewDispense(double lcValue) {
    if (lcValue <= 10) {
        Particle.publish("brew", "dispense complete", 0, PRIVATE);
        digitalWrite(pump3, LOW);   
        setState(orendaIdle);
        return;
    }
    
    // TODO: Also check that weight is decreasing
    
    long now = millis();
    
    if (now - brewTimer >= (targetDispenseTime * 1000)) {
        setState(orendaDispenseStart);
        
        Particle.publish("brew", "dispense timeout: " + String(lcValue) + " " + String(now - brewTimer), 0, PRIVATE);
        digitalWrite(pump3, LOW);
        setState(orendaIdle);
        return;
    }
}




static int brewControl(String command) {
    
    brewTimer = millis();
    
    if (command == "heat") {
        nextBrewState = orendaIdle;
        setState(orendaFillChamber);
        return 1;
        
    } else if (command == "simple") {
        nextBrewState = orendaMixStart;
        setState(orendaFillChamber);
        return 1;
    }
    
    return -1;
}



