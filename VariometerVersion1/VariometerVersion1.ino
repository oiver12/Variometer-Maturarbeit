#include <Dps310.h>
#include <SoftwareSerial.h>
#include "LinearRegression.h"
#include "Variometer.h"
#include "FiFo.h"

//#define logSDCard

#ifdef logSDCard
#include <SD.h>
#include <SPI.h>
File myFile;
const int chipSelect = 10;
int frequencyLog = 1; //all so und so Frame
float lastTimeSDCard = 0;
float lastPressureSDCard;
#endif

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
  up1 = 2,
  up2 = 3,
  up3 = 4,
  up4 = 5,
};
class Tonestate{
  public:
    biepstate state;
    float frequency;
    //Achtung duration = modulaCount --> kleiner und es werden mehr Töne pro Sekunde gespielt
    int duration;
    float lowerVelocity;
    float upVelocity;
};
Tonestate ToneArray[6] = {
  {biepstate::low, 120, -1, -999, -2.5f},
  {biepstate::equal, 0, 0, -2.5f, 0.1f},
  {biepstate::up1, 700, 25, 0.1f, 1.5f},
  {biepstate::up2, 850, 18, 1.5f, 3.5f},
  {biepstate::up3, 1000, 10, 3.5f, 10},
  {biepstate::up4, 1600, 3, 10, 999},
};
int indexCurrentToneArray = 1;

float frequencyNow = 0;
int durationNow = 0;
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
	Dps310PressureSensor.begin(Wire);
	#ifdef logSDCard
	if (!SD.begin())
  	{
	  return;
  	}
	SD.remove("data.txt");
  	myFile = SD.open("data.txt", FILE_WRITE);
	#endif
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
     if (BTserial.available() > 0) 
	 //if(Serial.available() > 0)
     {
		//Serial.println("Here");
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
      		BTserial.println("Start mit reduziertem Luftdruck von: " + String(variometer.reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure) + "Temp: " + String(lastTemp));
			#ifdef logSDCard
			if(myFile)
			{
				myFile.println("Start " + String(millis()) + ";" + String(startPressure) + ";" + String(startHeight) + ";" + String(lastTemp));
			}
			#endif
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
			lastTemp = readTemperatur(3);
			lastVelocity = variometer.getVelocitySinceLast();
			setNewBeep();
			Serial.println(String(lastVelocity));
			BTserial.println(String(lastVelocity));
			lastTimeVelocity = millis();
			if(millis() - lastTimeSet >= baseSetRate)
			{
				lastTemp = readTemperatur(7);
				lastPressure = ReadPressure(7);
				variometer.setNewBase(lastPressure, lastTemp);
				lastTimeSet = millis();
				Serial.println("New Base Set");
			}
			#ifdef logSDCard
			if(myFile)
			{
				myFile.println("v" + String(lastVelocity) + ";" + String(lastTemp) + ";" + String(frequencyNow)+ ";" + String(durationNow));
			}
			#endif
		 }
		variometer.addSample(lastPressure, millis()/static_cast<double>(1000));
		if(ToneArray[indexCurrentToneArray].state != biepstate::low && ToneArray[indexCurrentToneArray].state != biepstate::equal && count%durationNow == 0)
		{
			tone(8, frequencyNow, (deltaTimeFrame*durationNow)/2);
		}
		#ifdef logSDCard
		if(myFile && count%frequencyLog == 0)
		{
			 myFile.println(String(millis() - lastTimeSDCard) + ';' + String(lastPressure - lastPressureSDCard));
  			 lastTimeSDCard = millis();
  			 lastPressureSDCard = lastPressure;
		}
		#endif
		count++;
     }
  }
}

void setNewBeep()
{
  int lengthTonestate = sizeof(ToneArray)/sizeof(Tonestate);
  for (size_t i = 0; i < lengthTonestate; i++)
  {
    if(lastVelocity >= ToneArray[i].lowerVelocity && lastVelocity <= ToneArray[i].upVelocity)
    {
      indexCurrentToneArray = i;
      if((lengthTonestate - 1) == i)
      {
        frequencyNow = ToneArray[indexCurrentToneArray].frequency;
        durationNow = ToneArray[indexCurrentToneArray].duration;
      }
      else
      {
        float factor = (lastVelocity - ToneArray[indexCurrentToneArray].lowerVelocity)/(ToneArray[indexCurrentToneArray].upVelocity-ToneArray[indexCurrentToneArray].lowerVelocity);
        frequencyNow = factor * (ToneArray[i+1].frequency - ToneArray[indexCurrentToneArray].frequency) + ToneArray[indexCurrentToneArray].frequency;
        durationNow = factor * (ToneArray[i+1].duration - ToneArray[indexCurrentToneArray].duration) + ToneArray[indexCurrentToneArray].duration;
      }
    }
  }
  if(ToneArray[indexCurrentToneArray].state == biepstate::low)
      tone(8, ToneArray[indexCurrentToneArray].frequency);
    if(ToneArray[indexCurrentToneArray].state == biepstate::equal)
      noTone(8);
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

