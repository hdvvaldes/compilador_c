#ifndef QUAD_HPP
#define QUAD_HPP

#include <string>
using namespace std;

namespace yy {

/*
 * Cuádrupla de código intermedio de tres direcciones (TAC).
 * Formato:  resultado = arg1  op  arg2
 *
 * Ejemplos:
 *   t1 = c * d        → Quad("*", "c",  "d",  "t1")
 *   goto L1           → Quad("goto", "", "", "L1")
 *   if t1 goto L2     → Quad("if_goto", "t1", "", "L2")
 *   label L1          → Quad("label", "", "", "L1")
 *   param x           → Quad("param", "x", "", "")
 *   t0 = call suma, 2 → Quad("call", "suma", "2", "t0")
 *   return t1         → Quad("return", "t1", "", "")
 *   t2 = a[t1]        → Quad("[]", "a", "t1", "t2")
 *   a[t1] = t2        → Quad("[]=", "a", "t1", "t2")
 *   (cast) t1=float(t0)→ Quad("cast_float","t0","","t1")
 */
class Quad {
public:
    Quad() = default;
    Quad(string op, string arg1, string arg2, string res)
        : _op(op), _arg1(arg1), _arg2(arg2), _res(res) {}

    string getOp()   const { return _op;   }
    string getArg1() const { return _arg1; }
    string getArg2() const { return _arg2; }
    string getRes()  const { return _res;  }

private:
    string _op, _arg1, _arg2, _res;
};

} // namespace yy
#endif
