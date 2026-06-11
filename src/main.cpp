#include <iostream>
#include "parser.tab.hh"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " archivo.txt" << std::endl;
        return 1;
    }

    // Aquí se va a redirigir el archivo de entrada al Lexer de Flex más adelante.

    yy::parser compilador;
    std::cout << "Iniciando análisis del compilador..." << std::endl;

    if (compilador.parse() == 0) {
        std::cout << "Compilación exitosa (Análisis Léxico y Sintáctico correcto)." << std::endl;
    } else {
        std::cerr << "Proceso de compilación fallido." << std::endl;
        return 1;
    }

    return 0;
}
