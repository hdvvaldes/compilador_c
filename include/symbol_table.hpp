#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP
#include <vector>
using namespace std;
#include "Symbol.h"
#include "Type.h"

namespace yy
{
    class Table
    {
    private:
        vector<yy::Symbol> symTab;
        vector<yy::Type> typeTab;

    public:
        Table();
        ~Table();
        void addSymbol(string id);
        void addType(string name);
        bool isInSymbol(string id);
        void setDir(string id, int dir);
        void setType(string id, int type);
        void setTypeVar(string id, string type);
        void setArgs(string id, vector<int> args);

        int getDir(string id);
        int getType(string id);
        string getTypeVar(string id);
        vector<int> getArgs(string id);
        vector<yy::Symbol> getSymTab();
    };
}
#endif
