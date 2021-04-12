#ifndef LinearRegression_h
#define LinearRegression_h

#include "Arduino.h"

class LinearRegression {
public:
	void learn(double x, double y);
	double calculate(double x);
	//double correlation();
	void parameters(double values[]);
	void reset();
	int samples();
	double error(double x, double y);
	float getSlope();
private:
	double meanX = 0;
	double meanX2 = 0; //mean x�
	double varX = 0;
	double meanY = 0;
	//double meanY2 = 0; //mean y�
	//double varY = 0;
	double meanXY = 0; //mean x*y
	double covarXY = 0;
	double n = 0;
	// m*x + b = y;
	double m = 0;
	//double b = 0;
};

#endif