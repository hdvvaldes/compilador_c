// Biblioteca estándar para entrada y salida
#include <iostream>
// Definición de la clase Driver del compilador
#include "Driver.hpp"

int main(int argc, char* argv[]) {
    // Verificar que se haya proporcionado un archivo de entrada
    if (argc < 2) {
        std::cerr << "Uso: ./compiler <archivo.txt>" << std::endl;
        return 1;
    }
    // Crear una instancia del controlador del compilador
    yy::Driver driver;

    // Iniciar el proceso de análisis del archivo fuente
    driver.parse(argv[1]);

    // Finalizar la ejecución correctamente
    return 0;
}
