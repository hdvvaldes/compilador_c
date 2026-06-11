#ifndef SYMBOL_TABLE_HPP2
#define SYMBOL_TABLE_HPP2

#include <vector>
#include <string>
#include <stdexcept>
#include "Symbol.hpp"
using namespace std;

namespace yy {

/*
 * Una tabla de símbolos representa un ámbito.
 * La PilaTS es un vector<SymbolTable*> manejado en el Driver.
 */
class SymbolTable {
public:
    SymbolTable() = default;

    // Agrega un símbolo con todos sus campos
    void add(string id, int dir, int tipo, string cat, vector<int> args = {}) {
        Symbol s(id, dir, tipo, cat);
        s.setArgs(args);
        _table.push_back(s);
    }

    bool existe(string id) const {
        for (auto& s : _table)
            if (s.getId() == id) return true;
        return false;
    }

    Symbol& get(string id) {
        for (auto& s : _table)
            if (s.getId() == id) return s;
        throw runtime_error("Symbol not found: " + id);
    }

    int  getDir(string id)  { return get(id).getDir();  }
    int  getTipo(string id) { return get(id).getTipo(); }
    string getCat(string id){ return get(id).getCat();  }
    vector<int> getArgs(string id) { return get(id).getArgs(); }

    const vector<Symbol>& getAll() const { return _table; }

    void print() const {
        printf("%-15s %-6s %-6s %-8s\n", "ID", "DIR", "TIPO", "CAT");
        printf("%-15s %-6s %-6s %-8s\n", "---", "---", "----", "---");
        for (auto& s : _table)
            printf("%-15s %-6d %-6d %-8s\n",
                   s.getId().c_str(), s.getDir(),
                   s.getTipo(), s.getCat().c_str());
    }

private:
    vector<Symbol> _table;
};

} // namespace yy
#endif
