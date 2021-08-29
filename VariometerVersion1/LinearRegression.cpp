#include "LinearRegression.h"

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
void LinearRegression::addPoint(double x, double y)
{
	n++;
	sumMeanX = sumMeanX + ((x - sumMeanX) / n);
	sumMeanY = sumMeanY + ((y - sumMeanY) / n);
	sumMeanXY = sumMeanXY + (((x*y) - sumMeanXY) / n);
	sumMeanXSq = sumMeanXSq + (((x*x) - sumMeanXSq) / n);
}

float LinearRegression::getSlope()
{
	float m = (sumMeanXY - sumMeanX * sumMeanY) / (sumMeanXSq - sumMeanX * sumMeanX);
	return m;
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