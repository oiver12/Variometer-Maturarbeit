#include <MPU9250.h>
#include <Dps310.h>
#include <SoftwareSerial.h>
#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

#include <Kalman.h>
using namespace BLA;

#define Nstate 3 // position, speed, acceleration
#define Nobs 2   // position, acceleration

// measurement std
#define n_p 0.3
#define n_a 5.0

#define n_r_a 0.1
/*// model std (1/inertia)
#define m_p 0.1
#define m_s 0.1
#define m_a 0.8*/


BLA::Matrix<Nobs> obs; // observation vector
KALMAN<Nstate,Nobs> K; // your Kalman filter
BLA::Matrix<Nstate> state;

//es gibt verschiedene biepen für die Höhenangabe low hat konstanten tiefen Ton, equal hat keinen Ton
//die oberen Töne können eingestellt werden mit Frequenc vom Ton und Töne pro Sekunde zwischen den States wird
//linear gemacht
enum biepstate
{
  low = 0,
  equal = 1,
  up1 = 2,
  up2 = 3,
  up3 = 4,
  up4 = 5,
};
class Tonestate{
  public:
    biepstate state;
    float frequency;
    int duration;
    float lowerVelocity;
    float upVelocity;
};
//gesetzt wird untere Frequenc und all wie vielten Frame gepiept werden soll
Tonestate ToneArray[6] = {
  {biepstate::low, 120, -1, -999, -2.5f},
  {biepstate::equal, 0, 0, -2.5f, 0.15f},
  {biepstate::up1, 700, 750, 0.05f, 1.5f},
  {biepstate::up2, 850, 540, 1.5f, 3.5f},
  {biepstate::up3, 1000, 300, 3.5f, 10},
  {biepstate::up4, 1600, 90, 10, 999},
};
int indexCurrentToneArray = 2;
//benötigt um zu biepen --> nur einmal ausrechnen wenn Geschwindigkiet gesetzt wird
float frequencyNow = 700;
int durationNow = 750;
float lastTimeBuzzer = 0;
float lastBeepVelocity = 0;
float velocityThreshold = 0.2f;

MPU9250 mpu;
Dps310 Dps310PressureSensor = Dps310();
unsigned long lastTime = 0;
bool hasStartedDpsMeasure = false;
float waitingTime = 0;
float sumAcc;
float lastVelocity;
int countAcc;
unsigned long lastTimeKalman = 0;
bool hasStarted = false;
float startHeight = 0;
float startTemp = 0;
float startPressure = 0;
float lastPressure = 0;

//Objekt für den Bluethoot Controller
SoftwareSerial BTserial(2, 3); // RX | TX
BluetoothCommunication bluetooth = BluetoothCommunication();
uint8_t bluetoothRecieveBuffer[20];
int indexRecieveBuffer = 0;
bool packetStarted = false;


void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  Dps310PressureSensor.begin(Wire);
  delay(2000);
  MPU9250Setting settings  = MPU9250Setting();
  settings.accel_fs_sel = ACCEL_FS_SEL::A4G;
  settings.gyro_fs_sel = GYRO_FS_SEL::G500DPS;
  if (!mpu.setup(0x68, settings)) { 
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
  BTserial.begin(9600);
  //mpu.setFilterIterations(30);
  Serial.println("Begun");
  mpu.update();
}

void StartKalman()
{
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
  K.Q = {0.0,     0.0,     0.0,
                0.0, 0.0,     0.0,
                0.0,     0.0, 0.0};
  
  
  state.Fill(0.0);
  obs.Fill(0.0);
}

void loop() {
    if (BTserial.available() > 0) 
		{
			char incomingByte = BTserial.read();
      addByteToPacket(incomingByte);
		}

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
            Dps310PressureSensor.getResultWhenStarted(lastPressure);
            hasStartedDpsMeasure = false;
            float deltaT = (millis() - lastTimeKalman)/1000.0f;
            lastTimeKalman = millis();

            K.F = {1.0,  deltaT,  deltaT*deltaT/2,
		           0.0, 1.0, deltaT,
                   0.0, 0.0, 1.0};
            K.Q = {(deltaT*deltaT*deltaT*deltaT) / 4, (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT) / 2,
                (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT), deltaT,
                (deltaT*deltaT) / 2, deltaT, 1};
            K.Q *= n_r_a * n_r_a;

            state(0) = getHeightDifference(lastPressure);
            state(2) = sumAcc / (float)countAcc;
            obs = K.H * state;
            K.update(obs);
            lastVelocity = K.x(1);
            sendUpdate();
            //Serial.println(lastVelocity);
            sumAcc = 0.0f;
            countAcc = 0;
            setNewBeep();
            if(ToneArray[indexCurrentToneArray].state != biepstate::low && ToneArray[indexCurrentToneArray].state != biepstate::equal && millis() - lastTimeBuzzer > durationNow)
            {
              lastTimeBuzzer = millis();
              tone(8, frequencyNow, durationNow/2);
            }
        }
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
        }
    }
    else
    {
      mpu.update();
    }
}

