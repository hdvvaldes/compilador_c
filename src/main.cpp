#include <iostream>
#include "Driver.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: ./compiler <archivo.txt>" << std::endl;
        return 1;
    }
    yy::Driver driver;
    driver.parse(argv[1]);
    return 0;
}
