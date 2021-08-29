#include "LinearRegression.h"

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
void LinearRegression::addPoint(double x, double y)
{
	n++;
	/*sumMeanX = sumMeanX + x;
	sumMeanY = sumMeanY + y;
	sumMeanXY = sumMeanXY + x * y;
	sumMeanXSq = sumMeanXSq + x * x;*/
	sumMeanX = sumMeanX + ((x - sumMeanX) / n);
	sumMeanY = sumMeanY + ((y - sumMeanY) / n);
	sumMeanXY = sumMeanXY + (((x*y) - sumMeanXY) / n);
	sumMeanXSq = sumMeanXSq + (((x*x) - sumMeanXSq) / n);
	/*sumMeanX = sumMeanX * (n - 1) / n + x / n;
	sumMeanY = sumMeanY * (n - 1) / n + y / n;
	sumMeanXY = sumMeanXY * (n - 1) / n + (x*y) / n;
	sumMeanXSq = sumMeanXSq * (n - 1) / n + (x*x) / n;*/
}

//not working
void LinearRegression::deletePoint(double x, double y)
{
	float temp = (float)n / (float)(n-1);
	sumMeanX = temp * sumMeanX - (x / (n-1));
	sumMeanY = temp * sumMeanY - (y / (n - 1));
	sumMeanXY = temp * sumMeanXY - ((x*y) / (n - 1));
	sumMeanXSq = temp * sumMeanXSq - ((x*x) / (n - 1));
	n--;
	/*sumMeanX = sumMeanX - x / n;
	sumMeanY = sumMeanY - y / n;
	sumMeanXY = sumMeanXY - (x*y) / n;
	sumMeanXSq = sumMeanXSq - (x*x) / n;*/
	/*sumMeanX = sumMeanX - x;
	sumMeanY = sumMeanY - y;
	sumMeanXY = sumMeanXY - x * y;
	sumMeanXSq = sumMeanXSq - x * x;*/
}

float LinearRegression::getSlope()
{
	/*float xbar = sumMeanX / n;
	float ybar = sumMeanY / n;
	float xybar = sumMeanXY / n;
	float xsqbar = sumMeanXSq / n;

	float m = (xybar - xbar * ybar) / (xsqbar - xbar * xbar);
	return m;*/
	//float m = (n*sumMeanXY - sumMeanX * sumMeanY) / (n*sumMeanXSq - sumMeanX * sumMeanX);
	float m = (sumMeanXY - sumMeanX * sumMeanY) / (sumMeanXSq - sumMeanX * sumMeanX);
	return m;
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
	return sumMeanXSq;
}

//alle Summen zurücksetzen
void LinearRegression::reset()
{
	n = 0;
	sumMeanX = 0;
	sumMeanY = 0;
	sumMeanXY = 0;
	sumMeanXSq = 0;
	//b = 0;
}