%skeleton "lalr1.cc"
%require "3.0"

%defines
%define api.namespace {yy}
%define api.value.type variant
%define api.token.constructor

%code requires{
    #include <string>
    #include <vector>
    #include "Driver.hpp"
    using namespace std;
}

%param { yy::Driver& driver }

%code{
    #include <iostream>
    #include <stdexcept>

    // Prototipo del lexer para Bison C++
    #define YY_DECL yy::parser::symbol_type yylex(yy::Driver& driver)
    YY_DECL;
}

/* Definición de Tokens */
%token INT FLOAT CHAR BOOL STRUCT DEF IF ELSE WHILE FOR BREAK RETURN
%token LKEY RKEY LBRACKET RBRACKET LPAREN RPAREN SEMICOLON COMMA DOT
%token ASSIGN PLUS MINUS MULT DIV MOD AND OR EQ NE LT GT LE GE
%token TRUE FALSE

%token <string> ID
%token <int> INTLIT
%token <float> FLOATLIT
%token <char> CHARLIT

/* Tipos de no-terminales */
%type <int>         T B
%type <yy::Expr>    E C Z
%type <vector<int>> F G N M

%start P

/* Precedencias lógicas y aritméticas (Resuelven los conflictos de expresiones) */
%right ASSIGN
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MULT DIV MOD
%nonassoc ELSE_PREC
%nonassoc ELSE

%%

/* ═══════════════════════════════════════════════════
   P  →  H
   ═══════════════════════════════════════════════════ */
P : { driver.dir = 0; } H ;

H : D H | %empty ;

/* ═══════════════════════════════════════════════════
   D  →  T L ;
        | struct id { BLOQUE } ;
        | def T id ( F ) { BLOQUE }
   ═══════════════════════════════════════════════════ */
D : T[t] { driver._currentDeclType = $t; } L SEMICOLON

  | STRUCT ID[nombre] LKEY
    {
        if (driver.bottomTS()->existe($nombre))
            cerr << "Error semántico: struct '" << $nombre << "' duplicado." << endl;
        driver.pilaTS.push(new yy::SymbolTable());
        driver.pilaOffset.push(driver.dir);
        driver.dir = 0;
    }
    BLOQUE RKEY SEMICOLON
    {
        yy::SymbolTable* ts = driver.pilaTS.top();
        driver.pilaTS.pop();
        int tipoS = driver.tablaTipos.addStruct($nombre, ts);
        driver.dir = driver.pilaOffset.top();
        driver.pilaOffset.pop();
        driver.bottomTS()->add($nombre, -1, tipoS, "struct");
    }

  | DEF T[ret] ID[nombre] LPAREN
    {
        if (driver.bottomTS()->existe($nombre))
            cerr << "Error semántico: función '" << $nombre << "' duplicada." << endl;
        driver.pilaTS.push(new yy::SymbolTable());
        driver.pilaOffset.push(driver.dir);
        driver.dir = 0;
        driver.tipoFuncActual = $ret;
        driver.tac.emitLabel($nombre);
    }
    F[params] RPAREN LKEY BLOQUE RKEY
    {
        string lEnd = driver.newLabel();
        driver.tac.emitLabel(lEnd);
        driver.pilaTS.pop();
        driver.dir = driver.pilaOffset.top();
        driver.pilaOffset.pop();
        driver.bottomTS()->add($nombre, -1, $ret, "func", $params);
    }
  ;

BLOQUE : D BLOQUE | S BLOQUE | %empty ;

/* ═══════════════════════════════════════════════════
   T  →  B  (con soporte de arreglos en dimensiones)
   Estrategia: acumulamos las dimensiones en T mismo.
   ═══════════════════════════════════════════════════ */
T : B[b] DIMS[dims]
    {
        /* Construir el tipo de arreglo de adentro hacia afuera */
        $$ = $b;
        /* dims es vector<int> de dimensiones [d1, d2, ...] */
        /* Construimos el tipo en orden inverso */
        for (int i = (int)$dims.size() - 1; i >= 0; i--) {
            $$ = driver.tablaTipos.add($dims[i], $$, "array");
        }
    }
  ;

