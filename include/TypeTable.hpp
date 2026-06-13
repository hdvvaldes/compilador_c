#ifndef TYPE_TABLE_HPP
#define TYPE_TABLE_HPP

/* Bibliotecas necesarias para la gestión de tipos */
#include <vector>
#include <string>
#include <stdexcept>
#include "Type.hpp"
#include "SymbolTable.hpp"
using namespace std;

namespace yy {

/*
 * Tabla global de tipos del compilador.
 *
 * Almacena los tipos básicos y todos los tipos
 * compuestos (arreglos y structs) creados durante
 * el análisis semántico.
 *
 * Tipos predefinidos:
 *
 * ID   Nombre   Tamaño
 * --------------------
 * 0    int      4
 * 1    float    8
 * 2    bool     1
 * 3    char     1
 *
 * Los identificadores a partir de 4 corresponden
 * a tipos creados dinámicamente.
 */

class TypeTable {
public:

    /*
     * Constructor.
     * Inicializa la tabla con los tipos básicos.
     */
    TypeTable() {
        // Tipos básicos predefinidos
        _types.push_back(Type(0, "int",   4, -1));
        _types.push_back(Type(1, "float", 8, -1));
        _types.push_back(Type(2, "bool",  1, -1));
        _types.push_back(Type(3, "char",  1, -1));
    }

    /*
     * Obtiene el identificador asociado a un tipo
     * a partir de su nombre.
     */
    int getId(string nombre) const {
        for (auto& t : _types)
            if (t.getNombre() == nombre) return t.getId();
        throw runtime_error("Type not found: " + nombre);
    }

    /*
     * Verifica si un tipo ya existe en la tabla.
     */
    bool existe(string nombre) const {
        for (auto& t : _types)
            if (t.getNombre() == nombre) return true;
        return false;
    }

    /*
     * Agrega un tipo arreglo.
     *
     * El tamaño total se calcula como:
     *      dimensión × tamaño(tipoBase)
     */
    int add(int dim, int tipoBase, string cat) {
        int newId = (int)_types.size();
        if (cat == "array") {
            // tam del arreglo = dim * tam del tipo base
            int baseTam = getTam(tipoBase);
            _types.push_back(Type(newId, "array", dim * baseTam, tipoBase));
        }
        return newId;
    }

    /*
     * Agrega un nuevo tipo struct.
     *
     * El tamaño total del struct se obtiene
     * sumando los tamaños de todos sus campos.
     */
    int addStruct(string nombre, SymbolTable* ts) {
        int newId = (int)_types.size();
        // Calcular tamaño sumando tamaños de sus campos
        int totalTam = 0;
        for (auto& sym : ts->getAll())
            totalTam += getTam(sym.getTipo());
        _types.push_back(Type(newId, nombre, totalTam, -1, ts));
        return newId;
    }

    // Consultas de propiedades de tipos
    int getTam(int id) const {
        return getById(id).getTam();
    }

    string getNombre(int id) const {
        return getById(id).getNombre();
    }

    int getTipoBase(int id) const {
        return getById(id).getTipoBase();
    }

    /*
     * Devuelve la tabla interna asociada
     * a un tipo struct.
     */
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

    /*
     * Obtiene el tipo resultante de una operación.
     *
     * Regla implementada:
     *      float > int
     *
     * Si alguno de los operandos es float,
     * el resultado será float.
     */
    int maxTipo(int t1, int t2) const {
        if (t1 == 1 || t2 == 1) return 1; // float
        return t1;
    }

    /*
     * Imprime el contenido completo de la tabla
     * de tipos en formato tabular.
     */
    void print() const {
        printf("%-4s %-12s %-6s %-8s\n", "ID", "NOMBRE", "TAM", "BASE");
        printf("%-4s %-12s %-6s %-8s\n", "--", "------", "---", "----");
        for (auto& t : _types)
            printf("%-4d %-12s %-6d %-8d\n",
                   t.getId(), t.getNombre().c_str(),
                   t.getTam(), t.getTipoBase());
    }

private:

    /*
     * Contenedor principal de los tipos
     * registrados en el compilador.
     */
    vector<Type> _types;

    /*
     * Busca un tipo por identificador.
     */
    const Type& getById(int id) const {
        for (auto& t : _types)
            if (t.getId() == id) return t;
        throw runtime_error("Type id not found: " + to_string(id));
    }
};

} // namespace yy
#endif
