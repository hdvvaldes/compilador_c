%option noyywrap nounput
%option prefix="yy"

%{
#include <string>
#include <iostream>
#include "Driver.hpp"
#include "parser.tab.hh"

#undef YY_DECL
#define YY_DECL yy::parser::symbol_type yylex(yy::Driver& driver)
%}

/* Expresiones Regulares */
LENG_ID      [a-zA-Z_][a-zA-Z0-9_]*
LENG_INT     [0-9]+
LENG_FLOAT   [0-9]+\.[0-9]+
LENG_CHAR    '([^'\\\n]|\\.)'

%%

[ \t\r]+                    { /* Ignorar espacios en blanco */ }
"\n"                        { /* Control de líneas si es necesario */ }

"int"                       { return yy::parser::make_INT(); }
"float"                     { return yy::parser::make_FLOAT(); }
"char"                      { return yy::parser::make_CHAR(); }
"bool"                      { return yy::parser::make_BOOL(); }
"struct"                    { return yy::parser::make_STRUCT(); }
"def"                       { return yy::parser::make_DEF(); }
"if"                        { return yy::parser::make_IF(); }
"else"                      { return yy::parser::make_ELSE(); }
"while"                     { return yy::parser::make_WHILE(); }
"for"                       { return yy::parser::make_FOR(); }
"break"                     { return yy::parser::make_BREAK(); }
"return"                    { return yy::parser::make_RETURN(); }

"{"                         { return yy::parser::make_LKEY(); }
"}"                         { return yy::parser::make_RKEY(); }
"["                         { return yy::parser::make_LBRACKET(); }
"]"                         { return yy::parser::make_RBRACKET(); }
"("                         { return yy::parser::make_LPAREN(); }
")"                         { return yy::parser::make_RPAREN(); }
";"                         { return yy::parser::make_SEMICOLON(); }
","                         { return yy::parser::make_COMMA(); }
"."                         { return yy::parser::make_DOT(); }

"="                         { return yy::parser::make_ASSIGN(); }
"+"                         { return yy::parser::make_PLUS(); }
"-"                         { return yy::parser::make_MINUS(); }
"*"                         { return yy::parser::make_MULT(); }
"/"                         { return yy::parser::make_DIV(); }
"%"                         { return yy::parser::make_MOD(); }

"&&"                        { return yy::parser::make_AND(); }
"||"                        { return yy::parser::make_OR(); }
"=="                        { return yy::parser::make_EQ(); }
"!="                        { return yy::parser::make_NE(); }
"<"                         { return yy::parser::make_LT(); }
">"                         { return yy::parser::make_GT(); }
"<="                        { return yy::parser::make_LE(); }
">="                        { return yy::parser::make_GE(); }

"true"                      { return yy::parser::make_TRUE(); }
"false"                     { return yy::parser::make_FALSE(); }

{LENG_ID}                   { return yy::parser::make_ID(yytext); }
{LENG_INT}                  { return yy::parser::make_INTLIT(std::stoi(yytext)); }
{LENG_FLOAT}                { return yy::parser::make_FLOATLIT(std::stof(yytext)); }
{LENG_CHAR}                 { return yy::parser::make_CHARLIT(yytext[1]); }

.                           { std::cerr << "Error léxico: " << yytext << std::endl; }

%%
