#ifndef PacketHandler_h
#define PacketHandler_h

class PacketHandler{
public:
    static void StartVariometer(float startHeight, bool usXCTrack);
    static void StopVariometer();
    static void WelcomePacket();
};

#endif