

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
  shaker1     = 13,   // Top shaker 
  pump2       = 14,   // Top pump
  heater      = 15,   // Relay 3
  ledNP       = 16,   // LED 2/3 control, NeoPixel
  buttonClean = 17,   
   
  ledR        = 21,   // LED 1/System LED control
  ledG        = 22, 
  ledB        = 23,
   
  shaker2     = 24,   // Bottom shaker
  tds         = 25,   // TDS frequency count
  grinder     = 26,   // Grinder motor
  pump3       = 27,   // Dispense or recirculate
   
  lcCLK       = 28,   // Load cell clock
  lcDAT       = 29,   // Load cell data
 
} orendaPins;


typedef enum {
  orendaStartup,
  orendaIdle,
  orendaFlush,
  orendaFillChamber,
  orendaHeat,
  orendaMixStart,
  orendaMix,
  orendaDispenseStart,
  orendaDispense,
} orendaRunState;


#define lcThreshold 10  

typedef enum {
  lcDirectionUnknown = 0,
  lcDirectionDown    = 1,
  lcDirectionEven    = 2,
  lcDirectionUp      = 3,
} lcDirection;


void setState(orendaRunState state);

unsigned int parseHex(String hex);

void yield(void);

void powerDown(void);
void heaterAndPumpsOff(void);

bool heaterAction(bool on);


int getTDS(String command);

int parsePower(String power);


void brewSetup(void);

void brewFill(bool chamberF);
void brewHeat(bool chamberF, double tempReservoir);
void brewMixStart();
void brewMix(double lcValue);
void brewDispenseStart(orendaRunState nextState);
void brewDispense(double lcValue);

void ledSetup(void);
int ledSetColor(unsigned int num, int col);

void tdsSetup(void);
void tinkerSetup(void);


void flushSetup(void);
void flushProcess(double lcValue, lcDirection direction, bool chamberF);

void lcSetup(void);
void lcSetTare(double tareValue = 0.0);
long lcRead(lcDirection &direction, bool setTare = false, bool raw = false);
String lcDirectionName(lcDirection direction);


int recircControl(int recircNum, bool recirculate);
void recircSetup(void);

void grinderSetup(void);
void grinderAndShakerControl(bool on);



