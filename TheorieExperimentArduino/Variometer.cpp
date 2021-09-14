#include "LinearRegression.h"
#include "Variometer.h"
#include <math.h>
#include <Arduino.h>

void Variometer::addSample(double hoehe, double time)
{
	if (gleitenderDurchschnitt)
	{
		//die Summe wird der letzten Messungen wird gespeichert, die neuste Messung wird zur Summe hinzugefügt,
		//die älteste Messung wird vom Fifo genommen und von der Summe abgezogen
		meanPressureSum += hoehe;
		meanTimeSum += time;
		if (lastPressures.isFull())
		{
			//Höhendifferenz ausrechnen und zur Linearen Regression hinzufügen -->lr.addPoint
			float lastPressure = 0.0;
			float lastTime = 0.0;
			lastPressures.Get(lastPressure);
			lastTimes.Get(lastTime);
			meanPressureSum -= lastPressure;
			meanTimeSum -= lastTime;
			float meanPressure = meanPressureSum / lengthGeleitenderDurchschnitt;
			float meanTime = meanTimeSum / lengthGeleitenderDurchschnitt;
			//float height = getHeightDifferenz(meanPressure, baseTemp, 0);
			float height = meanPressure;

			lr.addPoint(meanTime, height);

			allPressures.PutOver(height);
			allTimes.PutOver(meanTime);


			float velocity = lr.getSlope();
			if(isfinite(velocity) == 0)
			{
				velocity = 0;
			}
			Serial.print(String(velocity, 7) + ", ");
			if (allPressures.isFull())
			{
				lr.reset();
				float pressureLr = 0;
				float timelLr = 0;
				allPressures.GetHeadPosition();
				allTimes.GetHeadPosition();
				for (size_t i = 0; i < lengthLinearRegression; i++)
				{
					allPressures.GetNext(pressureLr, i==0);
					allTimes.GetNext(timelLr, i==0);
					lr.addPoint(timelLr, pressureLr);
				}
			}
			else
			{
				Serial.print("0, ");
			}
		}
		else
		 {
			 Serial.print("0, ");
		 }
		//ältestes Resultat im FiFo überschreiben
		lastPressures.PutOver(hoehe);
		lastTimes.PutOver(time);
	}
	else
	{
		//bei Durchschnitt die Summe der letzten Messungen nehmen und dann wieder neu anfangen
		n++;
		meanPressureSum = meanPressureSum + hoehe;
		meanTimeSum = meanTimeSum + time;
		if (n == countMean)
		{
			meanPressureSum /= n;
			meanTimeSum /= n;
			double height = meanPressureSum;
			//double height = getHeightDifferenz(meanPressureSum, baseTemp, 0);
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


