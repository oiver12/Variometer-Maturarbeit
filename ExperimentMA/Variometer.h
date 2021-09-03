#ifndef Variometer_h
#define Variometer_h

#include "LinearRegression.h"
#include "FiFo.h"

#define lengthLinearRegression 50
#define lengthGeleitenderDurchschnitt  5

//Klasse um Durchschnitt bzw Gleitender Druchschnitt auszurechnen
class Variometer {
public:
	//gleitender oder normaler Durchschnitt (gleitender genauer)
	bool gleitenderDurchschnitt = true;
	//Start des Variometers
	void addSample(double hoehe, double time);
	void deleteSample();
	LinearRegression getLinearRegression();

private:
	int countMean;
	int n = 0;
	//für gleitenderDurchschnitt letzte Messungen werden gespeichert
	CFiFo<float, lengthGeleitenderDurchschnitt> lastPressures;
	CFiFo<float, lengthGeleitenderDurchschnitt> lastTimes;

	CFiFo<float, lengthLinearRegression> allPressures;
	CFiFo<float, lengthLinearRegression> allTimes;
	float meanPressureSum = 0;
	float meanTimeSum = 0;
	LinearRegression lr = LinearRegression();
};

#endif