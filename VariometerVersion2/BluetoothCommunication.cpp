#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

//hneues Paket machen: StartByte und index des Pakets
void BluetoothCommunication::newPacket(arduinoPacketTypes packetType)
{
	reset();
	buffer[indexEndBuffer] = startByte;
	indexEndBuffer++;
	addByte((char)packetType);
}

//Byte zum Paket hinzufügen, dabei wird der Buffer gefüllt und der Index vergrössert
void BluetoothCommunication::addByte(char byte)
{
	//ein zweites Mal dazu machen
	/*if (byte == startByte)
	{
		buffer[indexEndBuffer] = byte;
		indexEndBuffer++;
		crc8.add(byte);
	}*/
	buffer[indexEndBuffer] = byte;
	indexEndBuffer++;
	crc8.add(byte);
	//if (indexEndBuffer == sizeof(buffer) / sizeof(uint8_t))
		//Serial.println("Buffer full!!");
}
//bool zu Paket hinzufügen, welches gestartet wurde
void BluetoothCommunication::addBool(bool value)
{
	char* temp = static_cast<char*>(static_cast<void*>(&value));
	addByte(*temp);
}

void BluetoothCommunication::addInt(int value)
{
	char* temp = static_cast<char*>(static_cast<void*>(&value));
	addByte(*temp);
	addByte(*(temp + 1));
}

void BluetoothCommunication::addFloat(float value)
{
	char* temp = static_cast<char*>(static_cast<void*>(&value));
	addByte(*temp);
	addByte(*(temp + 1));
	addByte(*(temp + 2));
	addByte(*(temp + 3));
}

//Paket lesen, welches ganz angekommen ist
bool BluetoothCommunication::readPacket(uint8_t *_packet, int size)
{
	//Ist CRC richtig, wenn nicht dann ignorieren
	crc8.reset();
	readPos = size - 1;
	char crc = ' ';
	readType<char>(_packet, size, &crc);
	for (size_t i = 1; i < size - 1; i++)
	{
		crc8.add(*(_packet + i));
	}
	if (!(crc == (char)crc8.getCRC()))
	{
		//false CRC
		return false;
	}

	//auswerten
	readPos = 1;
	char packetType = 0;
	readType<char>(_packet, size, &packetType);
	uint8_t test = packetType;
	//Verschiedene Auswertung bei verschiedenem Paket
	if ((uint8_t)packetType == (uint8_t)flutterPacketTypes::start)
	{
		float startHeight = 0;
		readType<float>(_packet, size, &startHeight);
		bool useXCTrack = false;
		readType<bool>(_packet, size, &useXCTrack);
		bool soundON = false;
		readType<bool>(_packet, size, &soundON);
		PacketHandler::StartVariometer(startHeight, useXCTrack, soundON);
	}
	else if ((uint8_t)packetType == (uint8_t)flutterPacketTypes::stop)
	{
		PacketHandler::StopVariometer();
	}
	else if((uint8_t)packetType == (uint8_t)flutterPacketTypes::welcomePacket)
	{
		PacketHandler::WelcomePacket();
	}
	else if((uint8_t)packetType == (uint8_t)flutterPacketTypes::kalmansettings)
	{
		float standHeight = 0;
		readType<float>(_packet, size, &standHeight);
		float standMPU = 0;
		readType<float>(_packet, size, &standMPU);
		float processNoise = 0;
		readType<float>(_packet, size, &processNoise);
		PacketHandler::KalmanSetting(standHeight, standMPU, processNoise);
	}
	else if((uint8_t)packetType == (uint8_t)flutterPacketTypes::soundSetting)
	{
		//auslesen aller Punkte und in array speichern
		float *toneArray = new float[15];
		for (size_t i = 0; i < 15; i++)
		{
			float temp = 0;
			readType<float>(_packet, size, &temp);
			*(toneArray + i) = temp;
		}
		PacketHandler::soundSettings(toneArray, 15);
		delete [] toneArray;
	}
	else
		return false;

	reset();
}

//lesen von Type. Dabei werden z.B. vier Bytes für ein float gelesen
template<typename T>
bool BluetoothCommunication::readType(uint8_t *_packet, int size, T *returnType)
{
	if(size - readPos < sizeof(T))
		return false;
	
	//Genial von mir :) damit werden z.B. vier Bytes in die Bytes von einem Float gespeichert
	memcpy(returnType, _packet+readPos, sizeof(T));
	readPos += sizeof(T);
	return true;
}
/*
bool BluetoothCommunication::readByte(uint8_t *_packet, int size, char *returnChar)
{
	if (size - readPos == 0)
		return false;

	readPos += 1;
	*returnChar = *(_packet + (readPos - 1));
	return true;
}

bool BluetoothCommunication::readBool(uint8_t *_packet, int size, bool *returnBool)
{
	if(size - readPos < sizeof(bool))
		return false;
	
	memcpy(returnBool, _packet+readPos, sizeof(bool));
	readPos += sizeof(bool);
	return true;
}

bool BluetoothCommunication::readInt(uint8_t *_packet, int size, int *returnInt)
{
	if (size - readPos < sizeof(int))
		return false;

	memcpy(returnInt, _packet+readPos, sizeof(int));
	readPos += sizeof(int);
	return true;
}

bool BluetoothCommunication::readFloat(uint8_t *_packet, int size, float *returnFloat)
{
	if (size - readPos < sizeof(float))
		return false;

	memcpy(returnFloat, _packet+readPos, sizeof(float));
	readPos += sizeof(float);
	return true;
}*/

//Der Buffer wird hier ausgegebn für das schrieben auf den Adapter
char* BluetoothCommunication::getString(int *size)
{
	buffer[indexEndBuffer] = (char)crc8.getCRC();
	indexEndBuffer++;
	//if(_arduinoPackets[buffer[1]].lengthPacket != indexEndBuffer)
		//Serial.println("Packet not correct length");
	*size = indexEndBuffer;
	crc8.reset();
	return &buffer[0];
}

void BluetoothCommunication::reset()
{
	readPos = 0;
	indexEndBuffer = 0;
	crc8.reset();
}

