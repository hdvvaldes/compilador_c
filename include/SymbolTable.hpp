#ifndef SYMBOL_TABLE_HPP2
#define SYMBOL_TABLE_HPP2

/* Bibliotecas utilizadas por la tabla de símbolos */
#include <vector>
#include <string>
#include <stdexcept>
#include "Symbol.hpp"
using namespace std;

namespace yy {

/*
 * Una tabla de símbolos representa un ámbito del programa.
 *
 * Cada instancia almacena los símbolos declarados dentro
 * de un mismo contexto (global, función, struct, etc.).
 *
 * La pila de ámbitos es administrada por el Driver
 * mediante la estructura pilaTS.
 */
class SymbolTable {
public:
    /* Constructor por defecto */
    SymbolTable() = default;

    /*
     * Agrega un nuevo símbolo a la tabla.
     *
     * id   -> nombre del identificador
     * dir  -> dirección o desplazamiento
     * tipo -> identificador del tipo
     * cat  -> categoría semántica
     * args -> lista de parámetros (solo funciones)
     */
    void add(string id, int dir, int tipo, string cat, vector<int> args = {}) {
        Symbol s(id, dir, tipo, cat);
        s.setArgs(args);
        _table.push_back(s);
    }

    /*
     * Verifica si un identificador existe
     * dentro del ámbito actual.
     */
    bool existe(string id) const {
        for (auto& s : _table)
            if (s.getId() == id) return true;
        return false;
    }

    /*
     * Obtiene una referencia al símbolo asociado
     * al identificador indicado.
     */
    Symbol& get(string id) {
        for (auto& s : _table)
            if (s.getId() == id) return s;
        throw runtime_error("Symbol not found: " + id);
    }

    /* Métodos de acceso rápido*/

    int  getDir(string id)  { return get(id).getDir();  }
    int  getTipo(string id) { return get(id).getTipo(); }
    string getCat(string id){ return get(id).getCat();  }
    
    /* Devuelve la lista de tipos de parámetros */
    vector<int> getArgs(string id) { return get(id).getArgs(); }

    /*
     * Devuelve todos los símbolos almacenados
     * en la tabla.
     */
    const vector<Symbol>& getAll() const { return _table; }

    /*
     * Imprime el contenido completo de la tabla
     * de símbolos en formato tabular.
     */
    void print() const {
        printf("%-15s %-6s %-6s %-8s\n", "ID", "DIR", "TIPO", "CAT");
        printf("%-15s %-6s %-6s %-8s\n", "---", "---", "----", "---");
        for (auto& s : _table)
            printf("%-15s %-6d %-6d %-8s\n",
                   s.getId().c_str(), s.getDir(),
                   s.getTipo(), s.getCat().c_str());
    }

private:

    /*
     * Contenedor principal de símbolos
     * pertenecientes al ámbito actual.
     */
    vector<Symbol> _table;
};

} // namespace yy
#endif
