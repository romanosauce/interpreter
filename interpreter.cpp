#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <variant>
#include <set>
#include <unordered_set>

using namespace std;
enum TypeOfLex {
    LEX_NULL, // 0
    LEX_AND, LEX_BEGIN, LEX_WRITE, // 18
    LEX_FIN, // 19
    LEX_SEMICOLON, LEX_COMMA, LEX_GEQ, // 35
    LEX_NUM, // 36
    LEX_ID, // 37
    POLIZ_LABEL, // 38
    POLIZ_ADDRESS, // 39
    POLIZ_GO, // 40
    POLIZ_FGO // 41
};

vector<string> lex_names {
};

string get_lex_name(TypeOfLex lex) {
    return lex_names[(int)lex];
}

class Lex {
    public:
        Lex(TypeOfLex t = LEX_NULL, int v = 0) {
            t_lex = t;
            v_lex = v;
        }
        TypeOfLex get_type() {
            return t_lex;
        }
        int get_value() {
            return v_lex;
        }
        friend ostream &operator<<(ostream &s, Lex l) {
            s << '(' << get_lex_name(l.t_lex) << ',' << l.v_lex << ");";
            return s;
        }
    private:
        TypeOfLex t_lex;
        int v_lex;
};

class Ident {
    public:
        Ident() {
            declare = false;
            assign = false;
        }
        string get_name() {
            return name;
        }
        void put_name(string n) {
            name = n;
        }
        bool get_declare() {
            return declare;
        }
        void put_declare() {
            declare = true;
        }
        TypeOfLex get_type() {
            return type;
        }
        void put_type(TypeOfLex t) {
            type = t;
        }
        bool get_assign() {
            return assign;
        }
        void put_assign() {
            assign = true;
        }
        int get_value() {
            return value;
        }
        void put_value(int v) {
            value = v;
        }
    private:
        string name;
        bool declare;
        TypeOfLex type;
        bool assign;
        int value;
};

set<Ident> table_ident;

class Scanner {
    public:
        Lex GetLex();
        Scanner(string &file_name);
    private:
        enum State {
            H, IDENT, NUMBER, COMMENT, ALE, DELIMETER, NEQ
        };
        static unordered_set<string> reserved_words;
        static unordered_set<TypeOfLex> lex_reserved_words;
        static unordered_set<string> delimeters;
        static unordered_set<TypeOfLex> lex_delimeters;
        State cur_state;
        char c;
        string data;
        string buf;
        int cur_pos;
        void clear() {
            buf.clear();
        }
        void add() {
            buf.push_back(c);
        }
        unordered_set<string>::iterator 
        look(string &buf, unordered_set<string> &list) {
            return list.find(buf);
        }
        void gc() {
            c = data[cur_pos++];
        }
};

Scanner::Scanner(string &file_name) {
    ifstream file(file_name, ios::binary);
    stringstream buf;
    buf << file.rdbuf();
    data = buf.str();
    cur_state = H;
    cur_pos = 0;
    gc();
}

unordered_set<string> Scanner::reserved_words {
    "program", "int", "real", "string", "boolean", "if", "else",
    "do", "while", "read", "write", "for", "break", "continue", "true",
    "false", "not", "and", "or", "goto"
};

unordered_set<string> Scanner::delimeters {
    " ", "\n", "\t", "*", "%", "+", "-", "<", ">", "=", "!", ",",
    ";", ":", "(", ")", "/"
};


class 

