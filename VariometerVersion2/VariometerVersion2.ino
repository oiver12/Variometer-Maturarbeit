#include <MPU9250.h>
#include <Dps310.h>

#include "Kalman.h"
using namespace BLA;

#define Nstate 3 // position, speed, acceleration
#define Nobs 2   // position, acceleration

// measurement std
#define n_p 0.3
#define n_a 5.0
// model std (1/inertia)
#define m_p 0.1
#define m_s 0.1
#define m_a 0.8


BLA::Matrix<Nobs> obs; // observation vector
KALMAN<Nstate,Nobs> K; // your Kalman filter
BLA::Matrix<Nstate> state;

MPU9250 mpu;
Dps310 Dps310PressureSensor = Dps310();
unsigned long lastTime = 0;
bool hasStartedDpsMeasure = false;
float waitingTime = 0;
float sumAcc;
int countAcc;
unsigned long lastTimeKalman = 0;
bool hasStarted = false;
float startHeight = 0;
float startTemp = 0;
float startPressure = 0;


void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  Dps310PressureSensor.begin(Wire);
  delay(2000);

  if (!mpu.setup(0x68)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed.");
          delay(5000);
      }
  }
  mpu.setAccBias(640.5f, 65.4f, 313.1f);
  mpu.setGyroBias(-412.475f, 0.45f, -134.55f);
  mpu.setMagBias(118.51035f, -78.3431f, -30.889767f);
  mpu.setMagScale(1.356725f, 0.7227414f, 1.137255f);
  delay(1000);
  mpu.setMagneticDeclination(2.97);
  //mpu.setFilterIterations(30);
  Serial.println("Begun");
  mpu.update();

  // time evolution matrix
  K.F = {1.0, 0.0, 0.0,
          0.0, 1.0, 0.0,
          0.0, 0.0, 1.0};

  // measurement matrix
  K.H = {1.0, 0.0, 0.0,
          0.0, 0.0, 1.0};
  // measurement covariance matrix
  K.R = {n_p*n_p,   0.0,
          0.0, n_a*n_a};
  // model covariance matrix
  K.Q = {m_p*m_p,     0.0,     0.0,
              0.0, m_s*m_s,     0.0,
              0.0,     0.0, m_a*m_a};
  
  
  state.Fill(0.0);
  obs.Fill(0.0);
  Serial.println("Here ok wtf");
}

void loop() {
  //float result;
    //Dps310PressureSensor.measurePressureOnce(result);
    if(hasStarted)
    {
        if(!hasStartedDpsMeasure)
        {
            Dps310PressureSensor.startPressureWithoutDelay(waitingTime);
            hasStartedDpsMeasure = true;
            lastTime = millis();
        }
        if(hasStartedDpsMeasure && millis() - lastTime >= waitingTime)
        {
            float result;
            Dps310PressureSensor.getResultWhenStarted(result);
            hasStartedDpsMeasure = false;
            float deltaT = (millis() - lastTimeKalman)/1000.0f;
            lastTimeKalman = millis();
            K.F = {1.0,  deltaT,  deltaT*deltaT/2,
		           0.0, 1.0, deltaT,
                   0.0, 0.0, 1.0};
            state(0) = getHeightDifference(result);
            state(2) = sumAcc / (float)countAcc;
            obs = K.H * state;
            K.update(obs);
            /*Serial.print("State: ");
            for (size_t i = 0; i < state.Cols; i++)
            {
                Serial.print(state(i));
                Serial.print(", ");
            }*/
            /*Serial.print("Obs: ");
            for (size_t i = 0; i < state.Cols; i++)
            {
                Serial.print(obs(i));
                Serial.print(", ");
            }
            Serial.print("  X:");
            for (size_t i = 0; i < K.x.Cols; i++)
            {
                Serial.print(K.x(i));
                Serial.print(", ");
            }
            Serial.println();*/
            Serial.println(K.x(1));
            sumAcc = 0.0f;
            countAcc = 0;
        }
        float _c00, _c10, _c20, _c30, _c01, _c11, _c21 = 0;
        float _scaled_rawtemp = 0;
        float _pressure;

        _pressure =
            (int32_t)_c00 +
            _pressure * ((int32_t)_c10 +
            _pressure * ((int32_t)_c20 + _pressure * (int32_t)_c30)) +
            _scaled_rawtemp *
            ((int32_t)_c01 +
            _pressure * ((int32_t)_c11 + _pressure * (int32_t)_c21));

        if (mpu.update()) {
            float q0 = mpu.getQuaternionW();
            float q1 = mpu.getQuaternionX();
            float q2 = mpu.getQuaternionY();
            float q3 = mpu.getQuaternionZ();
            // accX minus da Madgwick hat X Beschleunigung minus
            float acc = (2*(q1*q3 - q0*q2)) * -mpu.getAccX() + (2*(q2*q3+q0*q1)) * mpu.getAccY() + (2*(q0*q0+q3*q3) -1)*mpu.getAccZ();
            float gravityCompensatedAccel = (acc - 1.0f) / -0.980665f;
            sumAcc += gravityCompensatedAccel;
            countAcc++;
            //Serial.println(String(mpu.getLinearAccZ()));
        }
    }
    else
    {
        mpu.update();
        if(Serial.available())
        {
            char incomingByte = Serial.read();
            if(incomingByte == 's')
            {
                startHeight = Serial.parseFloat();
                startPressure = ReadPressure(10);
                startTemp = readTemperatur(10);
                hasStarted = true;
            }
        }
    }
}



float getHeightDifference(float pressure)
{
	const float spezificR = 287.053;
	const float g = 9.80665;
	const float L = 0.0065;
    return ((startTemp + 273.15) / L)*(pow(pressure / startPressure, -L * spezificR / g) - 1);
}

float ReadPressure(int oversampling)
{
	float pres;
	//uint8_t oversampling = 1;
	int16_t ret;
	float sumPres = 0;
	for(int i = 0; i< oversampling; i++)
	{
		ret = Dps310PressureSensor.measurePressureOnce(pres, 1);
		sumPres += pres;
		if (ret != 0)
		{
			//Something went wrong.
			//Look at the library code for more information about return codes
			Serial.print("FAIL! ret = ");
			Serial.println(ret);
			return 0;
		}
	}
	return (sumPres/oversampling);
}

float readTemperatur(int oversampling)
{
	float temp;
	float sumTemp = 0;
	int16_t ret;
	for (size_t i = 0; i < oversampling; i++)
	{
		ret = Dps310PressureSensor.measureTempOnce(temp, 1);
		sumTemp += temp;
		if (ret != 0)
		{
			//Something went wrong.
			//Look at the library code for more information about return codes
			Serial.print("FAIL! ret = ");
			Serial.println(ret);
			return 0;
		}
	}
	return (sumTemp/oversampling);
}
