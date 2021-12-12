#ifndef PacketHandler_h
#define PacketHandler_h

class PacketHandler{
public:
    static void StartVariometer(float startHeight, bool _soundON);
    static void StopVariometer();
    static void WelcomePacket();
};

#endif