#ifndef TAC_HPP
#define TAC_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Quad.hpp"
using namespace std;

namespace yy {

/*
 * Generador de código intermedio de tres direcciones.
 * Mantiene la lista de cuádruplas y los contadores de
 * temporales y etiquetas.
 */
class TAC {
public:
    TAC() = default;

    // ---------- Fábricas de nombres ----------
    string newTemp() {
        return "t" + to_string(_numTemp++);
    }

    string newLabel() {
        return "L" + to_string(_numLabel++);
    }

    // ---------- Emisión de cuádruplas ----------
    void genCode(string op, string arg1, string arg2, string res) {
        _code.push_back(Quad(op, arg1, arg2, res));
    }

    // Helpers con firma directa según las reglas del PDF
    void emitAssign(string res, string arg1) {
        genCode("=", arg1, "", res);
    }
    void emitBinop(string res, string arg1, string op, string arg2) {
        genCode(op, arg1, arg2, res);
    }
    void emitArrayRead(string res, string base, string idx) {
        genCode("[]", base, idx, res);           // res = base[idx]
    }
    void emitArrayWrite(string base, string idx, string val) {
        genCode("[]=", base, idx, val);           // base[idx] = val
    }
    void emitLabel(string lbl) {
        genCode("label", "", "", lbl);
    }
    void emitGoto(string lbl) {
        genCode("goto", "", "", lbl);
    }
    void emitIfGoto(string cond, string lbl) {
        genCode("if_goto", cond, "", lbl);        // if cond goto lbl
    }
    void emitParam(string arg) {
        genCode("param", arg, "", "");
    }
    void emitCall(string res, string func, int nArgs) {
        genCode("call", func, to_string(nArgs), res);
    }
    void emitReturn(string val) {
        genCode("return", val, "", "");
    }
    void emitCast(string res, string src, string toType) {
        genCode("cast_" + toType, src, "", res);
    }

    // ---------- Impresión ----------
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
    vector<Quad> _code;
    int _numTemp  = 0;
    int _numLabel = 0;
};

} // namespace yy
#endif
