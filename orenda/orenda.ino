// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include "orenda.h"

void yield(void){};


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);


static double tempReservoir;
static double tempCirculate;
static bool chamberF;

/* This function is called once at start up ----------------------------------*/
void setup()
{    
    pinMode(fanPower, OUTPUT);
   	Particle.function("fan", fanControl);
 
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
   	
   	Particle.function("flushWater", flushWater);
   	Particle.function("powerOff", powerOff);   	
   	Particle.function("loadCell", loadCell);   	
   	   
   //	Particle.function("oneWire", oneWireControl);  	

 	
    Particle.function("strip", strip);  
    ledStrip.begin();
    ledStrip.show();
    
    tdsSetup();
    
    tinkerSetup();
    
    Particle.variable("tempRes", tempReservoir);
    Particle.variable("tempCir", tempCirculate);
    Particle.variable("chamberFull", chamberF);
    
    lcSetup();
}


static double readTemp(orendaPins pin) {
    double value = analogRead(pin);
    
    // 100C = 910
    // 21C  = 3170
    
    value = 2260 - (value - 910);     // 0 - 2260
    value = value / 2260 * (100 - 21) + 21;  // 0 - 100
    
    return round(value * 10) / 10;
}


/* This function loops forever --------------------------------------------*/
void loop()
{
	// This will run in a loop
	
	tempReservoir = readTemp(tempRes);
	tempCirculate = readTemp(tempCir);
    chamberF      = digitalRead(chamberFull);
	
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

int heaterControl(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    digitalWrite(heater, power);
    return 1;
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
 * Flush system.
 * 
 * Turn off heater
 * 
 * Recirculate off
 * Run pump1, 2, 3.
 */
 
 int flushWater(String command) {
     int power = parsePower(command);
    
     if (power == -1) return -1;
     
     digitalWrite(heater, 0);  // Heater off
     digitalWrite(recircBrew, 1);  // Recirculate brew chamber off
     digitalWrite(recircRes,  1);      // Recirculate reservoir off
     
     digitalWrite(pump1, power);
     digitalWrite(pump2, power);
     digitalWrite(pump3, power);
     
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


/**
 * Turn off as much as possible
 */ 
int powerOff(String command) {
    
    // Turn off heater and pumps
    flushWater("0");
    
    // Turn off motors
   digitalWrite(motor1, 0);
   digitalWrite(motor2, 0);
   // Grinder and fan
   digitalWrite(grinder, 0);
   digitalWrite(fanPower, 0);
   
   return 1;
}




/*int led(String command) {
    //return 0;
    
   RGB.control(true);

   // red, green, blue, 0-255.
   // the following sets the RGB LED to white:
   RGB.color(255, 255, 255);

  return 0;
}*/





int strip(String command) {

    int col = command.toInt();    

	ledStrip.setColor(0, 0, 0, 0);
	//ledStrip.setColor(1, 0, 0xff, 0);
	
	ledStrip.setPixelColor(1, col);
	
	ledStrip.show();

    return 0;
}












