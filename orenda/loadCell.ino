

/**
 * The load cell in the Orenda is this model:
 *
 * https://www.robotshop.com/en/micro-load-cell-5-kg.html
 * 
 * It's interfaced using the Load Cell Amplifier HX711:
 *
 *  https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide
 */


#include "orenda.h"


/**
 * Read load cell.  
 * Taken from:
 * https://github.com/bogde/HX711/blob/master/HX711.cpp
 */


/* Nominal readings taken during testing:
 * 
 *  double tare = 30454; // Empty
 *  double full = 81400; // 350ml
 *  So 350ml = 50946, 1ml = 145.56
 */
 

static double tare              = 30545;
static double threeFiftyReading = 50946;
static int gain = 1;  // channel A, gain factor 128

// Sample code suggests 5-20 times to get an average reading
#define averageTimes 10


void lcSetup(void) {
    // Begin 
    pinMode(lcCLK, OUTPUT);
    pinMode(lcDAT, INPUT);
    
    // Set_gain
    digitalWrite(lcCLK, LOW);
    lcRead();
    
    Particle.function("loadCell", loadCell);   	
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



long lcRead(bool setTare, bool raw) {
    double total = 0;
    
    for (int reading = 0; reading < averageTimes; reading++) {
        total += lcRead();
    }
    
    double value = total / averageTimes;
    
    if (raw) return value;
    
    if (setTare) {
        tare = value;
      //Particle.publish("loadCell/tare", String((int)value));
    }
    
    value = value - tare;
    
    value = (value * 350.0) / threeFiftyReading; 
    //Particle.publish("loadCell", String(round(value * 10) / 10));
    
    return round(value * 10) / 10;  
}




/**
 * Command processing
 */ 

static double loadCell(String command) {
    bool setTare = (command == "tare");
    bool raw = (command == "raw");
    
    return lcRead(setTare, raw);
}





