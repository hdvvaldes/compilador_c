#ifndef SYMBOL_HPP
#define SYMBOL_HPP

/* Bibliotecas utilizadas por la entrada de la tabla de símbolos */
#include <string>
#include <vector>
using namespace std;

namespace yy {

/*
 * Representa una entrada dentro de la tabla de símbolos.
 *
 * Cada símbolo almacena:
 * - Identificador.
 * - Dirección o desplazamiento.
 * - Tipo asociado.
 * - Categoría semántica.
 * - Lista de parámetros (si corresponde a una función).
 *
 * Categorías válidas según la DDS:
 *   var    -> variable
 *   arg    -> parámetro formal
 *   func   -> función
 *   struct -> estructura
 *   temp   -> temporal generado por el compilador
 */
class Symbol {
public:

    /* Constructor por defecto */
    Symbol() = default;

    /* Constructor completo de una entrada de la tabla */
    Symbol(string id, int dir, int tipo, string cat)
        : _id(id), _dir(dir), _tipo(tipo), _cat(cat) {}

    // Getters
    string getId()   const { return _id;   }
    int    getDir()  const { return _dir;  }
    int    getTipo() const { return _tipo; }
    string getCat()  const { return _cat;  }
    vector<int> getArgs() const { return _args; }

    // Setters
    void setDir(int d)          { _dir  = d; }
    void setTipo(int t)         { _tipo = t; }
    void setCat(string c)       { _cat  = c; }
    void setArgs(vector<int> a) { _args = a; }

private:
    /* Nombre del identificador */
    string      _id;

    /* Dirección o desplazamiento dentro de memoria */
    int         _dir  = -1;

    /* Identificador del tipo en la tabla de tipos */
    int         _tipo = -1;

    /* Categoría semántica del símbolo */
    string      _cat;          // "var" | "arg" | "func" | "struct" | "temp"
    vector<int> _args;         // lista de tipos de parámetros (solo para func)
};

} // namespace yy
#endif