%type <vector<int>> DIMS;
DIMS : LBRACKET INTLIT[n] RBRACKET DIMS[rest]
       {
           $$ = $rest;
           $$.insert($$.begin(), $n);
       }
     | %empty { $$ = vector<int>(); }
     ;

B : INT   { $$ = driver.tablaTipos.getId("int");   }
  | FLOAT { $$ = driver.tablaTipos.getId("float"); }
  | BOOL  { $$ = driver.tablaTipos.getId("bool");  }
  | CHAR  { $$ = driver.tablaTipos.getId("char");  }
  | ID[nombre]
    {
        if (!driver.tablaTipos.existe($nombre)) {
            cerr << "Error semántico: tipo '" << $nombre << "' no declarado." << endl;
            $$ = -1;
        } else {
            $$ = driver.tablaTipos.getId($nombre);
        }
    }
  ;

/* ═══════════════════════════════════════════════════
   L  →  lista de identificadores (tipo en _currentDeclType)
   ═══════════════════════════════════════════════════ */
L : L COMMA ID[nombre]
    {
        int tipo = driver._currentDeclType;
        if (driver.pilaTS.top()->existe($nombre))
            cerr << "Error semántico: variable '" << $nombre << "' duplicada." << endl;
        else {
            driver.pilaTS.top()->add($nombre, driver.dir, tipo, "var");
            driver.dir += driver.tablaTipos.getTam(tipo);
        }
    }
  | ID[nombre]
    {
        int tipo = driver._currentDeclType;
        if (driver.pilaTS.top()->existe($nombre))
            cerr << "Error semántico: variable '" << $nombre << "' duplicada." << endl;
        else {
            driver.pilaTS.top()->add($nombre, driver.dir, tipo, "var");
            driver.dir += driver.tablaTipos.getTam(tipo);
        }
    }
  ;

/* ═══════════════════════════════════════════════════
   F / G  — parámetros formales
   ═══════════════════════════════════════════════════ */
F : G[g] { $$ = $g; }
  | %empty { $$ = vector<int>(); }
  ;

G : G[g] COMMA T[t] ID[nombre]
    {
        if (driver.pilaTS.top()->existe($nombre))
            cerr << "Error semántico: parámetro '" << $nombre << "' duplicado." << endl;
        else {
            driver.pilaTS.top()->add($nombre, driver.dir, $t, "arg");
            driver.dir += driver.tablaTipos.getTam($t);
        }
        $$ = $g; $$.push_back($t);
    }
  | T[t] ID[nombre]
    {
        if (driver.pilaTS.top()->existe($nombre))
            cerr << "Error semántico: parámetro '" << $nombre << "' duplicado." << endl;
        else {
            driver.pilaTS.top()->add($nombre, driver.dir, $t, "arg");
            driver.dir += driver.tablaTipos.getTam($t);
        }
        $$ = vector<int>(); $$.push_back($t);
    }
  ;

/* ═══════════════════════════════════════════════════
   S  — sentencias
   ═══════════════════════════════════════════════════ */
