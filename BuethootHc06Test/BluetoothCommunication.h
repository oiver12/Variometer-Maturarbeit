#ifndef BluetoothCommunication_h
#define BluetoothCommunication_h


#include "CRC8.h"

constexpr uint8_t startByte = 254;

enum arduinoPacketTypes : uint8_t
{
	updateState = 0,
};

class arduinoPackets
{
	public:
		arduinoPacketTypes type;
		int lengthPacket;
};

enum flutterPacketTypes : uint8_t
{
	start = 0,
	stop = 1,
};

class flutterPackets
{
	public:
		flutterPacketTypes type;
		int lengthPacket;
};


class BluetoothCommunication {
public:
	arduinoPackets _arduinoPackets[1]{
		//+3 für startByte + indexByte + crcByte = 3
		{arduinoPacketTypes::updateState, (2 * 4 + 3)},
	};

	flutterPackets _flutterPackets[2]{
		{flutterPacketTypes::start, (4+3)},
		{flutterPacketTypes::stop, 3}, 
	};

	void newPacket(arduinoPacketTypes packetType);
	void addByte(char byte);
	void addBool(bool value);
	void addInt(int value);
	void addFloat(float value);

	bool readByte(uint8_t *_packet, int size, char *returnChar);
	bool readBool(uint8_t *_packet, int size, bool *returnBool);
	bool readInt(uint8_t *_packet, int size, int *returnInt);
	bool readFloat(uint8_t *_packet, int size, float *returnFloat);

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