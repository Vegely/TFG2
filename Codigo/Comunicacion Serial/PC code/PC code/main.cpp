#include <vector>
#include <string>
#include "serial_comunication.h"

int main(int argc, char* argv[]) {
	SerialComunication serialComunication;
    std::vector<std::pair<int, std::wstring>> ports = serialComunication.getAvailablePorts();
    serialComunication.displayAvaileblePorts();
    return 0;
}

