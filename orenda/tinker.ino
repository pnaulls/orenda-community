
/** 
 * Debug Tinker firmware funtions. 
 * 
 * These are retained for the moment for debug purposes for looking
 * at unassigned pins, but could be disabled during the build. 
 */

#include "orenda.h"

/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
static int tinkerDigitalRead(String pin)
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

	
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
static int tinkerDigitalWrite(String command)
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
static int tinkerAnalogRead(String pin)
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
static int tinkerAnalogWrite(String command)
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


void tinkerSetup(void) {
    Particle.function("digitalread",  tinkerDigitalRead);
    Particle.function("digitalwrite", tinkerDigitalWrite);
    Particle.function("analogread",   tinkerAnalogRead);
    Particle.function("analogwrite",  tinkerAnalogWrite);
}




