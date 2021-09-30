#include <MPU9250.h>
#include <Dps310.h>
#include <SoftwareSerial.h>
#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

#include <Kalman.h>
using namespace BLA;

//es gibt 3 Zustände (siehe MA)
#define Nstate 3 // Höhe, vertikale Geschwindigkeit, vertikale Beschleunigung
//zwei Werte werden gemessen(Höhe und Beschleunigung)
#define Nobs 2   // Höhe, vertikale Beschleunigung

//Observatonsmatrix (gemessene Werte)
BLA::Matrix<Nobs> obs;
//Die Klasse für den Filter
KALMAN<Nstate,Nobs> K;
//Ein Temporärer Vektor für die gemessenen Wert, bevor diese umgewandelt werden(in der MA z)
BLA::Matrix<Nstate> state;

//diese nicht verstellen. Lassen sich in der App temp. verstellen
//Prozessrauschen (zufällige Varianz in der Beschleunigung) (siehe Experiment MA)
float n_p = 0.1298;
//Standardabweichung Beschleunigung (siehe Experiment MA)
float n_a = 0.0069;
//Standardabweichun Höhe (siehe Experiment)
//float n_r_a = 0.1183;
float n_r_a = 0.9;

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
  {biepstate::equal, 0, 0, -2.5f, 0.1f},
  {biepstate::up1, 700, 750, 0.1f, 1.5f},
  {biepstate::up2, 850, 540, 1.5f, 3.5f},
  {biepstate::up3, 1000, 300, 3.5f, 10},
  {biepstate::up4, 1600, 90, 10, 999},
};
//der Inder für den Momentatenen Punkt (also der nächste Punkt untendran)
int indexCurrentToneArray = 2;
//benötigt um zu biepen --> nur einmal ausrechnen wenn Geschwindigkiet gesetzt wird
float frequencyNow = 700;
int durationNow = 750;
//für das speichern, wenn das letzte Mal der Buzzer betätigt wurde
float lastTimeBuzzer = 0;
//was die letzte Geschwindigkeit war, in welcher es gepiept hat, für den Threshold der Geschwindigkeit
float lastBeepVelocity = 0;
//Bei welcher Geschwindigkeitsänderung der Ton ändern soll. Damit bleibt das Piepen bei konstanter Thermik besser auf einem Ton
float velocityThreshold = 0.2f;
//ist das Variometer mit meinem App verbunden
bool isConnected = false;

int count = 0;
//all wie viel Frames ein Update an das App via Bluetoth gesendet werden soll
int moduloSendBluethoot = 30; //=30*30 = all 900ms
//keine SD-Karte...
bool initSDCard = false;
//über App einstellen
bool shouldMakeSound = true;

//kann definiert werden, ob SD Karte gespeichert werden soll. Speicherplatz für Arduin zu gross
#ifdef saveSDCard
#include <SPI.h>
#include <SD.h>
Sd2Card card;
SdVolume volume;
#endif

//Objekt für MPU9250
MPU9250 mpu;
Dps310 Dps310PressureSensor = Dps310();
//wann die Anfangszeit der Messung für den Luftdruck war
unsigned long lastTime = 0;
//wenn die Messung gestartet wird, kann nach x Sekunden die Messung abgefragt werden
bool hasStartedDpsMeasure = false;
//über App
bool useXCTrack = false;
float waitingTime = 0;
//summe der Beschleunigung bis der Luftdruck gemessen wird. Dadurch kann Mittelwert gebildet werden
float sumAcc;
//letzte Geschwindigkeit von Kalman Filter
float lastVelocity;
//Für Mittelwert Beschleunigung
int countAcc;
//für deltaT von Kalman
unsigned long lastTimeKalman = 0;
//ist das Variometer gestartet
bool hasStarted = false;
//Starthöhe über die App
float startHeight = 0;
//Temp am Anfang des Variometers
float startTemp = 0;
//Startdruck für deltaZ
float startPressure = 0;
float lastPressure = 0;

