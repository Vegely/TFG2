#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <string>
class CircularBuffer
{
private:
	int size;				// Size of the buffer
	int head;				// Head of the buffer
	int tail;				// Tail of the buffer
	int count;				// Number of elements in the buffer
	std::string* buffer;	// Buffer
public:
	CircularBuffer(int size);
	~CircularBuffer();
	
	bool push(const std::string& value);
	bool pop(std::string& value);
	bool isempty();
	bool isfull();

	int getCount();
	int getSize();
};

#endif // !CIRCULARBUFFER_H