S : BREAK SEMICOLON
    {
        if (driver.pilaBreak.empty())
            cerr << "Error semántico: 'break' fuera de bucle." << endl;
        else
            driver.tac.emitGoto(driver.pilaBreak.top());
    }

  | RETURN E[e] SEMICOLON
    {
        if (!driver.tablaTipos.compatibles(driver.tipoFuncActual, $e.tipo))
            cerr << "Error semántico: tipo de retorno incorrecto." << endl;
        else {
            string val = driver.reducir($e.dir, $e.tipo, driver.tipoFuncActual);
            driver.tac.emitReturn(val);
        }
    }

  | ID[id] ASSIGN E[e] SEMICOLON
    {
        if (!driver.existeId($id)) {
            cerr << "Error semántico: '" << $id << "' no declarado." << endl;
        } else {
            int tipo = driver.getTipoId($id);
            if (!driver.tablaTipos.compatibles(tipo, $e.tipo))
                cerr << "Error semántico: tipos incompatibles en asignación a '" << $id << "'." << endl;
            else {
                string val = driver.reducir($e.dir, $e.tipo, tipo);
                driver.tac.emitAssign($id, val);
            }
        }
    }

  | C[c] ASSIGN E[e] SEMICOLON
    {
        if (!driver.tablaTipos.compatibles($c.tipo, $e.tipo))
            cerr << "Error semántico: tipos incompatibles en asignación a arreglo." << endl;
        else {
            string val = driver.reducir($e.dir, $e.tipo, $c.tipo);
            driver.tac.emitArrayWrite($c.dir, driver._lastArrayIndex, val);
        }
    }

  | Z[z] ASSIGN E[e] SEMICOLON
    {
        if (!driver.tablaTipos.compatibles($z.tipo, $e.tipo))
            cerr << "Error semántico: tipos incompatibles en asignación a struct." << endl;
        else {
            string val = driver.reducir($e.dir, $e.tipo, $z.tipo);
            driver.tac.emitArrayWrite($z.dir, "0", val);
        }
    }

  | IF LPAREN E[e] RPAREN
    {
        string lTrue  = driver.newLabel();
        string lFalse = driver.newLabel();
        driver.pilaTrue.push(lTrue);
        driver.pilaFalse.push(lFalse);
        driver.tac.emitIfGoto($e.dir, lTrue);
        driver.tac.emitGoto(lFalse);
        driver.tac.emitLabel(lTrue);
    }
    S
    {
        driver.tac.emitLabel(driver.pilaFalse.top());
        driver.pilaTrue.pop(); driver.pilaFalse.pop();
    } %prec ELSE_PREC

  | IF LPAREN E[e] RPAREN
    {
        string lTrue  = driver.newLabel();
        string lFalse = driver.newLabel();
        string lNext  = driver.newLabel();
        driver.pilaTrue.push(lTrue);
        driver.pilaFalse.push(lFalse);
        driver.pilaNext.push(lNext);
        driver.tac.emitIfGoto($e.dir, lTrue);
        driver.tac.emitGoto(lFalse);
        driver.tac.emitLabel(lTrue);
    }
    S ELSE
    {
        driver.tac.emitGoto(driver.pilaNext.top());
        driver.tac.emitLabel(driver.pilaFalse.top());
    }
    S
    {
        driver.tac.emitLabel(driver.pilaNext.top());
        driver.pilaTrue.pop(); driver.pilaFalse.pop(); driver.pilaNext.pop();
    }

  | WHILE LPAREN
    {
        string lNext  = driver.newLabel();
        string lTrue  = driver.newLabel();
        string lFalse = driver.newLabel();
        driver.pilaNext.push(lNext);
        driver.pilaTrue.push(lTrue);
        driver.pilaFalse.push(lFalse);
        driver.pilaBreak.push(lFalse);
        driver.tac.emitLabel(lNext);
    }
    E[e] RPAREN
    {
        driver.tac.emitIfGoto($e.dir, driver.pilaTrue.top());
        driver.tac.emitGoto(driver.pilaFalse.top());
        driver.tac.emitLabel(driver.pilaTrue.top());
    }
    S
    {
        driver.tac.emitGoto(driver.pilaNext.top());
        driver.tac.emitLabel(driver.pilaFalse.top());
        driver.pilaTrue.pop(); driver.pilaFalse.pop();
        driver.pilaNext.pop(); driver.pilaBreak.pop();
    }

  | FOR LPAREN
    {
        string lNext  = driver.newLabel();
        string lTrue  = driver.newLabel();
        string lFalse = driver.newLabel();
        string lInc   = driver.newLabel();
        driver.pilaNext.push(lNext);
        driver.pilaTrue.push(lTrue);
        driver.pilaFalse.push(lFalse);
        driver._forInc.push(lInc);
        driver.pilaBreak.push(lFalse);
    }
    S[init] SEMICOLON
    { driver.tac.emitLabel(driver.pilaNext.top()); }
    E[cond] SEMICOLON
    {
        driver.tac.emitIfGoto($cond.dir, driver.pilaTrue.top());
        driver.tac.emitGoto(driver.pilaFalse.top());
        driver.tac.emitLabel(driver._forInc.top());
    }
    S[inc] RPAREN
    {
        driver.tac.emitGoto(driver.pilaNext.top());
        driver.tac.emitLabel(driver.pilaTrue.top());
    }
    S[body]
    {
        driver.tac.emitGoto(driver._forInc.top());
        driver.tac.emitLabel(driver.pilaFalse.top());
        driver.pilaTrue.pop(); driver.pilaFalse.pop();
        driver.pilaNext.pop(); driver._forInc.pop();
        driver.pilaBreak.pop();
    }

  | LKEY BLOQUE RKEY
  ;

