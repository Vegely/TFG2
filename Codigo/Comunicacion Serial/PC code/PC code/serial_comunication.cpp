#include "serial_comunication.h"
#include <iostream>
#include <vector>
#include <string>
#include <setupapi.h>
#include <devguid.h>

bool SerialComunication::setAvailablePorts() {
    std::vector<std::pair<int, std::wstring>> portList;
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: Unable to get device information set." << std::endl;
        return -1;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        // Get friendly name (e.g., "Arduino Mega 2560 (COM4)")
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
    return 1;
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

/*ARduino test code
   // Open serial port
   HANDLE hSerial = CreateFile(L"COM4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
   if (hSerial == INVALID_HANDLE_VALUE) {
       std::cerr << "Error: Unable to open COM port." << std::endl;
       return 1;
   }

   // Configure serial port
   DCB dcbSerialParams = { 0 };
   dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
   GetCommState(hSerial, &dcbSerialParams);
   dcbSerialParams.BaudRate = CBR_9600;
   dcbSerialParams.ByteSize = 8;
   dcbSerialParams.StopBits = ONESTOPBIT;
   dcbSerialParams.Parity = NOPARITY;
   SetCommState(hSerial, &dcbSerialParams);

   // Set timeouts
   COMMTIMEOUTS timeouts = { 0 };
   timeouts.ReadIntervalTimeout = 50;
   timeouts.ReadTotalTimeoutConstant = 50;
   timeouts.ReadTotalTimeoutMultiplier = 10;
   timeouts.WriteTotalTimeoutConstant = 50;
   timeouts.WriteTotalTimeoutMultiplier = 10;
   SetCommTimeouts(hSerial, &timeouts);

   std::cout << "Connected to COM4. Type 'exit' to quit." << std::endl;

   char input[100];

   while (true) {
       std::cout << "\nEnter message to send: ";
       std::cin.getline(input, sizeof(input));

       if (strcmp(input, "exit") == 0)
           break;

       // Append newline like VS Code does
       std::string message = std::string(input) + "\r\n";

       // Write to serial port
       DWORD bytesWritten;
       if (!WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, NULL)) {
           std::cerr << "Error: Failed to write to serial port." << std::endl;
           continue;
       }

       // Ensure data is flushed to the device
       FlushFileBuffers(hSerial);

       // Read response
       char buffer[256] = { 0 };
       DWORD bytesRead;
       if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
           buffer[bytesRead] = '\0';  // Null-terminate the string
           std::cout << "Received: " << buffer << std::endl;
       }
       else {
           std::cout << "No response received." << std::endl;
       }
   }

   CloseHandle(hSerial);
   std::cout << "Serial communication ended." << std::endl;*/