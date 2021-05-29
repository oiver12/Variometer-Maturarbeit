//#include "LinearRegression.h"
#include <Dps310.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

const int lengthLastDates = 25;

bool hasStarted = false;
float hs;
float startPressure;
float startHeight;
float startTemperature;
float reduzierterLuftdruckStart;
//float heightDifferenzes[lengthLastDates];
//float times[lengthLastDates];
float lastTime = 0;
float lastPressure = 0;
float pressure;
float temperature;

const float universalR = 8.3144598;
const float spezificR = 287.053;
const float g = 9.80665;
const float M = 0.0289644;
const float L = 0.0065;

File myFile;
SoftwareSerial BTserial(2, 3); // RX | TX

// change this to match your SD shield or module;
const int chipSelect = 10;

//LinearRegression lr = LinearRegression();
Dps310 Dps310PressureSensor = Dps310();

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Start");
  Dps310PressureSensor.begin(Wire);

  Serial.print("Initializing SD card...");

  if (!SD.begin())
  {
	  Serial.println("initialization failed!");
	  return;
  }
  Serial.println("Init complete!");
  //delete and remake again to overwrite last flight
  SD.remove("data.txt");
  myFile = SD.open("data.txt", FILE_WRITE);
  BTserial.begin(9600);
  //heightDifferenzes[0] = 0;
  //times[0] = 0;
  //lr.reset();
  //lr.learn(0, 0);
  //pressure = ReadPressure();
  //uint16_t ret = Dps310PressureSensor.measureTempOnce(temperature);
  //for (size_t i = 0; i < lengthLastDates; i++)
  //{
	 // times[i] = millis();
	 // heightDifferenzes[i] = 0;
	 // //times[i] = times[i - 1] + 0.05f;
	 // //heightDifferenzes[i] = times[i] * 3;
	 // //Serial.print(times[i]);
	 // //Serial.print("  ");
	 // //Serial.println(heightDifferenzes[i]);
	 // //lr.learn(times[i], heightDifferenzes[i]);
  //}
}



void loop()
{
	//float temperature;
  //ret = Dps310PressureSensor.measurePressureOnce(pressure, oversampling);
  //Serial.println(millis() - lastTime);
  //lastTime = millis();
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
    			temperature = readTemperatur(7);
    			startHeight = BTserial.parseInt();
    			startPressure = ReadPressure(7);
    			startTemperature = temperature;
    			hasStarted = true;
    			reduzierterLuftdruckStart = startPressure / pow(1-L*startHeight / ((temperature+273.15)+L*startHeight), 0.03416/L);
         if(myFile)
         {
            myFile.println("Start " + String(millis()) + ";" + String(startPressure) + ";" + String(startHeight) + ";" + String(startTemperature));
      			Serial.println("Start mit reduziertem Luftdruck von: " + String(reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure));
      			BTserial.println("Start mit reduziertem Luftdruck von: " + String(reduzierterLuftdruckStart) + "Luftdruck von: " + String(startPressure) + "Temp: " + String(startTemperature));
      			lastTime = millis();
      			lastPressure = pressure;
         }
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
  				 myFile.close();
  				 BTserial.println("done.");
  				 return;
  			 }
         if(incomingChar == 'f')
         {
          BTserial.println("Lenght of file: " + String(myFile.size()));
         }
  		 }
  		 if (myFile)
  		 {
  			 //Serial.println(String(millis() - lastTime) + ';' + String(pressure - lastPressure));
  		   myFile.println(String(millis() - lastTime) + ';' + String(pressure - lastPressure));
  			 lastTime = millis();
  			 lastPressure = pressure;
  		 }
  		 else
  		 {
  			 BTserial.println("error opening data.txt");
  		 }
  		 /*lr.reset();
  		 for (size_t i = lengthLastDates-2; i <= 0; i--)
  		 {
  			 heightDifferenzes[i + 1] = heightDifferenzes[i];
  			 times[i + 1] = times[i];
  			 lr.learn(times[i+1], heightDifferenzes[i+1]);
  		 }
  		 heightDifferenzes[0] = getHeightDifferenz(pressure, temperature, 0);
  		 times[0] = millis();
  		 lr.learn(times[0], heightDifferenzes[0]);
  		 Serial.print(String(times[0]) + String(heightDifferenzes[0]));
  		 Serial.println(lr.getSlope());*/
  		 //Serial.println(getHeightDifferenz(pressure, temperature, 0));
     }
  }

  //delay(500);
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

float getHeightDifferenz(float pressure, float temperature, int method)
{
  if(method == 0)
  {
    //https://en.wikipedia.org/wiki/Vertical_pressure_variation
    //veränderung in Temp mit einberechnet
    return ((startTemperature+273.15)/L)*(pow(pressure/startPressure, -L*spezificR/g) -1);
  }
  else if(method == 1)
  {
    //nicht einberechnet
    return -(spezificR*(temperature+273.15)/g)*log(pressure/startPressure);
  }
  else if(method == 2)
  {
    //https://en.wikipedia.org/wiki/Barometric_formula#:~:text=The%20barometric%20formula%2C%20sometimes%20called,1000%20meters%20above%20sea%20level.
    hs = (universalR*(temperature + 273.15))/(M*g);
    return -log(pressure/startPressure)*hs;
  }
  else
  {
     return (float)44330 * (1 - pow(((float) pressure/startPressure), 0.190295));
  }
}
