#include "LinearRegression.h"

void LinearRegression::learn(double x, double y)
{
	//bufferX[n] = x;
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

float LinearRegression::getN()
{
	return n;
}
float LinearRegression::getSumMeanX()
{
	return sumMeanX;
}
float LinearRegression::getSumMeanY()
{
	return sumMeanY;
}
float LinearRegression::getSumMeanXY()
{
	return sumMeanXY;
}
float LinearRegression::getSumMeanXYSq()
{
	return sumMeanXYSq;
}

void LinearRegression::reset()
{
	n = 0;
	sumMeanX = 0;
	sumMeanY = 0;
	sumMeanXY = 0;
	sumMeanXYSq = 0;
	//b = 0;
}