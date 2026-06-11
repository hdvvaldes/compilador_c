#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <vector>
using namespace std;

namespace yy {

/*
 * Entrada de la tabla de tipos según la DDS:
 *   (id, nombre, tamaño, tipoBase, tablaInterna)
 *
 * Para tipos básicos:  tipoBase = -1, tablaInterna = nullptr
 * Para arreglos:       nombre = "array", tipoBase = id del tipo elemento
 * Para structs:        nombre = "struct", tablaInterna apunta a su TS
 */
class SymbolTable;   // forward declaration

class Type {
public:
    Type() = default;
    Type(int id, string nombre, int tam, int tipoBase, SymbolTable* ts = nullptr)
        : _id(id), _nombre(nombre), _tam(tam), _tipoBase(tipoBase), _ts(ts) {}

    int    getId()       const { return _id;      }
    string getNombre()   const { return _nombre;  }
    int    getTam()      const { return _tam;     }
    int    getTipoBase() const { return _tipoBase;}
    SymbolTable* getTS() const { return _ts;      }

private:
    int    _id       = -1;
    string _nombre;
    int    _tam      = 0;
    int    _tipoBase = -1;       // -1 = no aplica
    SymbolTable* _ts = nullptr;  // solo structs
};

} // namespace yy
#endif
