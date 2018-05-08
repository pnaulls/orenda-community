// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>


typedef enum {
   fanPower     = 0,
   recirc       = 1,   // Relay 1
   brewButton   = 2,
   relay2       = 4,   // Relay 2 - Relates to pump1.
   
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
   analog4     = 25,
   grinder     = 26,  // Grinder motor
   pump3       = 27,  // Dispense or recirculate
   
   lcCLK       = 28,  // Load cell clock
   lcDAT       = 29,  // Load cell data
 
} orendaPins;


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);



void yield(void) {};


/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);

double tempReservoir;
double tempCirculate;


/* This function is called once at start up ----------------------------------*/
void setup()
{
    pinMode(fanPower, OUTPUT);
   	Particle.function("fan", fanControl);
   	
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
   	
   	Particle.function("flushWater", flushWater);
   	   	
   	Particle.function("powerOff", powerOff);   	
   	   	
   	Particle.function("loadCell", loadCell);   	
   	   	
   	//pinMode(ledR, OUTPUT);
   	//pinMode(ledG, OUTPUT);
   	//pinMode(ledB, OUTPUT);
   	//Particle.function("led", led);
   	
   	//Particle.function("setNum", setNum);   	
   	
    Particle.function("strip", strip);  
    Particle.function("getPulse", getPulse);      

	//Register all the Tinker functions
	Particle.function("digitalread", tinkerDigitalRead);
	Particle.function("digitalwrite", tinkerDigitalWrite);
	Particle.function("analogread", tinkerAnalogRead);
	Particle.function("analogwrite", tinkerAnalogWrite);
	
	Particle.variable("tempRes", tempReservoir);
	Particle.variable("tempCir", tempCirculate);
	
	lcSetup();

    ledStrip.begin();
    ledStrip.show();
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
	//This will run in a loop
	
	tempReservoir = readTemp(tempRes);
	tempCirculate = readTemp(tempCir);
	
	delay(2000);
}

static int parsePower(String power) {
    if (power == "0") return 0;
    if (power == "1") return 1;
    
    String lower = power.toLowerCase();
    
    if (lower == "low"  || lower == "off") return 0;
    if (lower == "high" || lower == "on")  return 1;
    
    return -1;
}



/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber > 9) return -1;

	if (pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	else if (pin.startsWith("L"))
	{
		pinMode(pinNumber+20, INPUT_PULLDOWN);
		return digitalRead(pinNumber+20);
	}
	else if (pin.startsWith("S"))
	{
		pinMode(pinNumber+24, INPUT_PULLDOWN);
		return digitalRead(pinNumber+24);
	}
	else if (pin.startsWith("X"))
	{
		pinMode(pinNumber+34, INPUT_PULLDOWN);
		return digitalRead(pinNumber+34);
	}
	
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	/*bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber > 9) return -1;

	if (command.substring(3,7) == "HIGH" || command.substring(3,3) == "0") value = 1;
	else if (command.substring(3,6) == "LOW" || command.substring(3,3) == "1") value = 0;
	else return -2;*/

    int comma = command.indexOf(",");
    
    if (comma == -1) return -1;
    
    String name = command.substring(0, comma);
    String svalue = command.substring(comma + 1);

    int value = parsePower(svalue);
    
    if (value == -1) return -3;
        
     int pinNumber = name.charAt(1) - '0';   
        
 
	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}	
	else if(command.startsWith("L"))
	{
		pinMode(pinNumber+20, OUTPUT);
		digitalWrite(pinNumber+20, value);
		return 1;
	}
	else if(command.startsWith("T"))
	{
		pinMode(pinNumber + 18, OUTPUT);
		digitalWrite(pinNumber + 18, value);
		return 1;
	}
	else if(command.startsWith("S") /*&& pinNumber < 7*/)
	{
		pinMode(pinNumber+24, OUTPUT);
		digitalWrite(pinNumber+24, value);
		return 1;
	}	
	else if(command.startsWith("X") /*&& pinNumber < 7*/)
	{
		pinMode(pinNumber+34, OUTPUT);
		digitalWrite(pinNumber+34, value);
		return 1;
	}

	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		return -3;
	}
	else if (pin.startsWith("A"))
	{
		return analogRead(pinNumber+10);
	}
	else if(pin.startsWith("T"))
	{
        return analogRead(pinNumber+18);
	}
	
	else if(pin.startsWith("S"))
	{
		return analogRead(pinNumber+24);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else if(command.startsWith("S"))
	{
		pinMode(pinNumber+24, OUTPUT);
		analogWrite(pinNumber+24, value.toInt());
		return 1;
	}
	
	else return -2;
}


int setNum(String command) {
    int num = command.toInt();
    
    pinMode(3, OUTPUT);
	digitalWrite(3, (num & 0x1) != 0);
    
    pinMode(5, OUTPUT);
	digitalWrite(5, (num & 0x2) != 0);
	
	pinMode(6, OUTPUT);
	digitalWrite(6, (num & 0x4) != 0);
	
	pinMode(7, OUTPUT);
	digitalWrite(7, (num & 0x8) != 0);
    
    return 1;
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
     digitalWrite(recirc, 1);  // Recirculate off
     digitalWrite(relay2, 1);  // Pump 1 to output
     
     digitalWrite(pump1, power);
     digitalWrite(pump2, power);
     digitalWrite(pump3, power);
     
     return 1;
 }


int pumpControl(String command) {
    int comma = command.indexOf(",");
    
    if (comma == -1) return -1;
    
    String name = command.substring(0, comma);
    String value = command.substring(comma + 1);
    
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
        
    digitalWrite(pumpNum, power);
    
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


void lcSetup(void) {
    // Begin 
    pinMode(lcCLK, OUTPUT);
    pinMode(lcDAT, INPUT);
    
    // set_gain
    digitalWrite(lcCLK, LOW);
    lcRead();
}


bool lcIsReady(void) {
	return digitalRead(lcDAT) == LOW;
}


/**
 * Read load cell.  
 * Taken from:
 * https://github.com/bogde/HX711/blob/master/HX711.cpp
 */

long lcRead(void) {
	// wait for the chip to become ready
	while (!lcIsReady()) {
		// Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
		yield();
	}

    int GAIN = 1;

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn(lcDAT, lcCLK, MSBFIRST);
	data[1] = shiftIn(lcDAT, lcCLK, MSBFIRST);
	data[0] = shiftIn(lcDAT, lcCLK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
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


int loadCell(String command) {
    
    double tare = 30454; // Empty
    double full = 81400; // 350ml
    
    double value = lcRead();
    
    if (command == "raw") return value;
    
    value = value - tare;
    
    value = value / (full - tare) * 350;
    
    return value;
}


int strip(String command) {

    int col = command.toInt();    

	ledStrip.setColor(0, 0xff, 0, 0);
	//ledStrip.setColor(1, 0, 0xff, 0);
	
	ledStrip.setPixelColor(1, col);
	
	ledStrip.show();

    return 0;
}


int getPulse(String command) {
    pinMode(P1S1, INPUT);
    
    return pulseIn(P1S1, LOW);
}








