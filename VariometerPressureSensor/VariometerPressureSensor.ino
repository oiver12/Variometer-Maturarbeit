#include <Dps310.h>

bool hasStarted = false;
float hs;
float startPressure;
float startHeight;
float startTemperature;
float reduzierterLuftdruckStart;
float heightDifferenzes[21];
float times[21];
float lastTime = 0;

const float universalR = 8.3144598;
const float spezificR = 287.053;
const float g = 9.80665;
const float M = 0.0289644;
const float L = 0.0065;

Dps310 Dps310PressureSensor = Dps310();

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  Dps310PressureSensor.begin(Wire);

  Serial.println("Init complete!");
  heightDifferenzes[0] = 0;
  times[0] = 0;
  for (size_t i = 1; i < 21; i++)
  {
	  times[i] = times[i - 1] + 0.05f;
	  heightDifferenzes[i] = times[i] * 3;
	  Serial.print(times[i]);
	  Serial.print("  ");
	  Serial.println(heightDifferenzes[i]);
  }
}



void loop()
{
  float temperature;
  float pressure;
  uint8_t oversampling = 1;
  int16_t ret;

  ret = Dps310PressureSensor.measureTempOnce(temperature, oversampling);

  if (ret != 0)
  {
    //Something went wrong.
    //Look at the library code for more information about return codes
    Serial.print("FAIL! ret = ");
    Serial.println(ret);
    return;
  }

  //Pressure measurement behaves like temperature measurement
  //ret = Dps310PressureSensor.measurePressureOnce(pressure);
  ret = Dps310PressureSensor.measurePressureOnce(pressure, oversampling);
  //Serial.println(millis() - lastTime);
  lastTime = millis();
  if (ret != 0)
  {
    //Something went wrong.
    //Look at the library code for more information about return codes
    Serial.print("FAIL! ret = ");
    Serial.println(ret);
    return;
  }
  else
  {
     if (Serial.available() > 0) 
     {
        // read the incoming byte:
        char incomingByte = Serial.read();
        if(incomingByte == 's')
        {
          startHeight = Serial.parseInt();
          startPressure = pressure;
          startTemperature = temperature;
          hasStarted = true;
          reduzierterLuftdruckStart = pressure / pow(1-L*startHeight / ((temperature+273.15)+L*startHeight), 0.03416/L);
          Serial.println(reduzierterLuftdruckStart);
          Serial.println("Start");
        }
     }
     if(hasStarted)
     {
		 Serial.println(getHeightDifferenz(pressure, temperature, 0));
     }
  }

  //delay(500);
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

float linearRegression()
{

}
