#include <Dps310.h>
#include <SoftwareSerial.h>
#include "LinearRegression.h"
#include "Variometer.h"
#include "FiFo.h"

const int lengthLastDates = 25;

bool hasStarted = false;
float startHeight;
float lastTimeVelocity = 0;
double lastPressure;
double lastTemp;
double lastTimeSet;
double baseSetRate = 20000;
float velocityMeasureRate = 1000;

enum biepstate
{
  low = 0,
  equal = 1,
  up = 2,
  upup = 3,
  upupup = 4,
};
struct Tonestate{
  biepstate state;
  float frequency;
  //Achtung duration = modulaCount --> kleiner und es werden mehr Töne pro Sekunde gespielt
  int duration;
  float lowerVelocity;
  float upVelocity;
};
Tonestate ToneArray[5] = {
  {biepstate::low, 120, -1, -999, -2.5f},
  {biepstate::equal, 0, 0, -2.5f, 0},
  {biepstate::up, 800, 15, 0, 1.5f},
  {biepstate::upup, 900, 12, 1.5f, 3.5f},
  {biepstate::upupup, 1010, 8, 3.5f, 999},
};
Tonestate state = ToneArray[1];
int deltaTimeFrame = 30;
unsigned int count = 0;

float lastVelocity = 0.0f; //in m/s

SoftwareSerial BTserial(2, 3); // RX | TX


Variometer variometer = Variometer();
Dps310 Dps310PressureSensor = Dps310();

void setup()
{
	Serial.begin(115200);
	while (!Serial);
	Serial.println("Start");
	Dps310PressureSensor.begin(Wire);

	Serial.print("Initializing SD card...");

	Serial.println("Init complete!");
	BTserial.begin(9600);
}



void loop()
{
  lastPressure = ReadPressure(1);
  if (lastPressure == 0)
  {
	  return;
  }
  else
  {
     if (BTserial.available() > 0) //if(Serial.available() > 0)
     {
		 Serial.println("Here");
        // read the incoming byte:
        char incomingByte = (char)BTserial.read();
		//char incomingByte = (char)Serial.read();
        if(incomingByte == 's')
        {
    		lastTemp = readTemperatur(10);
    		startHeight = BTserial.parseInt();
			//startHeight = Serial.parseFloat();
    		float startPressure = ReadPressure(10);
    		hasStarted = true;
			variometer.init(1, startPressure, lastTemp, startHeight);
      		Serial.println("Start mit reduziertem Luftdruck von: " + String(variometer.reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure));
      		BTserial.println("Start mit reduziertem Luftdruck von: " + String(variometer.reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure) + "Temp: " + String(lastTemp));
        }
     }
     if(hasStarted)
     {	
  		if (BTserial.available() > 0)
		//if(Serial.available() > 0)
  		 {
  			char incomingChar = (char)BTserial.read();
			//char incomingChar = (char)Serial.read();
  			 Serial.println(incomingChar);
  			 if (incomingChar == 'c')
  			 {
  				 hasStarted = false;
  				 BTserial.println("done.");
  				 return;
  			}
  		 }
		 if(millis() - lastTimeVelocity >= velocityMeasureRate)
		 {
			//Serial.println("n:" + String(variometer.getLinearRegression().getN()) + " sumMeanX:" + String(variometer.getLinearRegression().getSumMeanX()) + " sumMeanY:" + String(variometer.getLinearRegression().getSumMeanY()) + " sumMeanXY: " + String(variometer.getLinearRegression().getSumMeanXY()) + " sumMeanXYSq: " + String(variometer.getLinearRegression().getSumMeanXYSq()) + " m: " + String(variometer.getLinearRegression().getSlope()));
			/*for (size_t i = 0; i <	5; i++)
			{
				Serial.print(String(variometer.getLinearRegression().bufferX[i]) + ";");
			}
			Serial.print("\n");*/
			 //Serial.println(testSecond);
			 //Serial.println(millis() - lastTimeVelocity);
			lastTemp = readTemperatur(3);
			lastVelocity = variometer.getVelocitySinceLast();
			
			if(!(lastVelocity >= state.lowerVelocity && lastVelocity <= state.upVelocity))
			{
				for (size_t i = 0; i < sizeof(ToneArray)/sizeof(Tonestate); i++)
				{ 
					if(lastVelocity >= ToneArray[i].lowerVelocity && lastVelocity <= ToneArray[i].upVelocity)
					{
						state = ToneArray[i];
						Serial.println(String(state.state));
					}
				}
				if(state.state == biepstate::low)
					tone(8, state.frequency);
				if(state.state == biepstate::equal)
					noTone(8);
			}
			bool first = lastVelocity >= state.lowerVelocity;
			bool second = lastVelocity <= state.upVelocity;
			Serial.println(String(lastVelocity) + "  " + String(state.lowerVelocity) + "  " + String(state.upVelocity) + "  " + String(first) + "  " + String(second) + " " + String(first && second));
			lastTimeVelocity = millis();
			if(millis() - lastTimeSet >= baseSetRate)
			{
				lastTemp = readTemperatur(7);
				lastPressure = ReadPressure(7);
				variometer.setNewBase(lastPressure, lastTemp);
				lastTimeSet = millis();
				Serial.println("New Base Set");
			}
		 }
		variometer.addSample(lastPressure, millis()/static_cast<double>(1000));
		if(state.state != biepstate::low && state.state != biepstate::equal && count%state.duration == 0)
		{
			tone(8, state.frequency, (deltaTimeFrame*state.duration)/2);
		}
		count++;
     }
  }
}

float ReadPressure(int oversampling)
{
	float pres;
	//uint8_t oversampling = 1;
	int16_t ret;
	ret = Dps310PressureSensor.measurePressureOnce(pres, oversampling);
	if (ret != 0)
	{
		//Something went wrong.
		//Look at the library code for more information about return codes
		Serial.print("FAIL! ret = ");
		Serial.println(ret);
		return 0;
	}
	return pres;
}

float readTemperatur(int oversampling)
{
	float temp;
	//uint8_t oversampling = 1;
	int16_t ret;
	ret = Dps310PressureSensor.measureTempOnce(temp, oversampling);
	if (ret != 0)
	{
		//Something went wrong.
		//Look at the library code for more information about return codes
		Serial.print("FAIL! ret = ");
		Serial.println(ret);
		return 0;
	}
	return temp;
}

