#ifndef Variometer_h
#define Variometer_h

#include "LinearRegression.h"
#include "FiFo.h"

constexpr int lengthGeleitenderDurchschnitt = 5;

//Klasse um Durchschnitt bzw Gleitender Druchschnitt auszurechnen
class Variometer {
public:
	//gleitender oder normaler Durchschnitt (gleitender genauer)
	bool gleitenderDurchschnitt = true;
	//reduzierter Luftdruck, welcher mit der Starthöhe ausgerechent wird
	float reduzierterLuftdruckStart;
	//Start des Variometers
	void init(int countMean, float startPressure, float startTemp, float startHeight);
	void addSample(double pressure, double time);
	void deleteSample(double pressure, double time);
	void setNewBase(double newPressure, double newTemp);
	float getVelocitySinceLast();
	float getHeightDifferenz(float pressure, float temperature, int method);
	LinearRegression getLinearRegression();

private:
	int countMean;
	int n = 0;
	float startPressure;
	float basePressure;
	float baseTemp;
	//für gleitenderDurchschnitt letzte Messungen werden gespeichert
	CFiFo<float, lengthGeleitenderDurchschnitt> lastPressures;
	CFiFo<float, lengthGeleitenderDurchschnitt> lastTimes;

	float meanPressureSum = 0;
	float meanTimeSum = 0;
	LinearRegression lr = LinearRegression();
	const float universalR = 8.3144598;
	const float spezificR = 287.053;
	const float g = 9.80665;
	const float M = 0.0289644;
	const float L = 0.0065;
};

#endif