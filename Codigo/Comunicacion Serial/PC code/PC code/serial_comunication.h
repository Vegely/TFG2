#ifndef _SERIAL_COMUNICATION__HPP_
#define _SERIAL_COMUNICATION__HPP_
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <vector>
#include <string>
#include <iostream>

class SerialComunication
{
private:
	std::vector<std::pair<int, std::wstring>> availablePorts;
	bool setAvailablePorts();
public:
	SerialComunication() { setAvailablePorts(); }
	bool updateAvailablePorts();
	void displayAvaileblePorts(std::wostream& out = std::wcout);
	std::vector<std::pair<int, std::wstring>> getAvailablePorts();
	bool opemPort(int port);
};
#endif // !_SERIAL_COMUNICATION__HPP_
