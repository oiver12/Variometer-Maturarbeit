#include "LinearRegression.h"

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
void LinearRegression::addPoint(double x, double y)
{
	n++;
	MeanX = MeanX + ((x - MeanX) / n);
	MeanY = MeanY + ((y - MeanY) / n);
	MeanXY = MeanXY + (((x*y) - MeanXY) / n);
	MeanX2 = MeanX2 + (((x*x) - MeanX2) / n);
}

float LinearRegression::getSlope()
{
	float m = (MeanXY - MeanX * MeanY) / (MeanX2 - MeanX * MeanX);
	return m;
}

//alle Summen zurücksetzen
void LinearRegression::reset()
{
	n = 0;
	MeanX = 0;
	MeanY = 0;
	MeanXY = 0;
	MeanX2 = 0;
	//b = 0;
}