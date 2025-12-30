#ifndef SERIAL_PACKET_TRANSPORT_H
#define SERIAL_PACKET_TRANSPORT_H

#include "SerialComunication.h"
#include <vector>
#include <string>
#include <cstdint>

class SerialPacketTransport {
private:
    SerialComunication& serial;
    std::string internalBuffer; 


public:
    SerialPacketTransport(SerialComunication& s);
    bool sendPacket(const std::vector<unsigned char>& data);
    bool receivePacket(std::vector<unsigned char>& outData, int timeoutMs = 3000);
};

#endif // SERIAL_PACKET_TRANSPORT_H