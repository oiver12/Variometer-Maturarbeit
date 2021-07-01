#include "LinearRegression.h"
#include "Variometer.h"
#include <math.h> 

#define	IMPLEMENTATION	FIFO

void Variometer::init(int countMean, float startPressure, float startTemp, float startHeight)
{
	this->countMean = countMean;
	this->startPressure = startPressure;
	this->startTemp = startTemp;
	this->reduzierterLuftdruckStart = startPressure / pow(1 - L * startHeight / ((startTemp + 273.15) + L * startHeight), 0.03416 / L);;
	n = 0;
	meanPressure = 0;
	meanTime = 0;
	lr.reset();
}

void Variometer::addSample(double pressure, double time)
{
	n++;
	meanPressure = meanPressure + pressure;
	meanTime = meanTime + time;
	if (n == countMean)
	{
		meanPressure /= n;
		meanTime /= n;
		double height = getHeightDifferenz(meanPressure, startTemp, 0);
		lr.learn(meanTime, height);
		meanPressure = 0;
		meanTime = 0;
		n = 0;
	}
}

float Variometer::getVelocitySinceLast()
{
	float slope = lr.getSlope();
	lr.reset();
	meanPressure = 0;
	meanTime = 0;
	n = 0;
	return slope;
}

float Variometer::getHeightDifferenz(float pressure, float temperature, int method)
{
	if (method == 0)
	{
		//https://en.wikipedia.org/wiki/Vertical_pressure_variation
		//veränderung in Temp mit einberechnet
		return ((startTemp + 273.15) / L)*(pow(pressure / startPressure, -L * spezificR / g) - 1);
	}
	else if (method == 1)
	{
		//nicht einberechnet
		return -(spezificR*(temperature + 273.15) / g)*log(pressure / startPressure);
	}
	else if (method == 2)
	{
		//https://en.wikipedia.org/wiki/Barometric_formula#:~:text=The%20barometric%20formula%2C%20sometimes%20called,1000%20meters%20above%20sea%20level.
		float hs = (universalR*(temperature + 273.15)) / (M*g);
		return -log(pressure / startPressure)*hs;
	}
	else
	{
		return (float)44330 * (1 - pow(((float)pressure / startPressure), 0.190295));
	}
}