//Objekt für den Bluethoot Controller
SoftwareSerial BTserial(2, 3); // RX | TX
//Für die Pakte über Bluetooth
BluetoothCommunication bluetooth = BluetoothCommunication();
//Ein Buffer für die hineinkommenden Pakete: Wird gespeichert ab Startbyte und bis die Länge des Pakets erreicht ist (Bytes kommen stück für stück)
uint8_t bluetoothRecieveBuffer[70];
int indexRecieveBuffer = 0;
bool packetStarted = false;

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  Dps310PressureSensor.begin(Wire);
  delay(2000);
  //damit kann die Genuaigkeit höher sein, aber die maximale Beschleunigung und Drehung ist kleiner.Reciht jedoch längst für Gleitschirm...
  MPU9250Setting settings  = MPU9250Setting();
  settings.accel_fs_sel = ACCEL_FS_SEL::A4G;
  settings.gyro_fs_sel = GYRO_FS_SEL::G500DPS;
  if (!mpu.setup(0x68, settings)) { 
      while (1) {
          delay(5000);
      }
  }
  //Bestimmt mit MPU9250 Bibliothek: mpu.calibrateAccelGyro()
  mpu.setAccBias(640.5f, 65.4f, 313.1f);
  mpu.setGyroBias(-412.475f, 0.45f, -134.55f);
  mpu.setMagBias(118.51035f, -78.3431f, -30.889767f);
  mpu.setMagScale(1.356725f, 0.7227414f, 1.137255f);
  delay(1000);
  BTserial.begin(9600);
  //Starttöne einfacher Dreiklang
  int startTones[] = {349, 440, 523};
  //                  F4, A4, C5
  for (size_t i = 0; i < sizeof(startTones)/sizeof(startTones[i]); i++)
  {
    tone(8, startTones[i]);
    delay(500);
  }
  noTone(8);
  Serial.println("Begun");
  mpu.update();
  //starte das Variometer
  PacketHandler::StartVariometer(0, true, true);
  #ifdef saveSDCard
  initSDCard = card.init(SPI_HALF_SPEED, 10);
  initSDCard = volume.init(card);
  #endif
}

void StartKalman()
{
  //Die verschiedenen Matrixen, hergeleitet in der MA. Aber Zuerst zurücksetzten
  //K.Reset();
  //Unsicherheitsmatrix->gross
  K.P = {500.0, 0.0, 0.0,
          0.0, 500.0, 0.0,
          0.0, 0.0, 500.0
  };
  //wird noch in der Update oop richtig gesetzt. Ist für das Bringen in den nächsten Zustand
  K.F = {1.0, 0.0, 0.0,
          0.0, 1.0, 0.0,
          0.0, 0.0, 1.0};
  //Für das Umwandlen in den Zustand
  K.H = {1.0, 0.0, 0.0,
          0.0, 0.0, 1.0};
  //Messunsicherheiten
  K.R = {n_p*n_p,   0.0,
          0.0, n_a*n_a};
  //Prozessrauschen wird auch in der Loop gesetzt
  K.Q = {0.0,     0.0,     0.0,
                0.0, 0.0,     0.0,
                0.0,     0.0, 0.0};
  //Zustand auf 0 schätzen
  K.x.Fill(0.0);
  state.Fill(0.0);
  obs.Fill(0.0);
}

void loop() {
  //Wenn neues Byte über Bluetooth kommt, dann zum PAcket hinzufügen
    if (BTserial.available() > 0) 
		{
			char incomingByte = BTserial.read();
      addByteToPacket(incomingByte);
		}
    //wenn Variometer gestartet also Peipen
    if(hasStarted)
    {
      //ES startet eine Luftdruckmessung. Nun wartet es so lange, bis eine neue Luftdruckmessung zur Verfügung steht. In der Zwischenzeit wird die
      //orientierung und die Beschleunigung ungefähr 6 mal ausgerechnet. Damit geht Oreintierung nicht verloren
        if(!hasStartedDpsMeasure)
        {
          //Starte Messung
            Dps310PressureSensor.startPressureWithoutDelay(waitingTime);
            hasStartedDpsMeasure = true;
            lastTime = millis();
        }
        //Wenne Luftdruck vorhanden
        if(hasStartedDpsMeasure && millis() - lastTime >= waitingTime)
        {
            Dps310PressureSensor.getResultWhenStarted(lastPressure);
            hasStartedDpsMeasure = false;
            float deltaT = (millis() - lastTimeKalman)/1000.0f;
            lastTimeKalman = millis();
            //aus MA um den neuen Zustand auszurechnen
            K.F = {1.0,  deltaT,  deltaT*deltaT/2,
		           0.0, 1.0, deltaT,
                   0.0, 0.0, 1.0};
            //Prozessrauschen
            K.Q = {(deltaT*deltaT*deltaT*deltaT) / 4, (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT) / 2,
                (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT), deltaT,
                (deltaT*deltaT) / 2, deltaT, 1};
            K.Q *= n_r_a * n_r_a;
            //Die Messung wird gespeichert und mit der Matrix H in den Zustand umgewandelt
            state(0) = getHeightDifference(lastPressure);
            state(2) = sumAcc / (float)countAcc;
            obs = K.H * state;
            //Kalman Filter update
            K.update(obs);
            //Geschwindigkeit des Klaman Filters
            lastVelocity = K.x(1);
            //all so und so Frame ein UpdatePacket schicken, da Handy sonst überfordert
            if(count % moduloSendBluethoot == 0)
            {
              sendUpdate();
            }
            //Mittelwert zurücksetzten
            sumAcc = 0.0f;
            countAcc = 0;
            
            if(shouldMakeSound)
            {
              //neue Freuquenz und Länge ausrechnen und Piepen
              setNewBeep();
              if(ToneArray[indexCurrentToneArray].state != biepstate::low && ToneArray[indexCurrentToneArray].state != biepstate::equal && millis() - lastTimeBuzzer > durationNow)
              {
                lastTimeBuzzer = millis();
                tone(8, frequencyNow, durationNow/2);
              }
            }
            count++;
        }
        //Orientierung ausrechnen und vertikale Beschleunigung ausrechnen
        if (mpu.update()) {
            float q0 = mpu.getQuaternionW();
            float q1 = mpu.getQuaternionX();
            float q2 = mpu.getQuaternionY();
            float q3 = mpu.getQuaternionZ();
            //Drehen mit Orientierung
            // accX minus da Madgwick hat X Beschleunigung minus
            float acc = (2*(q1*q3 - q0*q2)) * -mpu.getAccX() + (2*(q2*q3+q0*q1)) * mpu.getAccY() + (2*(q0*q0+q3*q3) -1)*mpu.getAccZ();
            float gravityCompensatedAccel = (acc - 1.0f) / -0.980665f;
            sumAcc += gravityCompensatedAccel;
            countAcc++;
        }
    }
    else
    {
      //sonst nur Orienteirung ausrechnen
      mpu.update();
    }
}

