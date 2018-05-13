// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include "orenda.h"

void yield(void){};


#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel ledStrip(PIXEL_COUNT, ledNP, PIXEL_TYPE);



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
   	
   	pinMode(recirc, OUTPUT);
    Particle.function("recirculate", recircControl);
   	
   	Particle.function("flushWater", flushWater);
   	Particle.function("powerOff", powerOff);   	
   	Particle.function("loadCell", loadCell);   	
   	   
   //	Particle.function("oneWire", oneWireControl);  	

   	   	
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


int recircControl(String command) {
    int power = parsePower(command);
    
    if (power == -1) return -1;
    
    digitalWrite(recirc, !power);
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





int strip(String command) {

    int col = command.toInt();    

	ledStrip.setColor(0, 0xff, 0, 0);
	//ledStrip.setColor(1, 0, 0xff, 0);
	
	ledStrip.setPixelColor(1, col);
	
	ledStrip.show();

    return 0;
}


#if 0
int oneWireControl(String command) {
    OneWire oneWire = OneWire(command.toInt());  // 1-wire signal on pin D4
    
    
/*   unsigned long now = millis();
  // change the 3000(ms) to change the operation frequency
  // better yet, make it a variable!
  if ((now - lastUpdate) < 3000) {
    return;
  }
  lastUpdate = now;*/
  byte i;
  byte present = 0;
  byte addr[8];

  if (!oneWire.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    oneWire.reset_search();
    //delay(250);
    return -1;
  }

  // if we get here we have a valid address in addr[]
  // you can do what you like with it
  // see the Temperature example for one way to use
  // this basic code.

  // this example just identifies a few chip types
  // so first up, lets see what we have found

  // the first ROM byte indicates which chip family
  switch (addr[0]) {
    case 0x10:
      Serial.println("Chip = DS1820/DS18S20 Temp sensor");
      break;
    case 0x28:
      Serial.println("Chip = DS18B20 Temp sensor");
      break;
    case 0x22:
      Serial.println("Chip = DS1822 Temp sensor");
      break;
    case 0x26:
      Serial.println("Chip = DS2438 Smart Batt Monitor");
      break;
    default:
      Serial.println("Device type is unknown.");
      // Just dumping addresses, show them all
      //return;  // uncomment if you only want a known type
  }

  return addr[0];
}

#endif



int getPulse(String command) {
    int min = 1000000;
    int max = 0;
    int total = 0;
    int runs = 64;
    
    pinMode(P1S1, INPUT);
    
    for (int count = 0; count < runs; count++) {
        int value = pulseIn(P1S1, HIGH);
        
        if (value < min) min = value;
        if (value > max) max = value;
        
        total += value;
    }
    
    String message = "min:" + String(min) + " max: " + String(max);
    Particle.publish("TDS", message);
    
    return total / runs;
}










