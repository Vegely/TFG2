#include "SerialComunication.h"
#include <iostream>
#include <vector>
#include <string>
#include <setupapi.h>
#include <devguid.h>
#include <thread>
#include <mutex>

bool SerialComunication::setAvailablePorts() {
    std::vector<std::pair<int, std::wstring>> portList;
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: Unable to get device information set." << std::endl;
        return false;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        // Get friendly name 
        wchar_t friendlyName[256];
        if (SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, nullptr,
            (PBYTE)friendlyName, sizeof(friendlyName), nullptr))
        {
            std::wstring name(friendlyName);

            // Find the COM port number in the name
            size_t pos = name.find(L"(COM");
            if (pos != std::wstring::npos)
            {
                int comPort;
                swscanf_s(name.c_str() + pos + 4, L"%d", &comPort); // Extract COM number
                portList.push_back(std::make_pair(comPort, name));
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    availablePorts = portList;
    return true;
}

std::vector<std::pair<int, std::wstring>> SerialComunication::getAvailablePorts() {
    return availablePorts;
}

void SerialComunication::displayAvaileblePorts(std::wostream& out ) {
    for (std::pair<int, std::wstring>& port : availablePorts)
    {
        out << L"COM" << port.first << L": " << port.second << std::endl;
    }
    if (availablePorts.size() == 0) {
		out << L"No available ports." << std::endl;
    }
}

bool SerialComunication::updateAvailablePorts() {
    return setAvailablePorts();
}

bool SerialComunication::openPort(int port) {
    std::wstring portName = L"\\\\.\\COM" + std::to_wstring(port);  // Ensures compatibility with high COM numbers

    hSerial = CreateFile(
        portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,                     // No sharing
        NULL,                  // Default security
        OPEN_EXISTING,         // Open existing port only
        0,                     // Non-overlapped mode (change to FILE_FLAG_OVERLAPPED if needed)
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open COM port " << port << ". Error code: " << GetLastError() << std::endl;
        return false;
    }

    // Configure serial port settings
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: Unable to get COM port state." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // Set serial port parameters
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;  

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: Unable to set COM port state." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    // Set timeouts to prevent blocking
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;         // Max time between bytes (ms)
    timeouts.ReadTotalTimeoutMultiplier = 10;  // Multiplier per byte
    timeouts.ReadTotalTimeoutConstant = 50;    // Constant time added
    timeouts.WriteTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error: Unable to set COM port timeouts." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    std::cout << "Successfully opened COM" << port << std::endl;
    return true;
}

bool SerialComunication::closePort() {
    if (hSerial != NULL) {
        if (CloseHandle(hSerial)) {
            hSerial = NULL;
            return true;
        }
        else {
            std::cerr << "Error: Unable to close COM port." << std::endl;
            return false;
        }
    }
    return true;
}

bool SerialComunication::send(std::string message) {
	DWORD bytesWritten;
	if (!WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, NULL)) {
		std::cerr << "Error: Failed to write to serial port." << std::endl;
		return false;
	}
	// Ensure data is flushed to the device
	FlushFileBuffers(hSerial);
	return true;
}


void SerialComunication::startReceiving() {
    if (!running) {
        running = true;
        receiveThread = std::thread(&SerialComunication::receiveLoop, this);
    }
}

void SerialComunication::stopReceiving() {
    if (running) {
        running = false;
        if (receiveThread.joinable()) {
            receiveThread.join(); 
        }
    }
}

void SerialComunication::receiveLoop() {
    if (hSerial == NULL || hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Serial port not open for reading." << std::endl;
        return;
    }

    const int bufferSize = 256;
    char buffer[bufferSize] = { 0 };
    DWORD bytesRead = 0;

    while (running) {
        // Attempt to read from the serial port
        if (ReadFile(hSerial, buffer, bufferSize - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {

                std::lock_guard<std::mutex> lock(receiveMutex);
                std::string safeData(buffer, bytesRead);
                receiveBuffer.push(safeData);
            }
        }
        else {
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING) {
                std::cerr << "Error: Failed to read from serial port. Error code: " << error << std::endl;
                break;
            }
        }

        // Prevent busy looping, allowing other threads to run
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


bool SerialComunication::getReceivedMessage(std::string& message) {
    std::lock_guard<std::mutex> lock(receiveMutex);
    if (receiveBuffer.pop(message)) {
        return true;
    }
    return false;
}

bool SerialComunication::isBufferEmpty() {
    std::lock_guard<std::mutex> lock(receiveMutex);
    return receiveBuffer.isempty();
}