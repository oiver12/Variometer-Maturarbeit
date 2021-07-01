#ifndef Variometer_h
#define Variometer_h

#include "LinearRegression.h";


class Variometer {
public:
	float reduzierterLuftdruckStart;
	//Queue_t q;
	void init(int countMean, float startPressure, float startTemp, float startHeight);
	void addSample(double pressure, double time);
	float getVelocitySinceLast();
	float getHeightDifferenz(float pressure, float temperature, int method);

private:
	float startPressure;
	float startTemp;
	//int lengthBuffer;
	int countMean;
	int n = 0;
	double meanPressure = 0;
	double meanTime = 0;
	LinearRegression lr = LinearRegression();
	const float universalR = 8.3144598;
	const float spezificR = 287.053;
	const float g = 9.80665;
	const float M = 0.0289644;
	const float L = 0.0065;
};

#endif