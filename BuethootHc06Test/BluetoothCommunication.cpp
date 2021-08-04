#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

void BluetoothCommunication::newPacket(arduinoPacketTypes packetType)
{
	reset();
	buffer[indexEndBuffer] = startByte;
	indexEndBuffer++;
	addByte((char)packetType);
}

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
	if (indexEndBuffer == sizeof(buffer) / sizeof(uint8_t))
		Serial.println("Buffer full!!");
}

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

bool BluetoothCommunication::readPacket(uint8_t *_packet, int size)
{
	crc8.reset();
	readPos = size - 1;
	char crc = ' ';
	readByte(_packet, size, &crc);
	for (size_t i = 1; i < size - 1; i++)
	{
		Serial.print((uint8_t)*(_packet+i));
		Serial.print(";");
		crc8.add(*(_packet + i));
	}
	Serial.println();
	Serial.println(String((uint8_t)crc) + "  " + String((uint8_t)crc8.getCRC()));
	Serial.println(crc == (char)crc8.getCRC());
	if (!(crc == (char)crc8.getCRC()))
		return false;

	readPos = 1;
	char packetType = 0;
	readByte(_packet, size, &packetType);
	uint8_t test = packetType;
	if ((uint8_t)packetType == (uint8_t)flutterPacketTypes::start)
	{
		float startHeight = 0;
		readFloat(_packet, size, &startHeight);
		Serial.println("StartHeight: " + String(startHeight));
		PacketHandler::StartVariometer(startHeight);
	}
	else if ((uint8_t)packetType == (uint8_t)flutterPacketTypes::stop)
	{
		Serial.println("Vario ended");
	}
	else
		return false;

	reset();
}

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
}

char* BluetoothCommunication::getString(int *size)
{
	buffer[indexEndBuffer] = (char)crc8.getCRC();
	indexEndBuffer++;
	Serial.println(String(_arduinoPackets[buffer[1]].lengthPacket) + "  " + String(indexEndBuffer));
	if(_arduinoPackets[buffer[1]].lengthPacket != indexEndBuffer)
		Serial.println("Packet not correct length");
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

