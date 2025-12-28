#include "CircularBuffer.h"
CircularBuffer::CircularBuffer(int size) { 
	buffer = new std::string[size];
	head = 0;
	tail = 0;
	this->size = size;
	count = 0; 
};

CircularBuffer::~CircularBuffer() {
	delete[] buffer;
}

int CircularBuffer::getSize() {
	return size;
}
int CircularBuffer::getCount() {
	return count;
}

bool CircularBuffer::push(const std::string& value) {
    if (count == size) {
        return false;
    }
    buffer[tail] = value;
    tail = (tail + 1) % size;
    count++;
    return true;
}

bool CircularBuffer::pop(std::string& value) {
    if (count == 0) {
        return false;
    }
    value = buffer[head];
    head = (head + 1) % size;
    count--;
    return true;
}

bool CircularBuffer::isempty() {
	return count == 0;
}

bool CircularBuffer::isfull() {
	return count == size;
}