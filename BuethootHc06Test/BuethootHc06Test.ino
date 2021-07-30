#include <SoftwareSerial.h>
#include "BluetoothCommunication.h"
#include "CRC8.h"

SoftwareSerial BTserial(2, 3); // RX | TX
BluetoothCommunication bluetooth = BluetoothCommunication();
uint8_t bluetoothRecieveBuffer[20];

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
        Serial.write(BTserial.read());
    }
 
    if (Serial.available())
    {
		char c = Serial.read();
		if(c == 's')
        {
            bluetooth.newPacket(arduinoPacketTypes::updateState);
            bluetooth.addFloat(9234.34f);
            bluetooth.addFloat(1400.0f);
            int size = 0;
            char *pointerToFirst = bluetooth.getString(&size);
            *(pointerToFirst+3) =  23;
            for (size_t i = 0; i < size; i++)
            {
                Serial.write(*(pointerToFirst+i));
                BTserial.write(*(pointerToFirst+i));
            }
        }
    }
 
}
