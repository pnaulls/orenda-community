
/**
 * Cleaning cycles. 
 */


#include "orenda.h"

static int cleanCommand(String command) {
  setState(orendaClean1);
  return 1;
}


void cleanSetup(void) {
  Particle.function("clean", cleanCommand); 
}


void cleanProcess(unsigned int now) {
  setState(orendaIdle);    
}