/* ═══════════════════════════════════════════════════
   E  — expresiones con semántica completa
   ═══════════════════════════════════════════════════ */
E : E[e1] PLUS E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo, $e2.tipo)) {
            cerr << "Error semántico: tipos incompatibles en '+'." << endl; $$ = $e1;
        } else {
            int tipo = driver.tablaTipos.maxTipo($e1.tipo, $e2.tipo);
            string t = driver.newTemp();
            driver.tac.emitBinop(t,
                driver.ampliar($e1.dir, $e1.tipo, tipo), "+",
                driver.ampliar($e2.dir, $e2.tipo, tipo));
            $$ = {t, tipo};
        }
    }
  | E[e1] MINUS E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo, $e2.tipo)) {
            cerr << "Error semántico: tipos incompatibles en '-'." << endl; $$ = $e1;
        } else {
            int tipo = driver.tablaTipos.maxTipo($e1.tipo, $e2.tipo);
            string t = driver.newTemp();
            driver.tac.emitBinop(t,
                driver.ampliar($e1.dir,$e1.tipo,tipo), "-",
                driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tipo};
        }
    }
  | E[e1] MULT E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo, $e2.tipo)) {
            cerr << "Error semántico: tipos incompatibles en '*'." << endl; $$ = $e1;
        } else {
            int tipo = driver.tablaTipos.maxTipo($e1.tipo, $e2.tipo);
            string t = driver.newTemp();
            driver.tac.emitBinop(t,
                driver.ampliar($e1.dir,$e1.tipo,tipo), "*",
                driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tipo};
        }
    }
  | E[e1] DIV E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo, $e2.tipo)) {
            cerr << "Error semántico: tipos incompatibles en '/'." << endl; $$ = $e1;
        } else {
            int tipo = driver.tablaTipos.maxTipo($e1.tipo, $e2.tipo);
            string t = driver.newTemp();
            driver.tac.emitBinop(t,
                driver.ampliar($e1.dir,$e1.tipo,tipo), "/",
                driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tipo};
        }
    }
  | E[e1] MOD E[e2]
    {
        int tInt = driver.tablaTipos.getId("int");
        if ($e1.tipo != tInt || $e2.tipo != tInt) {
            cerr << "Error semántico: '%' requiere enteros." << endl; $$ = $e1;
        } else {
            string t = driver.newTemp();
            driver.tac.emitBinop(t, $e1.dir, "%", $e2.dir);
            $$ = {t, tInt};
        }
    }
  | E[e1] OR E[e2]
    {
        int tBool = driver.tablaTipos.getId("bool");
        if ($e1.tipo != tBool || $e2.tipo != tBool) {
            cerr << "Error semántico: '||' requiere bool." << endl; $$ = $e1;
        } else {
            string t = driver.newTemp();
            driver.tac.emitBinop(t, $e1.dir, "||", $e2.dir);
            $$ = {t, tBool};
        }
    }
  | E[e1] AND E[e2]
    {
        int tBool = driver.tablaTipos.getId("bool");
        if ($e1.tipo != tBool || $e2.tipo != tBool) {
            cerr << "Error semántico: '&&' requiere bool." << endl; $$ = $e1;
        } else {
            string t = driver.newTemp();
            driver.tac.emitBinop(t, $e1.dir, "&&", $e2.dir);
            $$ = {t, tBool};
        }
    }
  | E[e1] GT E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '>'." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     ">", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | E[e1] LT E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '<'." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     "<", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | E[e1] GE E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '>='." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     ">=", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | E[e1] LE E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '<='." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     "<=", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | E[e1] NE E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '!='." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     "!=", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | E[e1] EQ E[e2]
    {
        if (!driver.tablaTipos.compatibles($e1.tipo,$e2.tipo)) {
            cerr << "Error: tipos incompatibles en '=='." << endl; $$ = $e1;
        } else {
            int tBool = driver.tablaTipos.getId("bool");
            int tipo  = driver.tablaTipos.maxTipo($e1.tipo,$e2.tipo);
            string t  = driver.newTemp();
            driver.tac.emitBinop(t, driver.ampliar($e1.dir,$e1.tipo,tipo),
                                     "==", driver.ampliar($e2.dir,$e2.tipo,tipo));
            $$ = {t, tBool};
        }
    }
  | C[c]
    {
        string t = driver.newTemp();
        driver.tac.emitArrayRead(t, $c.dir, driver._lastArrayIndex);
        $$ = {t, $c.tipo};
    }
  | ID[id]
    {
        if (!driver.existeId($id)) {
            cerr << "Error semántico: '" << $id << "' no declarado." << endl;
            $$ = {$id, -1};
        } else {
            $$ = {$id, driver.getTipoId($id)};
        }
    }
  | ID[id] LPAREN N[args] RPAREN
    {
        if (!driver.bottomTS()->existe($id)) {
            cerr << "Error semántico: función '" << $id << "' no declarada." << endl;
            $$ = {"", -1};
        } else if (driver.bottomTS()->getCat($id) != "func") {
            cerr << "Error semántico: '" << $id << "' no es función." << endl;
            $$ = {"", -1};
        } else {
            vector<int> esp = driver.bottomTS()->getArgs($id);
            if (esp != $args)
                cerr << "Error semántico: parámetros incorrectos en '" << $id << "'." << endl;
            string t = driver.newTemp();
            int tr = driver.bottomTS()->getTipo($id);
            driver.tac.emitCall(t, $id, (int)$args.size());
            $$ = {t, tr};
        }
    }
  | Z[z]
    {
        string t = driver.newTemp();
        driver.tac.emitArrayRead(t, $z.dir, "0");
        $$ = {t, $z.tipo};
    }
  | INTLIT[n]  { $$ = {to_string($n), driver.tablaTipos.getId("int")};   }
  | FLOATLIT[f]{ $$ = {to_string($f), driver.tablaTipos.getId("float")}; }
  | TRUE       { $$ = {"1", driver.tablaTipos.getId("bool")};            }
  | FALSE      { $$ = {"0", driver.tablaTipos.getId("bool")};            }
  | CHARLIT[c] { $$ = {string(1,$c), driver.tablaTipos.getId("char")};   }
  ;

