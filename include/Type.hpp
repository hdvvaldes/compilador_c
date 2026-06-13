#ifndef TYPE_HPP
#define TYPE_HPP

/* Bibliotecas utilizadas por las entradas de la tabla de tipos */
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

/*
 * Representa una entrada de la tabla de tipos.
 *
 * Estructura según la DDS:
 *   (id, nombre, tamaño, tipoBase, tablaInterna)
 *
 * Casos de uso:
 *
 * Tipos básicos:
 *   tipoBase = -1
 *   tablaInterna = nullptr
 *
 * Arreglos:
 *   nombre = "array"
 *   tipoBase = identificador del tipo elemento
 *
 * Structs:
 *   nombre = "struct"
 *   tablaInterna apunta a la tabla de símbolos
 *   correspondiente a los campos de la estructura.
 */
class Type {
public:

    /* Constructor por defecto */
    Type() = default;

    /* Constructor completo de una entrada de tipo */
    Type(int id, string nombre, int tam, int tipoBase, SymbolTable* ts = nullptr)
        : _id(id), _nombre(nombre), _tam(tam), _tipoBase(tipoBase), _ts(ts) {}

    /* Getters */
    int    getId()       const { return _id;      }
    string getNombre()   const { return _nombre;  }
    int    getTam()      const { return _tam;     }
    int    getTipoBase() const { return _tipoBase;}
    SymbolTable* getTS() const { return _ts;      }

private:
    /* Identificador único del tipo */
    int    _id       = -1;
    
    /* Nombre del tipo (int, float, array, struct, etc.) */
    string _nombre;
    
    /* Tamaño en bytes ocupado por el tipo */
    int    _tam      = 0;

    /*
     * Tipo base asociado.
     * Se utiliza principalmente en arreglos.
     * Valor -1 indica que no aplica.
     */
    int    _tipoBase = -1;

    /*
     * Tabla de símbolos interna.
     * Solo se utiliza para tipos struct.
     */
    SymbolTable* _ts = nullptr;
};

} // namespace yy
#endif
