#include "LinearRegression.h"
#include "Variometer.h"
#include <Arduino.h>

//Stat des Variometers redzierter Luftdruck wird ausgerechent
void Variometer::init(int countMean, float startPressure, float startTemp, float startHeight)
{
	this->startPressure = startPressure;
	this->countMean = countMean;
	this->basePressure = startPressure;
	this->baseTemp = startTemp;
	this->reduzierterLuftdruckStart = startPressure / pow(1 - L * startHeight / ((startTemp + 273.15) + L * startHeight), 0.03416 / L);;
	n = 0;
	meanPressureSum = 0;
	meanTimeSum = 0;
	lr.reset();
}

void Variometer::addSample(double pressure, double time)
{
	if (gleitenderDurchschnitt)
	{
		//die Summe wird der letzten Messungen wird gespeichert, die neuste Messung wird zur Summe hinzugefügt,
		//die älteste Messung wird vom Fifo genommen und von der Summe abgezogen
		meanPressureSum += pressure;
		meanTimeSum += time;
		if (lastPressures.isFull())
		{
			//Höhendifferenz ausrechnen und zur Linearen Regression hinzufügen -->lr.learn
			float lastPressure = 0.0;
			float lastTime = 0.0;
			lastPressures.Get(lastPressure);
			lastTimes.Get(lastTime);
			meanPressureSum -= lastPressure;
			meanTimeSum -= lastTime;
			float meanPressure = meanPressureSum / lengthGeleitenderDurchschnitt;
			float meanTime = meanTimeSum / lengthGeleitenderDurchschnitt;
			float height = getHeightDifferenz(meanPressure, baseTemp, 0);

			lr.addPoint(meanTime, height);
		}
		//ältestes Resultat im FiFo überschreiben
		lastPressures.PutOver(pressure);
		lastTimes.PutOver(time);
	}
	else
	{
		//bei Durchschnitt die Summe der letzten Messungen nehmen und dann wieder neu anfangen
		n++;
		meanPressureSum = meanPressureSum + pressure;
		meanTimeSum = meanTimeSum + time;
		if (n == countMean)
		{
			meanPressureSum /= n;
			meanTimeSum /= n;
			double height = getHeightDifferenz(meanPressureSum, baseTemp, 0);
			lr.addPoint(meanTimeSum, height);
			meanPressureSum = 0;
			meanTimeSum = 0;
			n = 0;
		}
	}
}

//zum Debuggen
LinearRegression Variometer::getLinearRegression()
{
	return lr;
}

float Variometer::getVelocitySinceLast()
{
	//bekommen der Geschwindigkeit durch die lineare Regression und dann wieder alles zurücksetzten,
	//dass neu angefangen werden kann
	float slope = lr.getSlope();
	lr.reset();
	meanPressureSum = 0;
	meanTimeSum = 0;
	n = 0;
	lastPressures.Reset();
	lastTimes.Reset();
	return slope;
}

void Variometer::setNewBase(double newPressure, double newTemp)
{
	//neue Basis setzten
	basePressure = newPressure;
	baseTemp = newTemp;
}

//Höhendifferenz zur Baishöhe bekommen --> Methode 0 benutzt
float Variometer::getHeightDifferenz(float pressure, float temperature, int method)
{
	if (method == 0)
	{
		//https://en.wikipedia.org/wiki/Vertical_pressure_variation
		//veränderung in Temp mit einberechnet
		return ((baseTemp + 273.15) / L)*(pow(pressure / basePressure, -L * spezificR / g) - 1);
	}
	else if (method == 1)
	{
		//nicht einberechnet
		return -(spezificR*(temperature + 273.15) / g)*log(pressure / basePressure);
	}
	else if (method == 2)
	{
		//https://en.wikipedia.org/wiki/Barometric_formula#:~:text=The%20barometric%20formula%2C%20sometimes%20called,1000%20meters%20above%20sea%20level.
		float hs = (universalR*(temperature + 273.15)) / (M*g);
		return -log(pressure / basePressure)*hs;
	}
	else
	{
		return (float)44330 * (1 - pow(((float)pressure / basePressure), 0.190295));
	}
}

