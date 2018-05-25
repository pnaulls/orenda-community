
/**
 * Orenda firmware by Peter Naulls <peter@chocky.org>
 * 
 * Core functions.   The individual functions are split into their own
 * files.  
 * 
 * There are two types of control - individual peripheral for testing and then
 * brewer function such as flush, brew and cleaning. 
 * 
 * This code is subject to the MIT License.  See the LICENSE file. 
 */



#include "orenda.h"

void yield(void){};


// Variable that get upated on main loop
static double tempReservoir;
static double tempCirculate;
static bool chamberF;

// Current state in state machine.
orendaRunState runState = orendaStartup;

// Main loop timer
static unsigned long lastLoop;

/**
 * Called at start up
 * Most of the setup is in per-file setup functions.  
 */

void setup()
{   
  // Not particularly used
  pinMode(fanPower, OUTPUT);
   	Particle.function("fan", fanCommand);
 
  brewSetup();
  
  // Hidden chamber full sensor
  pinMode(chamberFull, INPUT);
  
  // Pump setup
  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  pinMode(pump3, OUTPUT);
  Particle.function("pump", pumpCommand);
   	
  // Grinder and shaker setup
  pinMode(grinder, OUTPUT);
  Particle.function("grinder", grinderCommand);
   	
  pinMode(shaker1, OUTPUT);
  pinMode(shaker2, OUTPUT);
  Particle.function("shaker", shakerCommand);
  pinMode(heater, OUTPUT);
  Particle.function("heater", heaterCommand);
   	
  // Return to low power idle state
  Particle.function("powerDown", powerDownCommand);
  
  // Expose common variable readings
  Particle.variable("tempRes", tempReservoir);
  Particle.variable("tempCir", tempCirculate);
  Particle.variable("chamberFull", chamberF);
  
  // Individual method setup
  recircSetup();
  flushSetup();  
  tdsSetup();
  lcSetup();
  brewSetup();
  ledSetup();
  
//  tinkerSetup();

  
  lastLoop = millis();
}

 
/**
 * State names used in notifications 
 */ 
 
static String getStateName(orendaRunState state) {
  switch (state) {
    case orendaStartup:
      return "startup";
  
    case orendaIdle:
      return "idle";
  
    case orendaFlush:
      return "flush";
    
    case orendaFillChamber:
      return "fill chamber";
    
    case orendaHeat:
      return "heat";
      
    case orendaMixStart:
      return "mix start";
      
    case orendaMix:
      return "mix";
      
    case orendaDispenseStart:
      return "dispense start";
      
    case orendaDispense:
        return "dispense";
  }
  
  return "unknown";
}


/**
 * LED modes to match various states. 
 * 
 * TODO: Add changing color handling.
 *
 * See also ledSetColor in led.ino 
 */

void setStateColors(void) {
   switch (runState) {
    case orendaIdle:
      // Both red
      ledSetColor(1, 0xff0000);
      ledSetColor(2, 0xff0000);
      return;
  
    case orendaFlush:
      // Both blue
      ledSetColor(1, 0x0000ff);
      ledSetColor(2, 0x0000ff);
      return;
      
    case orendaFillChamber:
      break;
      
    case orendaHeat:
      break;
      
    case orendaMixStart:
      break;
      
    case orendaMix:
      break;
      
    case orendaDispenseStart:
      break;
      
    case orendaDispense:
      break;
  }  
  
  // Both green
  ledSetColor(1, 0x00ff00);
  ledSetColor(2, 0x00ff00);
}


void setState(orendaRunState state) {
  if (state == runState) return;  
  
  runState = state;
  
  if (state == orendaIdle) {
    powerDown();
  } 
  
  setStateColors();
  
  Particle.publish("orenda/state", getStateName(runState), 0, PRIVATE);
}


unsigned int parseHex(String hex) {
  unsigned int value = 0;

  for (unsigned int pos = 2; pos < hex.length(); pos++) {
    int c = hex.charAt(pos);
   
    value = value << 4;
   
    if (c >= '0' && c <= '9') {
      value |= c - '0';  
    } else if (c >= 'a' && c <= 'f') {
      value |= c - 'a' + 10;  
    } else if (c >= 'A' && c <= 'F') {
      value |= c - 'a' + 10;  
    } else {
      return value >> 4;
    } 
  }
  
  return value;
}




static double readTemp(orendaPins pin) {
  double value = analogRead(pin);
  
  // 100C = 910
  // 21C  = 3170
  
  value = 2260 - (value - 910);   // 0 - 2260
  value = value / 2260 * (100 - 21) + 21;  // 0 - 100
  
  return round(value * 10) / 10;
}




