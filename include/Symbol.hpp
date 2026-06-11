#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>
using namespace std;

namespace yy {

/*
 * Categorías válidas según la DDS:
 *   var, arg, func, struct, temp
 */
class Symbol {
public:
    Symbol() = default;
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
    string      _id;
    int         _dir  = -1;
    int         _tipo = -1;
    string      _cat;          // "var" | "arg" | "func" | "struct" | "temp"
    vector<int> _args;         // lista de tipos de parámetros (solo para func)
};

} // namespace yy
#endif
