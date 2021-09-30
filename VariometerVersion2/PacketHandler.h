#ifndef PacketHandler_h
#define PacketHandler_h

//Hilfsklasse um von BluetoothCommunication zu .ino file zu kommne
class PacketHandler{
public:
    static void StartVariometer(float startHeight, bool usXCTrack, bool soundON);
    static void StopVariometer();
    static void WelcomePacket();
    static void soundSettings(float *toneArray, int size);
    static void KalmanSetting(float standHeight, float standMPU, float processNoise);
};

#endif