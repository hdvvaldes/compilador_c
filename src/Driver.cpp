#include <fstream>
#include <iostream>
#include "Driver.hpp"
#include "parser.tab.hh"

// Prototipo del lexer (definido en lexer.ll)
extern FILE* yyin;

namespace yy {

Driver::Driver() {
    // Inicializar ámbito global
    pushTS();
    pilaOffset.push(0);
}

Driver::~Driver() {
    while (!pilaTS.empty()) popTS();
}

void Driver::parse(const string& filename) {
    FILE* f = fopen(filename.c_str(), "r");
    if (!f) {
        cerr << "Error: no se puede abrir el archivo '" << filename << "'" << endl;
        exit(EXIT_FAILURE);
    }
    yyin = f;
    yy::parser p(*this);
    int result = p.parse();
    fclose(f);
    if (result == 0)
        printResults();
}

void Driver::printResults() {
    // Tabla de tipos
    cout << "\n=== TABLA DE TIPOS ===" << endl;
    tablaTipos.print();

    // Tabla de símbolos global
    cout << "\n=== TABLA DE SÍMBOLOS (GLOBAL) ===" << endl;
    bottomTS()->print();

    // Código intermedio
    tac.print();
}

} // namespace yy
