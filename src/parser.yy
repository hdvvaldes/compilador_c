%skeleton "lalr1.cc"
%require "3.0"

%defines
%define api.namespace {yy}
%define api.value.type variant
%define api.token.constructor

%code requires{
    #include <string>
    #include <vector>
    using namespace std;
}

%code{
    #include <iostream>
    #include <stdexcept>

    // Prototipo del lexer para Bison C++
    #define YY_DECL yy::parser::symbol_type yylex()
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

%start P

/* Precedencias lógicas y aritméticas (Resuelven los conflictos de expresiones) */
%right ASSIGN
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MULT DIV MOD

%%

/* --- ÁMBITO GLOBAL --- */

/* Raíz de la Gramática */
P : H {
    // dir <- 0, PilaTs.push(nuevaTs())
};

/* Un archivo de código fuente en C solo contiene declaraciones globales (variables, structs y funciones) */
H : D H
  | %empty
  ;

/* Declaraciones globales y definiciones */
D : T L SEMICOLON
  | STRUCT ID LKEY BLOQUE RKEY SEMICOLON
  | DEF T ID LPAREN F RPAREN LKEY BLOQUE RKEY
  ;


/* --- ÁMBITO LOCAL (Dentro de funciones / estructuras) --- */

/* Un bloque interno permite cualquier combinación de declaraciones y sentencias */
BLOQUE : D BLOQUE
       | S BLOQUE
       | %empty
       ;


/* --- REGLAS DE TIPOS Y ARREGLOS --- */
T : B A ;

B : INT | FLOAT | CHAR | BOOL | ID ;

A : LBRACKET INTLIT RBRACKET A
  | %empty
  ;

L : L COMMA ID
  | ID
  ;


/* --- PARÁMETROS DE FUNCIONES --- */
F : G
  | %empty
  ;

G : G COMMA T ID
  | T ID
  ;


/* --- SENTENCIAS (STATEMENTS) --- */
S : BREAK SEMICOLON
  | RETURN E SEMICOLON
  | ID ASSIGN E SEMICOLON
  | C ASSIGN E SEMICOLON
  | Z ASSIGN E SEMICOLON
  | IF LPAREN E RPAREN S
  | IF LPAREN E RPAREN S ELSE S
  | WHILE LPAREN E RPAREN S
  | FOR LPAREN S SEMICOLON E SEMICOLON S RPAREN S
  | LKEY BLOQUE RKEY
  ;


/* --- EXPRESIONES (EXPRESSIONS) --- */
E : E PLUS E
  | E MINUS E
  | E MULT E
  | E DIV E
  | E MOD E
  | E OR E
  | E AND E
  | E GT E
  | E LT E
  | E GE E
  | E LE E
  | E NE E
  | E EQ E
  | C
  | ID
  | ID LPAREN N RPAREN
  | Z
  | INTLIT
  | FLOATLIT
  | TRUE
  | FALSE
  | CHARLIT
  ;


/* --- ACCESOS A DATOS COMPLEJOS --- */

/* Acceso a Arreglos (Ej: vector[0][1]) */
C : ID LBRACKET E RBRACKET
  | C LBRACKET E RBRACKET
  ;

/* Acceso a Estructuras (Ej: persona.datos.edad)
   Fijamos el caso base como 'ID DOT ID' para evitar el conflicto con un ID simple. */
Z : ID DOT ID
  | Z DOT ID
  ;


/* --- ARGUMENTOS DE LLAMADA --- */
N : M
  | %empty
  ;

M : M COMMA E
  | E
  ;

%%

void yy::parser::error(const string& msg){
    cerr << "Error sintáctico: " << msg << endl;
}
