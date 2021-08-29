#ifndef LinearRegression_h
#define LinearRegression_h

//Lineare Regression, welche laufend geupdatet wird, dann durch getSlope kann die Geschwindigkiet
//ausgelesen werden
class LinearRegression {
public:
	void addPoint(double x, double y);
	void reset();
	float getSlope();
private:
	int n = 0;
	float MeanX = 0;
	float MeanY = 0;
	float MeanXY = 0;
	float MeanX2 = 0;
};

#endif