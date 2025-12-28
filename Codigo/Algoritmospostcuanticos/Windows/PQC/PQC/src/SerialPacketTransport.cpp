#include "SerialPacketTransport.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>

SerialPacketTransport::SerialPacketTransport(SerialComunication& s) : serial(s) {}

uint32_t SerialPacketTransport::readHeader(const std::string& data) {
    if (data.size() < 4) return 0;


    uint32_t len =
        (static_cast<unsigned char>(data[0])) |
        (static_cast<unsigned char>(data[1]) << 8) |
        (static_cast<unsigned char>(data[2]) << 16) |
        (static_cast<unsigned char>(data[3]) << 24);

    return len;
}

bool SerialPacketTransport::sendPacket(const std::vector<unsigned char>& data) {
    uint32_t len = static_cast<uint32_t>(data.size());


    std::string packet;
    packet.reserve(4 + len);


    packet.push_back(static_cast<char>(len & 0xFF));
    packet.push_back(static_cast<char>((len >> 8) & 0xFF));
    packet.push_back(static_cast<char>((len >> 16) & 0xFF));
    packet.push_back(static_cast<char>((len >> 24) & 0xFF));


    packet.append(reinterpret_cast<const char*>(data.data()), len);

    return serial.send(packet);
}

bool SerialPacketTransport::receivePacket(std::vector<unsigned char>& outData, int timeoutMs) {
    auto start = std::chrono::steady_clock::now();

    while (true) {
        std::string fragment;
        while (serial.getReceivedMessage(fragment)) {
            internalBuffer += fragment;
        }

        if (internalBuffer.size() >= 4) {
            uint32_t msgLen = readHeader(internalBuffer);

            if (msgLen > 100000) {
                internalBuffer.clear();
            }
            else if (internalBuffer.size() >= 4 + msgLen) {
                std::string payload = internalBuffer.substr(4, msgLen);
                outData.assign(payload.begin(), payload.end());
                internalBuffer.erase(0, 4 + msgLen);

                return true;
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeoutMs) {
            return false; 
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}