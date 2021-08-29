#include "LinearRegression.h"

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
void LinearRegression::addPoint(double x, double y)
{
	n++;
	sumMeanX = sumMeanX + x;
	sumMeanY = sumMeanY + y;
	sumMeanXY = sumMeanXY + x * y;
	sumMeanXYSq = sumMeanXYSq + x * x;
}

void LinearRegression::deletePoint(double x, double y)
{
	n--;
	sumMeanX = sumMeanX - x;
	sumMeanY = sumMeanY - y;
	sumMeanXY = sumMeanXY - x * y;
	sumMeanXYSq = sumMeanXYSq - x * x;
}

float LinearRegression::getSlope()
{
	/*float xbar = sumMeanX / n;
	float ybar = sumMeanY / n;
	float xybar = sumMeanXY / n;
	float xsqbar = sumMeanXYSq / n;

	float m = (xybar - xbar * ybar) / (xsqbar - xbar * xbar);
	return m;*/
	float m = (n*sumMeanXY-sumMeanX*sumMeanY)/(n*sumX2-sumX*sumX);
}

//Zum debuggen
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

//alle Summen zurücksetzen
void LinearRegression::reset()
{
	n = 0;
	sumMeanX = 0;
	sumMeanY = 0;
	sumMeanXY = 0;
	sumMeanXYSq = 0;
	//b = 0;
}