//ausrechnen mit welcher Geschwindigkiet und Freqeunz gepiepst werden soll
void setNewBeep()
{
  //in einer For schleife herausfinden, zwischen welchen Punkten die Geschwindigkeit ist
  int lengthTonestate = sizeof(ToneArray)/sizeof(Tonestate);
  for (size_t i = 0; i < lengthTonestate; i++)
  {
    if(lastVelocity >= ToneArray[i].lowerVelocity && lastVelocity <= ToneArray[i].upVelocity)
    {
      float difference = lastBeepVelocity - lastVelocity;
      //Wenn Differenz Geschwindigkeit kliener als Thershold ist, dann nicht neuer Ton ausgeben, ausser man geht von nicht piepsen in piepsen oder umgekehrt
      if(fabs(difference) < velocityThreshold && !(indexCurrentToneArray > 1 && i == 1) && !(indexCurrentToneArray  <= 1 && i > 1))
      {
          return;
      }
      lastBeepVelocity = lastVelocity;
      indexCurrentToneArray = i;
      //wenn letzter Ton, dann einfach Geschwindigkeit nehemn
      if((lengthTonestate - 1) == i)
      {
        frequencyNow = ToneArray[indexCurrentToneArray].frequency;
        durationNow = ToneArray[indexCurrentToneArray].duration;
      }
      //sonst linear Interplorieren
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

//senden eines Update an App oder XCTrack
void sendUpdate()
{
  //neues Packet mit Geschwindigkeit und Druck
  if(!useXCTrack && isConnected)
  {
    bluetooth.newPacket(arduinoPacketTypes::updateState);
    bluetooth.addFloat(lastVelocity);
    bluetooth.addFloat(lastPressure);
    sendPacket();
  }
  //senden an XCTrack
  else
  {
    //Bluetooth Protokoll für XCTrack
    String str_out = String("LK8EX1,")
    +String(lastPressure)
    +String(",99999,") //Höhe wird  ignoriert, wenn Druck gesendet wird
    +String(lastVelocity * 100) //in cm/s
    +String(",99") //Temperatur 
    +String(",999,"); //Batterie
    unsigned int checksum_end, ai, bi;      
    //checksumme                                        
    for (checksum_end = 0, ai = 0; ai < str_out.length(); ai++)
    {
      bi = (unsigned char)str_out[ai];
      checksum_end ^= bi;
    }
    BTserial.print("$");       
    BTserial.print(str_out);
    BTserial.print("*");
    BTserial.println(checksum_end, HEX);
  }
}

//nur aufrufen wenn Packet ready ist in der Bluetooth Klasse
void sendPacket()
{
  //schrieben von Packet auf den Ausgang des Adapters
    int size = 0;
    char *pointerToFirst = bluetooth.getString(&size);
    for (size_t i = 0; i < size; i++)
    {
      BTserial.write(*(pointerToFirst+i));
    }
}

//Byte zu Paket hinzufügen
void addByteToPacket(char byte)
{
  //wenn Paket nicht gestartet ist, dann schauen ob startbyte, sonst ignorieren
    if(!packetStarted)
    {
        indexRecieveBuffer = 0;
        //ignorieren
        if((uint8_t)byte != startByte)
        {
            packetStarted = false;
            return;
        }
        packetStarted = true;
    }
    //in Paket einfach Buffer füllen
    bluetoothRecieveBuffer[indexRecieveBuffer] = byte;
    indexRecieveBuffer++;
    //wenn Paketlänge erreicht, mit index geschaut, dann Paket auswerten
    if(packetStarted && indexRecieveBuffer >= 1 && bluetooth._flutterPackets[(uint8_t)bluetoothRecieveBuffer[1]].lengthPacket == indexRecieveBuffer)
    {
      //Paket auswerten
        bluetooth.readPacket(bluetoothRecieveBuffer, indexRecieveBuffer);
        packetStarted = false;
        indexRecieveBuffer = 0;
    }
}

//Start Variometer
void PacketHandler::StartVariometer(float height, bool _useXCTrack, bool _soundON)
{
  Serial.println("Started" + String(_soundON));
  useXCTrack = _useXCTrack;
  startHeight = height;
  shouldMakeSound = _soundON;
  if(!_soundON)
  {
    noTone(8);
  }
  //Startdruck 10 mal messen und Start Temp auch
  startPressure = ReadPressure(10);
  startTemp = readTemperatur(10);
  hasStarted = true;
  //wenn verbunden ist, dann startpacket schicken
  if(isConnected)
  {
    bluetooth.newPacket(arduinoPacketTypes::StartVarioPacket);
    bluetooth.addFloat(startPressure);
    bluetooth.addFloat(startTemp);
    sendPacket();
  }
  StartKalman();
}

//VArio stoppen
void PacketHandler::StopVariometer()
{
  Serial.println("Stop");
	hasStarted = false;
  Dps310PressureSensor.getResultWhenStarted(lastPressure);
}

//welche Komponenten funktionieren. Vario stoppen bei weil hier App verbunden
void PacketHandler::WelcomePacket()
{
  isConnected = true;
  StopVariometer();
  //erstes Mal vom Arduino senden wird erstes Byte verloren daher hier ein Byte senden (ACHTUNG SCHLECHTE LÖSUNG)
  BTserial.write(29);
  bluetooth.newPacket(arduinoPacketTypes::WelcomResponsePacket);
  bluetooth.addBool(Dps310PressureSensor.initSucces());
  bluetooth.addBool(mpu.isConnected());
  #ifdef saveSDCard
  bluetooth.addBool(initSDCard);
  //Von SDCard Info example
  if(initSDCard)
    bluetooth.addFloat(volume.clusterCount() * volume.blocksPerCluster() / 2);
  else
    bluetooth.addFloat(0);
  #endif
  #ifndef saveSDCard
  bluetooth.addBool(false);
  bluetooth.addFloat(0);
  #endif
  sendPacket();
}

//sound Settings empfangen
void PacketHandler::soundSettings(float *tempArray, int size)
{
  ToneArray[0].upVelocity = *(tempArray);
  ToneArray[1].lowerVelocity = *(tempArray);
  ToneArray[0].frequency = *(tempArray+1);
  //ignore 2
  for(int i = 0; i < 4; i++)
  {
    ToneArray[i + 1].upVelocity = *(tempArray + (i*3) + 3);
    ToneArray[i + 2].lowerVelocity = *(tempArray+ (i*3) + 3);
    ToneArray[i + 2].frequency = *(tempArray+ (i*3) + 4);
    ToneArray[i + 2].duration = *(tempArray+ (i*3) + 5);
    Serial.println(String(ToneArray[i + 1].upVelocity) + "   " + String(ToneArray[i + 2].frequency) + "   " + String(ToneArray[i + 2].duration));
  }
}

//kalman settings empfangen
void PacketHandler::KalmanSetting(float standHeight, float standAcc, float processNoise)
{
  Serial.println(String(standHeight) + ";  " + String(standAcc) + "; " + String(processNoise));
  n_p = standHeight;
  n_a = standAcc;
  n_r_a = processNoise;
   K.R = {n_p*n_p,   0.0,
          0.0, n_a*n_a};
}

//berechnen der Differenzhlhe (siehe MA)
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
			//Serial.print("FAIL! ret = ");
			//Serial.println(ret);
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
			//Serial.print("FAIL! ret = ");
			//Serial.println(ret);
			return 0;
		}
	}
	return (sumTemp/oversampling);
}