/* ═══════════════════════════════════════════════════
   N / M  — argumentos de llamada
   ═══════════════════════════════════════════════════ */
N : M[m] { $$ = $m; }
  | %empty { $$ = vector<int>(); }
  ;

M : M[m] COMMA E[e]
    {
        driver.tac.emitParam($e.dir);
        $$ = $m; $$.push_back($e.tipo);
    }
  | E[e]
    {
        driver.tac.emitParam($e.dir);
        $$ = {$e.tipo};
    }
  ;

/* ═══════════════════════════════════════════════════
   C  — acceso a arreglos multidimensionales
   ═══════════════════════════════════════════════════ */
C : ID[id] LBRACKET E[e] RBRACKET
    {
        if (!driver.existeId($id)) {
            cerr << "Error semántico: '" << $id << "' no declarado." << endl;
            $$ = {$id, -1};
        } else {
            int tipo = driver.getTipoId($id);
            if (driver.tablaTipos.getNombre(tipo) != "array") {
                cerr << "Error semántico: '" << $id << "' no es arreglo." << endl;
                $$ = {$id, -1};
            } else if ($e.tipo != driver.tablaTipos.getId("int")) {
                cerr << "Error semántico: índice debe ser entero." << endl;
                $$ = {$id, -1};
            } else {
                int tipoBase = driver.tablaTipos.getTipoBase(tipo);
                int tam      = driver.tablaTipos.getTam(tipoBase);
                string idx   = driver.newTemp();
                driver.tac.emitBinop(idx, $e.dir, "*", to_string(tam));
                driver._lastArrayBase  = $id;
                driver._lastArrayIndex = idx;
                $$ = {$id, tipoBase};
            }
        }
    }
  | C[c] LBRACKET E[e] RBRACKET
    {
        int tipo = $c.tipo;
        if (driver.tablaTipos.getNombre(tipo) != "array") {
            cerr << "Error semántico: demasiadas dimensiones." << endl; $$ = $c;
        } else if ($e.tipo != driver.tablaTipos.getId("int")) {
            cerr << "Error semántico: índice debe ser entero." << endl; $$ = $c;
        } else {
            int tipoBase = driver.tablaTipos.getTipoBase(tipo);
            int tam      = driver.tablaTipos.getTam(tipoBase);
            string t1    = driver.newTemp();
            string t2    = driver.newTemp();
            driver.tac.emitBinop(t1, $e.dir, "*", to_string(tam));
            driver.tac.emitBinop(t2, driver._lastArrayIndex, "+", t1);
            driver._lastArrayIndex = t2;
            $$ = {$c.dir, tipoBase};
        }
    }
  ;

