// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include "orenda.h"

void yield(void){};



static double tempReservoir;
static double tempCirculate;
static bool chamberF;

orendaRunState runState = orendaIdle;

/* This function is called once at start up ----------------------------------*/
void setup()
{    
    pinMode(fanPower, OUTPUT);
   	Particle.function("fan", fanControl);
 
    brewSetup();
    
    pinMode(chamberFull, INPUT);
    
   	pinMode(pump1, OUTPUT);
   	pinMode(pump2, OUTPUT);
   	pinMode(pump3, OUTPUT);
   	Particle.function("pump", pumpControl);
   	
   	pinMode(grinder, OUTPUT);
   	Particle.function("grinder", grinderControl);
   	
   	pinMode(motor1, OUTPUT);
   	pinMode(motor2, OUTPUT);
   	Particle.function("motor", motorControl);
   	
   	pinMode(heater, OUTPUT);
   	Particle.function("heater", heaterControl);
   	
   	pinMode(recircBrew, OUTPUT);
    pinMode(recircRes,  OUTPUT);
    Particle.function("recirculate", recircControl);
   	

   	Particle.function("powerOff", powerOff);   	
   	Particle.function("loadCell", loadCell);   	
   	   
    flushSetup();
    
    ledSetup();
    tdsSetup();
    tinkerSetup();
    
    Particle.variable("tempRes", tempReservoir);
    Particle.variable("tempCir", tempCirculate);
    Particle.variable("chamberFull", chamberF);
    
    lcSetup();
    
    brewSetup();
}


static double readTemp(orendaPins pin) {
    double value = analogRead(pin);
    
    // 100C = 910
    // 21C  = 3170
    
    value = 2260 - (value - 910);     // 0 - 2260
    value = value / 2260 * (100 - 21) + 21;  // 0 - 100
    
    return round(value * 10) / 10;
}


/**
 *  Main processing loop 
 */
void loop()
{
    tempReservoir = readTemp(tempRes);
    tempCirculate = readTemp(tempCir);
    chamberF      = digitalRead(chamberFull);

    switch (runState) {
        case orendaIdle:
            // Nothing
            break;
    
        case orendaFlush:
            flushProcess(chamberF);
            break;
            
        case orendaHeat:
            brewHeat(chamberF, tempReservoir);
            break;
    }
            
    delay(2000);
}

int parsePower(String power) {
    if (power == "0") return 0;
    if (power == "1") return 1;
    
    String lower = power.toLowerCase();
    
    if (lower == "low"  || lower == "off") return 0;
    if (lower == "high" || lower == "on")  return 1;
    
    return -1;
}


int fanControl(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    digitalWrite(fanPower, power);
    return 1;
}

int grinderControl(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    digitalWrite(grinder, power);
    return 1;
}


/**
 * Must be water in the chamber
 */ 

static bool heaterAction(bool on) {
    if (on) {
        if (!chamberFull) {
          return false;
        }
    }
    
    digitalWrite(heater, on);
    
    return true;
}


int heaterControl(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    return heaterAction(power);
}


int recircControl(String command) {
     int comma = command.indexOf(",");
    
    if (comma == -1) return -1;
    
    String name = command.substring(0, comma);
    String value = command.substring(comma + 1);
    
    int recircNum;
    
    if (name == "1") 
        recircNum = recircRes;
    else if (name == "2")
        recircNum = recircBrew;
    else
        return -2;
        
    int power = parsePower(value);
    
    if (power == -1) return -3;
        
    digitalWrite(recircNum, !power);
    return 1;
}



 
 /** 
  * Pump control.  Pumps 1-3, 0,1 or high low.
  * 
  * Additionally, pump into the brew chamber can be PWM
  * controlled. A third parameter is accepted from 0-255.
  */ 
 

int pumpControl(String command) {
    int comma1 = command.indexOf(",");
    
    if (comma1 == -1) return -1;
    
    int comma2 = command.indexOf(",", comma1 + 1);
    
    String name = command.substring(0, comma1);
    String value = (comma2 == -1) ? 
                   command.substring(comma1 + 1) :
                   command.substring(comma1 + 1, comma2);
    
    int pumpNum;
    
    if (name == "1") 
        pumpNum = pump1;
    else if (name == "2")
        pumpNum = pump2;
     else if (name == "3")
        pumpNum = pump3;
    else
        return -2;
        
    int power = parsePower(value);
    
    if (power == -1) return -3;

    // PWM control
    if (comma2 != -1) {
        if (pumpNum != pump2) return -4;
        
        String pwm = command.substring(comma2 + 1);
        int pValue = pwm.toInt();
        
        if (power)
            analogWrite(pumpNum, pValue);
        else
            digitalWrite(pumpNum, power);
    
    } else {
        digitalWrite(pumpNum, power);
    }
    
    return 1;
}
  
  
int motorControl(String command) {
    int comma = command.indexOf(",");
    
    if (comma == -1) return -1;
    
    String name = command.substring(0, comma);
    String value = command.substring(comma + 1);
    
    int motorNum;
    
    if (name == "1") 
        motorNum = motor1;
    else if (name == "2")
        motorNum = motor2;
    else
        return -2;
        
    int power = parsePower(value);
    
    if (power == -1) return -3;
        
    digitalWrite(motorNum, power);
    
    return 1; 
}  


void heaterAndPumpsOff(void) {
    heaterAction(false);    
    
    digitalWrite(recircBrew, HIGH);  // Recirculate brew chamber off
    digitalWrite(recircRes,  HIGH);  // Recirculate reservoir off
     
    digitalWrite(pump1, LOW);
    digitalWrite(pump2, LOW);
    digitalWrite(pump3, LOW);
}


void powerDown(void) {
    runState = orendaIdle;
    
    // Turn off heater and pumps
    heaterAndPumpsOff();
    
    // Turn off motors
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    // Grinder and fan
    digitalWrite(grinder, LOW);
    digitalWrite(fanPower, LOW);
}



/**
 * Turn off as much as possible
 */ 
int powerOff(String command) {
    powerDown();    
   
    return 1;
}



















