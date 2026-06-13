#ifndef DRIVER_HPP
#define DRIVER_HPP

/* Bibliotecas estándar utilizadas por el Driver */
#include <string>
#include <vector>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <sstream>
using namespace std;

/* Componentes principales del compilador */
#include "SymbolTable.hpp"
#include "TypeTable.hpp"
#include "TAC.hpp"

namespace yy {

/*
 * Expresión: par (dir, tipo) que viaja por las reglas semánticas.
 * dir  -> nombre de temporal, literal o identificador.
 * tipo -> identificador del tipo dentro de la tabla de tipos.
 */
struct Expr {
    string dir;   // nombre del temporal, literal o identificador
    int    tipo;  // id en la tabla de tipos
    Expr() : dir(""), tipo(-1) {}
    Expr(string d, int t) : dir(d), tipo(t) {}
};

/*
 * Driver: orquesta el compilador completo.
 * Implementa todas las pilas de la DDS y expone los métodos
 * que invoca el parser.
 */
class Driver {
public:

    /* Constructor y destructor */
    Driver();
    ~Driver();
    /* Punto de entrada para analizar un archivo fuente */
    void parse(const string& filename);

    // ────────────────────────────────────────────
    //  Variables globales del DDS
    // ────────────────────────────────────────────
    int  dir = 0;               // dirección actual
    int  tipoFuncActual = -1;   // tipo de retorno de la función que se está compilando

    // ────────────────────────────────────────────
    //  Pilas DDS
    // ────────────────────────────────────────────
    stack<SymbolTable*> pilaTS;      // pila de tablas de símbolos (ámbitos)
    stack<int>          pilaOffset;  // pila de offsets (dir al entrar a nuevo ámbito)
    stack<string>       pilaBreak;   // etiqueta de salida de bucle actual
    stack<string>       pilaReturn;  // etiqueta de retorno (por si se necesita)
    stack<string>       pilaTrue;    // etiqueta true de condición
    stack<string>       pilaFalse;   // etiqueta false de condición
    stack<string>       pilaNext;    // etiqueta de continuación (after-block)

    // ────────────────────────────────────────────
    //  Tablas
    // ────────────────────────────────────────────
    TypeTable  tablaTipos;

    // ────────────────────────────────────────────
    //  Generador TAC
    // ────────────────────────────────────────────
    TAC tac;

    // ────────────────────────────────────────────
    //  API para las acciones semánticas del parser
    // ────────────────────────────────────────────

    /* Crear un nuevo ámbito */
    void pushTS()  { pilaTS.push(new SymbolTable()); }

    /* Eliminar el ámbito actual */
    void popTS()   { if (!pilaTS.empty()) { delete pilaTS.top(); pilaTS.pop(); } }

    /* Obtener la tabla de símbolos del ámbito actual */
    SymbolTable* topTS()    { return pilaTS.top();     

    /* Obtener la tabla de símbolos global */                 }
    SymbolTable* bottomTS() {
        // Devuelve el ámbito global (fondo de la pila)
        stack<SymbolTable*> tmp = pilaTS;
        while (tmp.size() > 1) tmp.pop();
        return tmp.top();
    }

    // Búsqueda en la pila (local primero, luego global)
    bool existeId(const string& id) {
        stack<SymbolTable*> tmp = pilaTS;
        while (!tmp.empty()) {
            if (tmp.top()->existe(id)) return true;
            tmp.pop();
        }
        return false;
    }

    /*
     * Obtener el tipo asociado a un identificador.
     * Lanza una excepción si no existe.
     */
    int getTipoId(const string& id) {
        stack<SymbolTable*> tmp = pilaTS;
        while (!tmp.empty()) {
            if (tmp.top()->existe(id)) return tmp.top()->getTipo(id);
            tmp.pop();
        }
        throw runtime_error("ID no declarado: " + id);
    }

    // Temporales y etiquetas
    string newTemp()  { return tac.newTemp();  }
    string newLabel() { return tac.newLabel(); }

    /*
     * Amplía un valor a un tipo de mayor capacidad.
     * Ejemplo: int → float.
     */
    string ampliar(const string& srcDir, int srcTipo, int dstTipo) {
        if (srcTipo == dstTipo) return srcDir;
        // int → float
        if (srcTipo == 0 && dstTipo == 1) {
            string t = newTemp();
            tac.emitCast(t, srcDir, "float");
            return t;
        }
        return srcDir; // mismo tipo u otros casos
    }

    /*
     * Reduce un valor a un tipo de menor capacidad.
     * Ejemplo: float → int.
     */
    string reducir(const string& srcDir, int srcTipo, int dstTipo) {
        if (srcTipo == dstTipo) return srcDir;
        // float → int
        if (srcTipo == 1 && dstTipo == 0) {
            string t = newTemp();
            tac.emitCast(t, srcDir, "int");
            return t;
        }
        return srcDir;
    }

    // Imprimir resultados al final
    void printResults();

    // Campos auxiliares para el parser
    int    _currentDeclType = -1;  // tipo actual en una declaración D → T L ;
    string _lastArrayBase;         // base del último acceso a arreglo (C)
    string _lastArrayIndex;        // índice calculado del último acceso (C)
    stack<string> _forInc;         // etiqueta de incremento del for

private:

    /* Ejecuta el análisis sobre un flujo de entrada */
    void parse_helper(istream& stream);
};

} // namespace yy
#endif