//ausrechnen mit welcher Geschwindigkiet und Freqeunz gepiepst werden soll
void setNewBeep()
{
  int lengthTonestate = sizeof(ToneArray)/sizeof(Tonestate);
  for (size_t i = 0; i < lengthTonestate; i++)
  {
    if(lastVelocity >= ToneArray[i].lowerVelocity && lastVelocity <= ToneArray[i].upVelocity)
    {
      float difference = lastBeepVelocity - lastVelocity;
      if(fabs(difference) < velocityThreshold && !(indexCurrentToneArray > 1 && i == 1) && !(indexCurrentToneArray  == 1 && i > 1))
      {
          return;
      }
      Serial.println("Higher: " + String(fabs(difference), 4));
      lastBeepVelocity = lastVelocity;
      indexCurrentToneArray = i;
      if((lengthTonestate - 1) == i)
      {
        frequencyNow = ToneArray[indexCurrentToneArray].frequency;
        durationNow = ToneArray[indexCurrentToneArray].duration;
      }
      else
      {
		//linear zwischen zwei States machen
        float factor = (lastVelocity - ToneArray[indexCurrentToneArray].lowerVelocity)/(ToneArray[indexCurrentToneArray].upVelocity-ToneArray[indexCurrentToneArray].lowerVelocity);
        frequencyNow = factor * (ToneArray[i+1].frequency - ToneArray[indexCurrentToneArray].frequency) + ToneArray[indexCurrentToneArray].frequency;
        durationNow = factor * (ToneArray[i+1].duration - ToneArray[indexCurrentToneArray].duration) + ToneArray[indexCurrentToneArray].duration;
      }
    }
  }
  //wenn low dann ein konstanter Ton tief
  if(ToneArray[indexCurrentToneArray].state == biepstate::low)
      tone(8, ToneArray[indexCurrentToneArray].frequency);
	  //nicht biepsen wenn equal
    if(ToneArray[indexCurrentToneArray].state == biepstate::equal)
      noTone(8);
}

void sendUpdate()
{
	bluetooth.newPacket(arduinoPacketTypes::updateState);
	bluetooth.addFloat(lastVelocity);
	bluetooth.addFloat(lastPressure);
	sendPacket();
}

//nur aufrufen wenn Packet ready ist in der Bluetooth Klasse
void sendPacket()
{
    int size = 0;
    char *pointerToFirst = bluetooth.getString(&size);
    for (size_t i = 0; i < size; i++)
    {
        BTserial.write(*(pointerToFirst+i));
    }
}

void addByteToPacket(char byte)
{
    if(!packetStarted)
    {
        indexRecieveBuffer = 0;
        if((uint8_t)byte != startByte)
        {
            Serial.println("Erstes Byte war nicht startByte");
            packetStarted = false;
            return;
        }
        Serial.println("Packet started!");
        packetStarted = true;
    }
    bluetoothRecieveBuffer[indexRecieveBuffer] = byte;
    indexRecieveBuffer++;
    if(packetStarted && indexRecieveBuffer >= 1 && bluetooth._flutterPackets[(uint8_t)bluetoothRecieveBuffer[1]].lengthPacket == indexRecieveBuffer)
    {
        Serial.println("Packet ended");
        bluetooth.readPacket(bluetoothRecieveBuffer, indexRecieveBuffer);
        packetStarted = false;
        indexRecieveBuffer = 0;
    }
}

void PacketHandler::StartVariometer(float height)
{
  Serial.println("Start");
  startHeight = height;
  startPressure = ReadPressure(10);
  startTemp = readTemperatur(10);
  hasStarted = true;
  delay(100);
	bluetooth.newPacket(arduinoPacketTypes::startPacket);
  bluetooth.addFloat(startPressure);
  bluetooth.addFloat(startTemp);
  sendPacket();
  StartKalman();
}

void PacketHandler::StopVariometer()
{
	hasStarted = false;
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
