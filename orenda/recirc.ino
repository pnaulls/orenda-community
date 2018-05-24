
/**
 * Recirculation control.
 * 
 * The recirculation is controled by relay valves.  
 * 
 * The use of the first one isn't entirely clear, but could
 * potentially be used for descaling, since it loops back into
 * the reservior.  
 * 
 * The second recirculation loops from the bottom of the brew 
 * chamber past the second thermometer and TDS meter back into
 * the brew chamber.  
 * 
 */ 


#include "orenda.h"



int recircControl(int recircNum, bool recirculate) {
   
  if (recircNum != recircRes && recircNum != recircBrew) {
    return -2;
  }
  
  digitalWrite(recircNum, !recirculate);
  return 1;
}



static int recircCommand(String command) {
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
        
  return recircControl(recircNum, power);
}




void recircSetup(void) {
  pinMode(recircBrew, OUTPUT);
  pinMode(recircRes,  OUTPUT);
  Particle.function("recirculate", recircCommand); 
}


