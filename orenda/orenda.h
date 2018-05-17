

typedef enum {
   fanPower     = 0,
   recircBrew   = 1,   // Relay 1
   brewButton   = 2,
   chamberFull  = 3,
   recircRes    = 4,   // Relay 2 - Relates to pump1.
   tdsEnable    = 5,
   
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


typedef enum {
   orendaIdle,
   orendaFlush,
   orendaHeat,
   orendaBrew,
   orendaMix,
} orendaRunState;


#define lcThreshold 10  

typedef enum {
  lcDirectionUnknown = 0,
  lcDirectionDown    = 1,
  lcDirectionEven    = 2,
  lcDirectionUp      = 3,
} lcDirection;




extern orendaRunState runState;

void yield(void);

void powerDown(void);
void heaterAndPumpsOff(void);

bool heaterAction(bool on);

void lcSetup(void);
int getTDS(String command);

int parsePower(String power);


void brewSetup(void);
void brewHeat(bool chamberF, double tempReservoir);

void ledSetup(void);
void tdsSetup(void);
void tinkerSetup(void);


void flushSetup(void);
void flushProcess(double lcValue, bool chamberF);

long lcRead(bool setTare = false, bool raw = false);


