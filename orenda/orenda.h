

typedef enum {
   fanPower     = 0,
   recircBrew   = 1,   // Relay 1
   brewButton   = 2,
   recircRes    = 4,   // Relay 2 - Relates to pump1.
   
   tempRes     = 10,
   pump1       = 11,   // Reservoir
   tempCir     = 12,   // Temperature in recirculation
   motor1      = 13,
   pump2       = 14,   // Top pump
   heater      = 15,   // Relay 3
   ledNP       = 16,   // LED 2/3 control, NeoPixel
   buttonClean = 17,   
   
   ledR        = 21,   // LED 1 control
   ledG        = 22, 
   ledB        = 23,
   
   motor2      = 24,
   tds         = 25,  // TDS frequency count
   grinder     = 26,  // Grinder motor
   pump3       = 27,  // Dispense or recirculate
   
   lcCLK       = 28,  // Load cell clock
   lcDAT       = 29,  // Load cell data
 
} orendaPins;


void yield(void);

void lcSetup(void);
double loadCell(String command);
int getTDS(String command);
