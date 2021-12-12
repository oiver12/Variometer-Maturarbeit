#include <Dps310.h>
#include <SoftwareSerial.h>
#include "LinearRegression.h"
#include "Variometer.h"
#include "FiFo.h"
#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

//#define logSDCard

//wenn der Flug auf der SD Karte aufgezeichnet werden soll kann das hier aktiviert werden, jedoch hat der Arduino Nano zu wenig Speicherplatz :(
#ifdef logSDCard
#include <SD.h>
#include <SPI.h>
File myFile;
const int chipSelect = 10;
int frequencyLog = 1; //all so und so Frame
float lastTimeSDCard = 0;
float lastPressureSDCard;
#endif

//ob das Variometer gestartet wurde über die App
bool hasStarted = false;
//start Höche von der App
float startHeight;
//das letzte Mal als die Geschwindigkiet gemessen wurde
float lastTimeVelocity = 0;
//letzer gemessener Druck
double lastPressure;
//letzer gemessener Temperatur
double lastTemp;
//letztes Mal als Basishöhe für Höhendifferenz gesetzt wurde sonst wird es nach einer Zeit ungenau, weiss nicht genau wieso
double lastTimeSet;
//all wie viele Milisekunden die Basishöhe für die Höhendifferenz gesetzt werden soll
double baseSetRate = 200000;
//wie oft die Geschwindigkeit bestimmt werden soll
float velocityMeasureRate = 1500;

bool shouldMakeSound = true;
bool isConnected = false;

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
    //Achtung duration = modulaCount --> kleiner und es werden mehr Töne pro Sekunde gespielt
    int duration;
    float lowerVelocity;
    float upVelocity;
};
//gesetzt wird untere Frequenc und all wie vielten Frame gepiept werden soll
Tonestate ToneArray[6] = {
  {biepstate::low, 120, -1, -999, -2.5f},
  {biepstate::equal, 0, 0, -2.5f, 0.15f},
  {biepstate::up1, 700, 25, 0.1f, 1.5f},
  {biepstate::up2, 850, 18, 1.5f, 3.5f},
  {biepstate::up3, 1000, 10, 3.5f, 10},
  {biepstate::up4, 1600, 3, 10, 999},
};
int indexCurrentToneArray = 1;
//benötigt um zu biepen --> nur einmal ausrechnen wenn Geschwindigkiet gesetzt wird
float frequencyNow = 0;
int durationNow = 0;
int deltaTimeFrame = 30;
unsigned int count = 0;

//letzte gemessene Geschwindigkeit
float lastVelocity = 0.0f; //in m/s

//Objekt für den Bluethoot Controller
SoftwareSerial BTserial(2, 3); // RX | TX
BluetoothCommunication bluetooth = BluetoothCommunication();
uint8_t bluetoothRecieveBuffer[20];
int indexRecieveBuffer = 0;
bool packetStarted = false;

Variometer variometer = Variometer();
//Objekt für Luftdrucksensor
Dps310 Dps310PressureSensor = Dps310();

void setup()
{
	Serial.begin(115200);
	while (!Serial);
	Dps310PressureSensor.begin(Wire);
	#ifdef logSDCard
	//wenn der Arduino gestartet wird, wird altes File gelöscht und neues kreiert
	if (!SD.begin(chipSelect))
  	{
		Serial.println("Failed to set SDCard");
	  return;
  	}
	SD.remove("data.txt");
  	myFile = SD.open("data.txt", FILE_WRITE);
	#endif
	BTserial.begin(9600);
   int startTones[] = {349, 440, 523};
  //F4, A4, C5
  for (size_t i = 0; i < sizeof(startTones)/sizeof(startTones[i]); i++)
  {
    tone(8, startTones[i]);
    delay(500);
  }
  noTone(8);
	PacketHandler::StartVariometer(0, true);
}



