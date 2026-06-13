// Inclusión de bibliotecas estándar para entrada/salida
#include <fstream>
#include <iostream>
// Inclusión de archivos propios del proyecto
#include "Driver.hpp"
#include "parser.tab.hh"

// Prototipo del lexer (definido en lexer.ll)
// yyin es el archivo de entrada que leerá el lexer.
extern FILE* yyin;

namespace yy {

// Constructor de la clase Driver
Driver::Driver() {
    // Crear e inicializar el ámbito global
    pushTS();
    // Inicializar el desplazamiento (offset) global en 0
    pilaOffset.push(0);
}

// Destructor de la clase Driver
Driver::~Driver() {
    // Liberar todas las tablas de símbolos almacenadas en la pila
    while (!pilaTS.empty()) popTS();
}

// Método principal para analizar un archivo fuente
void Driver::parse(const string& filename) {

    // Abrir el archivo de entrada en modo lectura
    FILE* f = fopen(filename.c_str(), "r");

    // Verificar que el archivo se abrió correctamente
    if (!f) {
        cerr << "Error: no se puede abrir el archivo '" << filename << "'" << endl;
        exit(EXIT_FAILURE);
    }
    // Asignar el archivo al lexer
    yyin = f;

    // Crear una instancia del parser y pasarle el Driver
    yy::parser p(*this);

    // Ejecutar el análisis sintáctico
    int result = p.parse();

    // Cerrar el archivo una vez terminado el análisis
    fclose(f);

    // Si el análisis fue exitoso, imprimir los resultados
    if (result == 0)
        printResults();
}

// Imprime las estructuras generadas durante la compilación
void Driver::printResults() {
    
    // Mostrar la tabla de tipos construida durante el análisis
    cout << "\n=== TABLA DE TIPOS ===" << endl;
    tablaTipos.print();

    // Mostrar la tabla de símbolos del ámbito global
    cout << "\n=== TABLA DE SÍMBOLOS (GLOBAL) ===" << endl;
    bottomTS()->print();

    // Mostrar el código intermedio generado
    tac.print();
}

} // namespace yy
