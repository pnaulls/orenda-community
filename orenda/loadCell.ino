

/**
 * The load cell in the Orenda is this model (I believe):
 *
 * https://www.robotshop.com/en/micro-load-cell-5-kg.html
 * 
 * It's interfaced using the Load Cell Amplifier HX711:
 *
 * https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide
 */


#include "orenda.h"


/**
 * Read load cell.  
 * Taken from:
 * https://github.com/bogde/HX711/blob/master/HX711.cpp
 */


/* Nominal readings taken during testing:
 * 
 *  double tare = 30454 - Empty
 *  double full = 81400 - 350ml
 *  So 350ml = 50946, 1ml = 145.56
 * 
 *  Gain values:
 *  1 - channel A, gain factor 128
 *  2 - channel B, gain factor 32
 *  3 - channel C, gain factor 64
 */


static double baseTare;
static double tare;
static double fourFiftyReading;
static unsigned int gain = 3;  // channel A, gain factor 128


// Sample code suggests 5-20 times to get an average reading,
// although we do a median
#define numReads 11


void lcSetup(void) {
  // Begin 
  pinMode(lcCLK, OUTPUT);
  pinMode(lcDAT, INPUT);
  
  if (gain == 1) {
    baseTare         = 29369;
    fourFiftyReading = 73903; 
  } else if (gain == 3) {
    baseTare         = 12983;
    fourFiftyReading = 34740;
  }
  
  tare = baseTare;
  
  // Set_gain
  digitalWrite(lcCLK, LOW);
  lcDirection direction;
  lcRead(direction);
  
  Particle.function("loadCell", loadCell);   	
  Particle.function("setTare", setTareCommand);  
}


static bool lcIsReady(void) {
  return digitalRead(lcDAT) == LOW;
}



static long lcReadSingle(void) {
  // wait for the chip to become ready
  while (!lcIsReady()) {
    // Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
    yield();
  }

  unsigned long value = 0;
  uint8_t data[3] = { 0 };
  uint8_t filler = 0x00;

  // Pulse the clock pin 24 times to read the data
  data[2] = shiftIn(lcDAT, lcCLK, MSBFIRST);
  data[1] = shiftIn(lcDAT, lcCLK, MSBFIRST);
  data[0] = shiftIn(lcDAT, lcCLK, MSBFIRST);

  // set the channel and the gain factor for the next reading using the clock pin
  for (unsigned int i = 0; i < gain; i++) {
    digitalWrite(lcCLK, HIGH);
    digitalWrite(lcCLK, LOW);
  }

  // Replicate the most significant bit to pad out a 32-bit signed integer
  if (data[2] & 0x80) {
    filler = 0xFF;
  } else {
    filler = 0x00;
  }

  // Construct a 32-bit signed integer
  value = (filler) << 24 | ((unsigned long)data[2] << 16) | ((unsigned long)data[1] << 8) | (unsigned long)data[0];

  return (long)value;
}


void lcSetTare(double tareValue) {
  double newTare = (tareValue / 450.0) * (fourFiftyReading - baseTare) + baseTare; 
  
  //value = ((value - tare) * 450.0) / (fourFiftyReading - baseTare); 
  
  
  tare = newTare;
}


int setTareCommand(String command) {
  int value = command.toInt();
   
  lcSetTare(value);
   
  return 1;
}


/** 
 * Take a median of a given number of reads.
 * 
 * This will filter out any occasional bad reads.
 */

static double lcLastValue = 0;

String lcDirectionName(lcDirection direction) {
   switch (direction) {
     case lcDirectionDown:
       return "down";
      
     case lcDirectionEven:
       return "even";
        
     case lcDirectionUp:
       return "up"; 
   }
   
   return "unknown";
}


long lcRead(lcDirection &direction, bool setTare, bool raw) {
  double readings[numReads];
  
  for (int reading = 0; reading < numReads; reading++) {
    double lcValue = lcReadSingle();
    bool shifted = false;

    // Low to high, shift values if this value is less than
    // Current one
    for (int check = 0; check < reading; check++) {
       if (lcValue >= readings[check]) continue;
       
       for (int move = reading; move > check; move--) {
        readings[move] = readings[move - 1];   
       }
       readings[check] = lcValue;
       shifted = true;
       break;
    }
    
    if (!shifted) readings[reading] = lcValue;
  }
  
  int middle = numReads / 2 + 1;  
  // Middle 3 readings
  double value = (readings[middle - 1] + readings[middle] + readings[middle + 1]) / 3;
  
  if (raw) return value;

  // Scale to raw value
  double thresholdScale = (lcThreshold / 450.0) * (fourFiftyReading - baseTare); 
  
  if ((lcLastValue - value) > thresholdScale) {
    // Decreasing
    direction = lcDirectionDown;
    // Reset comparison
    lcLastValue = value;
     
  } else if ((lcLastValue - value) < -thresholdScale) {
    // Increasing 
    direction = lcDirectionUp;
    // Reset comparison
    lcLastValue = value;
    
  } else {
    direction = lcDirectionEven;
  }
  
  
  
  if (setTare) {
    tare = value;
    //Particle.publish("loadCell/tare", String((int)value));
  }
  
  
  
  value = value - tare;
  
  value = (value * 450.0) / (fourFiftyReading - baseTare); 
  //Particle.publish("loadCell", String(round(value * 10) / 10));
  
  return round(value * 10) / 10;  
}



/**
 * Command processing
 */ 

static double loadCell(String command) {
  bool setTare = (command == "tare");
  bool raw = (command == "raw");

  lcDirection direction;  
  return lcRead(direction, setTare, raw);
}





