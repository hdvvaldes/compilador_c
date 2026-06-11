#ifndef TYPE_TABLE_HPP
#define TYPE_TABLE_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include "Type.hpp"
#include "SymbolTable.hpp"
using namespace std;

namespace yy {

/*
 * Tabla de tipos global (singleton dentro del Driver).
 * Se pre-carga con los 4 tipos básicos al inicializar.
 *
 *   ID  Nombre   Tam   TipoBase
 *   0   int      4     -1
 *   1   float    8     -1
 *   2   bool     1     -1
 *   3   char     1     -1
 *   4+  array/struct  dinámico
 */
class TypeTable {
public:
    TypeTable() {
        // Tipos básicos predefinidos
        _types.push_back(Type(0, "int",   4, -1));
        _types.push_back(Type(1, "float", 8, -1));
        _types.push_back(Type(2, "bool",  1, -1));
        _types.push_back(Type(3, "char",  1, -1));
    }

    // Obtener id de un tipo básico por nombre
    int getId(string nombre) const {
        for (auto& t : _types)
            if (t.getNombre() == nombre) return t.getId();
        throw runtime_error("Type not found: " + nombre);
    }

    bool existe(string nombre) const {
        for (auto& t : _types)
            if (t.getNombre() == nombre) return true;
        return false;
    }

    // Agregar tipo arreglo: add(dim, tipoBase, "array")
    int add(int dim, int tipoBase, string cat) {
        int newId = (int)_types.size();
        if (cat == "array") {
            // tam del arreglo = dim * tam del tipo base
            int baseTam = getTam(tipoBase);
            _types.push_back(Type(newId, "array", dim * baseTam, tipoBase));
        }
        return newId;
    }

    // Agregar tipo struct: add(nombre, "struct", ts)
    int addStruct(string nombre, SymbolTable* ts) {
        int newId = (int)_types.size();
        // Calcular tamaño sumando tamaños de sus campos
        int totalTam = 0;
        for (auto& sym : ts->getAll())
            totalTam += getTam(sym.getTipo());
        _types.push_back(Type(newId, nombre, totalTam, -1, ts));
        return newId;
    }

    int getTam(int id) const {
        return getById(id).getTam();
    }

    string getNombre(int id) const {
        return getById(id).getNombre();
    }

    int getTipoBase(int id) const {
        return getById(id).getTipoBase();
    }

    SymbolTable* getTS(int id) const {
        return getById(id).getTS();
    }

    // Compatibilidad de tipos para operaciones aritméticas/relacionales
    // int y float son compatibles entre sí; lo demás debe ser igual
    bool compatibles(int t1, int t2) const {
        if (t1 == t2) return true;
        // int(0) y float(1) son compatibles
        if ((t1 == 0 || t1 == 1) && (t2 == 0 || t2 == 1)) return true;
        return false;
    }

    // Retorna el tipo "mayor" (float > int para ampliar)
    int maxTipo(int t1, int t2) const {
        if (t1 == 1 || t2 == 1) return 1; // float
        return t1;
    }

    void print() const {
        printf("%-4s %-12s %-6s %-8s\n", "ID", "NOMBRE", "TAM", "BASE");
        printf("%-4s %-12s %-6s %-8s\n", "--", "------", "---", "----");
        for (auto& t : _types)
            printf("%-4d %-12s %-6d %-8d\n",
                   t.getId(), t.getNombre().c_str(),
                   t.getTam(), t.getTipoBase());
    }

private:
    vector<Type> _types;

    const Type& getById(int id) const {
        for (auto& t : _types)
            if (t.getId() == id) return t;
        throw runtime_error("Type id not found: " + to_string(id));
    }
};

} // namespace yy
#endif
