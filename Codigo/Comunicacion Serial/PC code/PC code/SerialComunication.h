#ifndef _SERIAL_COMUNICATION__HPP_
#define _SERIAL_COMUNICATION__HPP_
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <vector>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include "CircularBuffer.h"


class SerialComunication
{
private:
	std::vector<std::pair<int, std::wstring>> availablePorts;		// List of available ports
	HANDLE hSerial;													// Serial port handle
	std::atomic<bool> running;										// Flag to stop the receive loop
	std::thread receiveThread;										// Thread for receiving data
	CircularBuffer receiveBuffer;									// Buffer to store received data

	std::mutex receiveMutex;										// Mutex to protect the send function
	bool setAvailablePorts();										// Set available ports
	void receiveLoop();												// Thread to receive messages
public:
	SerialComunication(int bufferSize) :receiveBuffer(bufferSize) {setAvailablePorts(); running = false; hSerial= NULL;}
	~SerialComunication() { closePort(); stopReceiving();}
	
	bool updateAvailablePorts();
	bool openPort(int port);
	bool closePort();
	bool send(std::string message);
	bool isBufferEmpty();
	bool getReceivedMessage(std::string& message);

	void startReceiving();											// Start the receive thread
	void stopReceiving();											// Stop the receive thread
	void displayAvaileblePorts(std::wostream& out = std::wcout);

	std::vector<std::pair<int, std::wstring>> getAvailablePorts();
	
};
#endif // !_SERIAL_COMUNICATION__HPP_
