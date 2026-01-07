#include "SerialPacketTransport.h"
#include <thread>
#include <chrono>
#include <algorithm> // for std::min

SerialPacketTransport::SerialPacketTransport(SerialComunication& serialObj)
    : serial(serialObj)
{
}

bool SerialPacketTransport::sendPacket(const std::vector<unsigned char>& data) {
    if (data.size() > 65535) return false;

    uint16_t len = (uint16_t)data.size();

    std::string frame;
    frame.reserve(2 + len);
    frame.push_back((char)(len & 0xFF));
    frame.push_back((char)((len >> 8) & 0xFF));
    frame.append((char*)data.data(), data.size());

    const size_t CHUNK_SIZE = 4096;

    for (size_t i = 0; i < frame.size(); i += CHUNK_SIZE) {
        size_t currentSize = std::min(CHUNK_SIZE, frame.size() - i);
        std::string chunk = frame.substr(i, currentSize);

        if (!serial.send(chunk)) return false;


    }

    return true;
}

bool SerialPacketTransport::receivePacket(std::vector<unsigned char>& buffer, int timeoutMs) {
    buffer.clear();
    auto start = std::chrono::steady_clock::now();

    std::string header;
    while (header.size() < 2) {
        std::string chunk;
        if (serial.getReceivedMessage(chunk)) header += chunk;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count() > timeoutMs) return false;

        if (header.size() < 2) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    uint16_t packetLen = (uint8_t)header[0] | ((uint8_t)header[1] << 8);
    std::string data = header.substr(2);

    while (data.size() < packetLen) {
        std::string chunk;
        if (serial.getReceivedMessage(chunk)) data += chunk;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count() > timeoutMs) return false;

        if (data.size() < packetLen) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for (size_t i = 0; i < packetLen; i++) buffer.push_back((unsigned char)data[i]);
    return true;
}