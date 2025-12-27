#include <vector>
#include <string>
#include "SerialComunication.h"


int main(int argc, char* argv[]) {
	SerialComunication serialComunication(8);
    std::vector<std::pair<int, std::wstring>> ports = serialComunication.getAvailablePorts();
    serialComunication.displayAvaileblePorts();
    
    return 0;
}

