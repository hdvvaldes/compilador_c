#ifndef TAC_HPP
#define TAC_HPP

/* Bibliotecas necesarias para la generación de TAC */
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Quad.hpp"
using namespace std;

namespace yy {

/*
 * Generador de Código Intermedio de Tres Direcciones (TAC).
 *
 * Responsabilidades:
 * - Crear temporales y etiquetas únicas.
 * - Almacenar la secuencia de cuádruplas generadas.
 * - Proporcionar operaciones auxiliares para emitir
 *   instrucciones TAC desde las acciones semánticas.
 * - Imprimir el código intermedio en formato legible.
 */
class TAC {
public:

    /* Constructor por defecto */
    TAC() = default;

    // ---------- Fábricas de nombres ----------
    string newTemp() {
        return "t" + to_string(_numTemp++);
    }

    string newLabel() {
        return "L" + to_string(_numLabel++);
    }

    // ---------- Emisión de cuádruplas ----------
    /*
     * Inserta una cuádrupla genérica en la lista TAC.
     */
    void genCode(string op, string arg1, string arg2, string res) {
        _code.push_back(Quad(op, arg1, arg2, res));
    }

    /*
     * Emite una asignación simple.
     * res = arg1
     */
    void emitAssign(string res, string arg1) {
        genCode("=", arg1, "", res);
    }
    /*
     * Emite una operación binaria.
     * res = arg1 op arg2
     */
    void emitBinop(string res, string arg1, string op, string arg2) {
        genCode(op, arg1, arg2, res);
    }
    /*
     * Lectura de arreglo.
     * res = base[idx]
     */
    void emitArrayRead(string res, string base, string idx) {
        genCode("[]", base, idx, res);           // res = base[idx]
    }
    /*
     * Escritura de arreglo.
     * base[idx] = val
     */
    void emitArrayWrite(string base, string idx, string val) {
        genCode("[]=", base, idx, val);           // base[idx] = val
    }
    /*
     * Define una etiqueta.
     */
    void emitLabel(string lbl) {
        genCode("label", "", "", lbl);
    }
    /*
     * Salto incondicional.
     */
    void emitGoto(string lbl) {
        genCode("goto", "", "", lbl);
    }
    /*
     * Salto condicional.
     * if cond goto lbl
     */
    void emitIfGoto(string cond, string lbl) {
        genCode("if_goto", cond, "", lbl);        // if cond goto lbl
    }
    /*
     * Paso de parámetro en llamada a función.
     */
    void emitParam(string arg) {
        genCode("param", arg, "", "");
    }
    /*
     * Llamada a función.
     * res = call func, nArgs
     */
    void emitCall(string res, string func, int nArgs) {
        genCode("call", func, to_string(nArgs), res);
    }
    /*
     * Retorno de función.
     */
    void emitReturn(string val) {
        genCode("return", val, "", "");
    }
    /*
     * Conversión explícita de tipo.
     * Ejemplo: int → float o float → int.
     */
    void emitCast(string res, string src, string toType) {
        genCode("cast_" + toType, src, "", res);
    }

    // ---------- Impresión del TAC----------
    /*
     * Recorre todas las cuádruplas generadas y las
     * muestra en una representación textual legible.
     */
    void print() const {
        cout << "\n=== CÓDIGO INTERMEDIO (TAC) ===" << endl;
        for (auto& q : _code) {
            string op = q.getOp();
            if (op == "label")
                cout << q.getRes() << ":" << endl;
            else if (op == "goto")
                cout << "\tgoto " << q.getRes() << endl;
            else if (op == "if_goto")
                cout << "\tif " << q.getArg1() << " goto " << q.getRes() << endl;
            else if (op == "=")
                cout << "\t" << q.getRes() << " = " << q.getArg1() << endl;
            else if (op == "[]")
                cout << "\t" << q.getRes() << " = " << q.getArg1()
                     << "[" << q.getArg2() << "]" << endl;
            else if (op == "[]=")
                cout << "\t" << q.getArg1() << "[" << q.getArg2()
                     << "] = " << q.getRes() << endl;
            else if (op == "param")
                cout << "\tparam " << q.getArg1() << endl;
            else if (op == "call")
                cout << "\t" << q.getRes() << " = call "
                     << q.getArg1() << ", " << q.getArg2() << endl;
            else if (op == "return")
                cout << "\treturn " << q.getArg1() << endl;
            else if (op.substr(0, 5) == "cast_")
                cout << "\t" << q.getRes() << " = ("
                     << op.substr(5) << ") " << q.getArg1() << endl;
            else
                cout << "\t" << q.getRes() << " = "
                     << q.getArg1() << " " << op << " " << q.getArg2() << endl;
        }
    }

private:
    /*
     * Lista completa de cuádruplas generadas
     * durante la compilación.
     */
    vector<Quad> _code;

    /* Contador de temporales */
    int _numTemp  = 0;

    /* Contador de etiquetas */
    int _numLabel = 0;
};

} // namespace yy
#endif
