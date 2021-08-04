#ifndef LinearRegression_h
#define LinearRegression_h

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
class LinearRegression {
public:
	void addPoint(double x, double y);
	void reset();
	float getSlope();
	float getN();
	float getSumMeanX();
	float getSumMeanY();
	float getSumMeanXY();
	float getSumMeanXYSq();
	//double bufferX[5];
private:
	int n = 0;
	float sumMeanX = 0;
	float sumMeanY = 0;
	float sumMeanXY = 0;
	float sumMeanXYSq = 0;
};

#endif