#ifndef BluetoothCommunication_h
#define BluetoothCommunication_h


#include "CRC8.h"

constexpr uint8_t startByte = 254;

enum arduinoPacketTypes : uint8_t
{
	WelcomResponsePacket = 0,
	StartVarioPacket = 1,
	updateState = 2,

};

class arduinoPackets
{
	public:
		arduinoPacketTypes type;
		int lengthPacket;
};

enum flutterPacketTypes : uint8_t
{
	welcomePacket = 0,
	start = 1,
	stop = 2,
};

class flutterPackets
{
	public:
		flutterPacketTypes type;
		int lengthPacket;
};


class BluetoothCommunication {
public:
	arduinoPackets _arduinoPackets[3]{
		//+3 für startByte + indexByte + crcByte = 3
		//bool: DPS working + bool: MPU9250 working + bool: SD-Card working + float: SD-Card full Space
		{arduinoPacketTypes::WelcomResponsePacket, (3+4+3)},
		//float: StartDruck + bool: useXCTrack + float: StartTemperatur
		{arduinoPacketTypes::StartVarioPacket, (2*4 + 3)},
		//float: Geschwindigkeit + float: neuerDruck
		{arduinoPacketTypes::updateState, (2 * 4 + 3)},
	};

	flutterPackets _flutterPackets[3]{
		{flutterPacketTypes::welcomePacket, 3},
		{flutterPacketTypes::start, (4 + 1 +3)},
		{flutterPacketTypes::stop, 3}, 
	};

	void newPacket(arduinoPacketTypes packetType);
	void addByte(char byte);
	void addBool(bool value);
	void addInt(int value);
	void addFloat(float value);

	//bool readByte(uint8_t *_packet, int size, char *returnChar);
	//bool readBool(uint8_t *_packet, int size, bool *returnBool);
	//bool readInt(uint8_t *_packet, int size, int *returnInt);
	//bool readFloat(uint8_t *_packet, int size, float *returnFloat);

	template<typename T>
	bool readType(uint8_t *_packet, int size, T *returnType);

	char* getString(int *size);
	bool readPacket(uint8_t *_packet, int size);

	void reset();
private:
	int indexEndBuffer = 0;
	int readPos;
	char buffer[20];
	CRC8 crc8;
};

#endif