void loop()
{
	//neuer Druck messen einmal
	lastPressure = ReadPressure(1);
	if (lastPressure == 0)
	{
		//etwas ist schiefgelaufen
		return;
	}
	else
	{
		if (BTserial.available() > 0) 
		{
			char incomingByte = BTserial.read();
        	addByteToPacket(incomingByte);
		}

		if(hasStarted)
		{	
			//neue Geschwindigkiet ausrechnen
			if(millis() - lastTimeVelocity >= velocityMeasureRate)
			{
				lastTemp = readTemperatur(3);
				lastVelocity = variometer.getVelocitySinceLast();
				if(isfinite(lastVelocity) == 0)
				{
					//Serial.println(String(lastVelocity) + "  " + "was not finit");
					lastVelocity = 0;
				}
				Serial.println(lastVelocity);
				setNewBeep();
				//BTserial.println(String(lastVelocity));
				lastTimeVelocity = millis();
				//neue Basishöhe setzten
				//TODO nach einer Basissetzung ist die Geschwindigkeit immer tiefer als es sollte
				if(millis() - lastTimeSet >= baseSetRate)
				{
					//sieben mal Temp und Druck messen und das als neue Basis setzten
					lastTemp = readTemperatur(1);
					lastPressure = ReadPressure(1);
					variometer.setNewBase(lastPressure, lastTemp);
					lastTimeSet = millis();
				}
				sendUpdate();
				#ifdef logSDCard
				if(myFile)
				{
					myFile.println("v" + String(lastVelocity) + ";" + String(lastTemp) + ";" + String(frequencyNow)+ ";" + String(durationNow));
				}
				#endif
			}
			 if(Serial.availableForWrite())
            {
				float height = variometer.getHeightDifferenz(lastPressure, lastTemp, 0);
				char *c_Data = ( char* ) &height;
				for( char c_Index = 0 ; c_Index < sizeof( float ) ; Serial.write( c_Data[ c_Index++ ] ) );
				Serial.write(0xFF);
            }
			//neuer Druck und Zeit (in Sekunden) der Messung hinzufügen
			variometer.addSample(lastPressure, millis()/static_cast<double>(1000));
			//wenn es biepsen soll (nicht low oder equal) dann biepsen
			if(ToneArray[indexCurrentToneArray].state != biepstate::low && ToneArray[indexCurrentToneArray].state != biepstate::equal && count%durationNow == 0 && shouldMakeSound)
			{
				tone(8, frequencyNow, (deltaTimeFrame*durationNow)/2);
			}
			#ifdef logSDCard
			if(myFile && count%frequencyLog == 0)
			{
				myFile.println(String(millis() - lastTimeSDCard) + ';' + String(lastPressure - lastPressureSDCard));
				lastTimeSDCard = millis();
				lastPressureSDCard = lastPressure;
			}
			#endif
			count++;
		}
	}
}

void sendUpdate()
{
	if(isConnected)
	{
		bluetooth.newPacket(arduinoPacketTypes::updateState);
		bluetooth.addFloat(lastVelocity);
		bluetooth.addFloat(lastPressure);
		sendPacket();
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
            //Serial.println("Erstes Byte war nicht startByte");
            packetStarted = false;
            return;
        }
        //Serial.println("Packet started!");
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

void PacketHandler::StartVariometer(float height, bool _soundON)
{
	Serial.println("Started");
	shouldMakeSound = _soundON;
	lastTemp = readTemperatur(10);
	startHeight = height;
	float startPressure = ReadPressure(10);
	delay(100);
	hasStarted = true;
	variometer.init(1, startPressure, lastTemp, startHeight);
	if(isConnected)
	{
		bluetooth.newPacket(arduinoPacketTypes::StartVarioPacket);
		bluetooth.addFloat(startPressure);
		bluetooth.addFloat(lastTemp);
		sendPacket();	
	}
	#ifdef logSDCard
	if(myFile)
	{
		myFile.println("Start " + String(millis()) + ";" + String(startPressure) + ";" + String(startHeight) + ";" + String(lastTemp));
	}
	#endif
}

void PacketHandler::StopVariometer()
{
	hasStarted = false;
}

//es gibt Funktion oversampling bei measurePressureOnce, jedoch gibt es dort falsche Werte hinaus, z.B. bei Druck immer zu tiefe Werte
//daher oversampling mit einer Loop selbst implementiert
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

void PacketHandler::WelcomePacket()
{
	isConnected = true;
	StopVariometer();
	//erstes Mal vom Arduino senden wird erstes Byte verloren daher hier ein Byte senden (ACHTUNG SCHLECHTE LÖSUNG)
	BTserial.write(29);
	bluetooth.newPacket(arduinoPacketTypes::WelcomResponsePacket);
	bluetooth.addBool(Dps310PressureSensor.initSucces());
	bluetooth.addBool(true);
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
