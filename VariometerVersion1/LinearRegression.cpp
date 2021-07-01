#include "LinearRegression.h"

void LinearRegression::learn(double x, double y)
{
	n++;
	sumMeanX = sumMeanX + x;
	sumMeanY = sumMeanY + y;
	sumMeanXY = sumMeanXY + x * y;
	sumMeanXYSq = sumMeanXYSq + x * x;
}

float LinearRegression::getSlope()
{
	float xbar = sumMeanX / n;
	float ybar = sumMeanY / n;
	float xybar = sumMeanXY / n;
	float xsqbar = sumMeanXYSq / n;

	float m = (xybar - xbar * ybar) / (xsqbar - xbar * xbar);
	return m;
}

int LinearRegression::samples()
{
	return n;
}

void LinearRegression::reset()
{
	Serial.println(String(n) + "n");
	int n = 0;
	sumMeanX = 0;
	sumMeanY = 0;
	sumMeanXY = 0;
	sumMeanXYSq = 0;
	//b = 0;
}