/**
 *  Main processing loop 
 */
void loop()
{
  unsigned int freq = (runState == orendaIdle) ? 10000 : 2000;
  unsigned long now = millis();
  double lcValue;
  
  if (now - lastLoop > freq) {
    tempReservoir = readTemp(tempRes);
    tempCirculate = readTemp(tempCir);
    chamberF    = digitalRead(chamberFull);
     
  }
  
  if (runState == orendaIdle) {
    return;
  }
  
  lcValue = lcRead();
  
  // TODO: 
  // Reduce polling during idle state, and turn 
  // more things off.
  // Calculate load cell direction here instead of in the
  // flush handling. 
  // Reduce 2 second delay to process more often. 
  
  switch (runState) {
    case orendaStartup:
      setState(orendaIdle);
      break;
    
    case orendaIdle:
      // Nothing
      break;
  
    case orendaFlush:
      flushProcess(lcValue, chamberF);
      break;
      
    case orendaFillChamber:
      brewFill(chamberF);
      break;
      
    case orendaHeat:
      brewHeat(chamberF, tempReservoir);
      break;
      
    case orendaMixStart:
      brewMixStart();
      break;
      
    case orendaMix:
      brewMix(lcValue);
      break;
      
    case orendaDispenseStart:
      brewDispenseStart(orendaIdle);
//      brewDispenseStart(orendaDispense);
      break;
      
    case orendaDispense:
      brewDispense(lcValue);
      break;
  }
      
  delay(200);
}

int parsePower(String power) {
  if (power == "0") return 0;
  if (power == "1") return 1;
  
  String lower = power.toLowerCase();
  
  if (lower == "low"  || lower == "off") return 0;
  if (lower == "high" || lower == "on")  return 1;
  
  return -1;
}


int fanCommand(String command) {
  int power = parsePower(command);
  
  if (power == -1) return -1;
  
  digitalWrite(fanPower, power);
  return 1;
}

int grinderCommand(String command) {
  int power = parsePower(command);
  
  if (power == -1) return -1;
  
  digitalWrite(grinder, power);
  return 1;
}


/**
 * Must be water in the chamber
 */ 

bool heaterAction(bool on) {
  if (on) {
    if (!chamberFull) {
      return false;
    }
  }
  
  digitalWrite(heater, on);
  
  return true;
}


int heaterCommand(String command) {
  int power = parsePower(command);
  
  if (power == -1) return -1;
  
  return heaterAction(power);
}


 
 /** 
  * Pump control.  Pumps 1-3, 0,1 or high low.
  * 
  * Additionally, pump into the brew chamber can be PWM
  * controlled. A third parameter is accepted from 0-255.
  * 
  * e.g,  
  *  1,0    - pump 1 off  
  *  2,1,128  - pump 2 50%
  * 
  * The pump rates vary by water temperature.  See the notes in brew.ino
  */ 

int pumpCommand(String command) {
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
  
  
int shakerCommand(String command) {
  int comma = command.indexOf(",");
  
  if (comma == -1) return -1;
  
  String name = command.substring(0, comma);
  String value = command.substring(comma + 1);
  
  int shakerNum;
  
  if (name == "1") 
    shakerNum = shaker1;
  else if (name == "2")
    shakerNum = shaker2;
  else
    return -2;
    
  int power = parsePower(value);
  
  if (power == -1) return -3;
    
  digitalWrite(shakerNum, power);
  
  return 1; 
}  


/**
 * Just turn off header and pumps.  
 */ 

void heaterAndPumpsOff(void) {
  heaterAction(false);  
  
  recircControl(recircBrew, false); // Recirculate brew chamber off
  recircControl(recircRes,  false); // Recirculate reservoir off
  
  digitalWrite(pump1, LOW);
  digitalWrite(pump2, LOW);
  digitalWrite(pump3, LOW);
}


/**
 * Idle state.  Could also turn off TDS clock.
 */ 

void powerDown(void) {
  runState = orendaIdle;
  
  // Turn off heater and pumps
  heaterAndPumpsOff();
  
  // Turn off shakers
  digitalWrite(shaker1, LOW);
  digitalWrite(shaker2, LOW);
  // Grinder and fan
  digitalWrite(grinder, LOW);
  digitalWrite(fanPower, LOW);
}



/**
 * Turn off as much as possible
 */ 
int powerDownCommand(String command) {
  powerDown();  
   
  return 1;
}