/* ═══════════════════════════════════════════════════
   Z  — acceso a structs
   ═══════════════════════════════════════════════════ */
Z : ID[id] DOT ID[campo]
    {
        if (!driver.existeId($id)) {
            cerr << "Error semántico: '" << $id << "' no declarado." << endl;
            $$ = {$id, -1};
        } else {
            int tipo = driver.getTipoId($id);
            yy::SymbolTable* ts = driver.tablaTipos.getTS(tipo);
            if (ts == nullptr) {
                cerr << "Error semántico: '" << $id << "' no es struct." << endl;
                $$ = {$id, -1};
            } else if (!ts->existe($campo)) {
                cerr << "Error semántico: campo '" << $campo << "' no existe." << endl;
                $$ = {$id, -1};
            } else {
                int tipoCampo = ts->getTipo($campo);
                int dirCampo  = ts->getDir($campo);
                string t = driver.newTemp();
                driver.tac.emitBinop(t, $id, "+", to_string(dirCampo));
                $$ = {t, tipoCampo};
            }
        }
    }
  | Z[z] DOT ID[campo]
    {
        yy::SymbolTable* ts = driver.tablaTipos.getTS($z.tipo);
        if (ts == nullptr) {
            cerr << "Error semántico: no es struct." << endl; $$ = $z;
        } else if (!ts->existe($campo)) {
            cerr << "Error semántico: campo '" << $campo << "' no existe." << endl;
            $$ = $z;
        } else {
            int tipoCampo = ts->getTipo($campo);
            int dirCampo  = ts->getDir($campo);
            string t = driver.newTemp();
            driver.tac.emitBinop(t, $z.dir, "+", to_string(dirCampo));
            $$ = {t, tipoCampo};
        }
    }
  ;

%%

void yy::parser::error(const string& msg) {
    cerr << "Error sintáctico: " << msg << endl;
}
