#include <Dps310.h>
#include <SoftwareSerial.h>
#include "LinearRegression.h"
#include "Variometer.h"

const int lengthLastDates = 25;

bool hasStarted = false;
float startHeight;
float lastTimeTemp = 0;
float lastTimeVelocity = 0;
float pressure;
float temperature;
float tempMeasureRate = 1000;
float velocityMeasureRate = 500;

float testTimes[31] = {
	173.0039978f,
	173.0209961f,
	173.0390015f,
	173.0559998f,
	173.0740051f,
	173.0910034f,
	173.1080017f,
	173.1260071f,
	173.1419983f,
	173.1600037f,
	173.177002f,
	173.1950073f,
	173.2120056f,
	173.2290039f,
	173.2460022f,
	173.2630005f,
	173.2810059f,
	173.2980042f,
	173.3150024f,
	173.3329926f,
	173.348999f,
	173.3670044f,
	173.3840027f,
	173.401001f,
	173.4190063f,
	173.4360046f,
	173.4539948f,
	173.470993f,
	173.4889984f,
	173.5059967f,
	173.522995f,
	};
float testPressures[31] = {
	82577.09375f,
	82578.04688f,
	82576.75781f,
	82570.57031f,
	82572,
	82576.6875f,
	82579.875f,
	82575.125f,
	82570.64844f,
	82570.10938f,
	82574.72656f,
	82570.25f,
	82571.67188f,
	82573.03125f,
	82570.25f,
	82574.25781f,
	82573.51563f,
	82568.55469f,
	82571.13281f,
	82572.49219f,
	82573.71875f,
	82573.03125f,
	82572.89844f,
	82572.49219f,
	82568.55469f,
	82571.27344f,
	82571,
	82570.11719f,
	82570.58594f,
	82571.0625f,
	82570.99219f,
};


SoftwareSerial BTserial(2, 3); // RX | TX


Variometer variometer = Variometer();
Dps310 Dps310PressureSensor = Dps310();

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Start");
  Dps310PressureSensor.begin(Wire);

  Serial.print("Initializing SD card...");

  Serial.println("Init complete!");
  BTserial.begin(9600);

  /*variometer.init(1, 82216.38f, 25.33f, 1800);
  Serial.println(variometer.reduzierterLuftdruckStart);
  for (size_t i = 0; i < 31; i++)
  {
	  variometer.addSample(testPressures[i], testTimes[i]);
  }
  Serial.println("Velocity: " + String(variometer.getVelocitySinceLast()));*/
}



void loop()
{
  pressure = ReadPressure(1);
  if (pressure == 0)
  {
	  return;
  }
  else
  {
     if (BTserial.available() > 0) 
     {
        // read the incoming byte:
        char incomingByte = (char)BTserial.read();
        if(incomingByte == 's')
        {
    		temperature = readTemperatur(10);
    		startHeight = BTserial.parseInt();
    		float startPressure = ReadPressure(10);
    		hasStarted = true;
			variometer.init(5, startPressure, temperature, startHeight);
      		Serial.println("Start mit reduziertem Luftdruck von: " + String(variometer.reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure));
      		BTserial.println("Start mit reduziertem Luftdruck von: " + String(variometer.reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure) + "Temp: " + String(temperature));
			lastTimeTemp = millis();
        }
     }
     if(hasStarted)
     {	
  		 if (BTserial.available() > 0)
  		 {
  			 char incomingChar = (char)BTserial.read();
  			 Serial.println(incomingChar);
  			 if (incomingChar == 'c')
  			 {
  				 hasStarted = false;
  				 BTserial.println("done.");
  				 return;
  			}
  		 }
		 if (millis() - lastTimeTemp >= tempMeasureRate)
		 {
			 float temp = readTemperatur(3);
			 Serial.println("Temperature: " + String(temp));
			 lastTimeTemp = millis();
		 }
		 if(millis() - lastTimeVelocity >= velocityMeasureRate)
		 {
			 variometer.getVelocitySinceLast();
			 lastTimeVelocity = millis();
		 }
		 variometer.addSample(ReadPressure(1), millis()/1000);
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

