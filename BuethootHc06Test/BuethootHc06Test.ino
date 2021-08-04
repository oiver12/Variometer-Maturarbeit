#include <SoftwareSerial.h>
#include "BluetoothCommunication.h"
#include "CRC8.h"
#include "PacketHandler.h"

SoftwareSerial BTserial(2, 3); // RX | TX
BluetoothCommunication bluetooth = BluetoothCommunication();
uint8_t bluetoothRecieveBuffer[20];
int indexRecieveBuffer = 0;
bool packetStarted = false;
float startHeight;

void setup() 
{
    Serial.begin(115200);
    // HC-06 default serial speed is 9600
    BTserial.begin(9600);

   /*BluetoothCommunication bl = BluetoothCommunication();
	bl.newPacket(arduinoPacketTypes::updateState);
	bl.addFloat(10.023f);
    bl.addFloat(321.23f);
	int size = 0;
	char *first = bl.getString(&size);;
	for (size_t i = 0; i < size; i++)
	{
		Serial.print((uint8_t)*(first + i));
		Serial.print("  ");
		char test = *(first + i);
		bluetoothRecieveBuffer[i] = (uint8_t)*(first + i);
	}
	Serial.print("\n");
	bool sd = bl.readPacket(bluetoothRecieveBuffer, size);*/
}
 
void loop()
{
 
    if (BTserial.available())
    {  
        char incomingByte = BTserial.read();
        addByteToPacket(incomingByte);
        Serial.println(String((uint8_t)incomingByte) + " Byte");
    }
 
    if (Serial.available())
    {
		char c = Serial.read();
		if(c == 's')
        {
            /*bluetooth.newPacket(arduinoPacketTypes::updateState);
            bluetooth.addFloat(9234.34f);
            bluetooth.addFloat(1400.0f);
            sendPacket();*/
            uint8_t first = startByte;
            uint8_t second = 5;
            BTserial.write(first);
            BTserial.write(second);
        }
    }
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
    bluetooth.newPacket(arduinoPacketTypes::startPacket);
    bluetooth.addFloat(83942.43f);
    bluetooth.addFloat(15.23f);
    sendPacket();
    startHeight = height;
}
