
/**
 * Grinder and shaker/motor functions
 * 
 */

#include "orenda.h"


void shakerControl(int shakerPin, bool on) {
  digitalWrite(shakerPin, on); 
}


/**
 * Turn on all 
 */
void grinderAndShakerControl(bool on) {
  shakerControl(shaker1, on);
  shakerControl(shaker2, on);
  
  digitalWrite(grinder, on);
}


static int shakerCommand(String command) {
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
  
  shakerControl(shakerNum, power);
  
  return 1; 
}  




static int grinderCommand(String command) {
  if (command == "2") {
    grinderAndShakerControl(true);
    return 1;
  }
  
  int power = parsePower(command);
  
  if (power == -1) return -1;
 
  if (power) {
    digitalWrite(grinder, power);
  } else {
    grinderAndShakerControl(false);
  }
    
  return 1;
}



void grinderSetup(void) {
  pinMode(grinder, OUTPUT);
  Particle.function("grinder", grinderCommand);
   	
  pinMode(shaker1, OUTPUT);
  pinMode(shaker2, OUTPUT);
  Particle.function("shaker", shakerCommand);
  
}
