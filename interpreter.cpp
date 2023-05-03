#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>

using namespace std;

enum  TypeOfLex {
    LEX_NULL
};

class Lex {
    public:
        Lex(TypeOfLex t = LEX_NULL, int v = 0) : t_lex_(t), v_lex_(v) {}
        TypeOfLex get_type() const {
            return t_lex_;
        }
        int get_value() const {
            return v_lex_;
        }
        friend ostream &operator<<(ostream &s, Lex lexeme);
    private:
        TypeOfLex t_lex_;
        int v_lex_;
};

class Ident {
    public:
        Ident() : declare_(false), assign_(false) {}
        Ident(const string name) : name_(name), declare_(false),
                                   assign_(false) {}
        bool operator==(const string &s) const {
            return name_ == s;
        }
        string get_name() const {
            return name_;
        }
        bool get_declare() const {
            return declare_;
        }
        void set_declare() {
            declare_ = true;
        }
        TypeOfLex get_type() const {
            return type_;
        }
        void set_type(TypeOfLex t) {
            type_ = t;
        }
        bool get_assign() const {
            return assign_;
        }
        void set_assign() {
            assign_ = true;
        }
        int get_value() const {
            return value_;
        }
        void set_value(int v) {
            value_ = v;
        }
    private:
        string name_;
        bool declare_;
        bool assign_;
        TypeOfLex type_;
        int value_;
};

unordered_map<string, Ident> TID;

class Scanner {
    public:
        static const set<string> reserved_words_, delimeters_;
        Scanner(string file);
        Lex GetLex();
    private:
        string data_;
        char cur_char_;
};

Scanner::Scanner(string file_name) {
    ifstream file(file_name, std::ios::binary);
    stringstream buf;
    buf << file.rdbuf();
    data_ = buf.str();
}

const set<string> Scanner::reserved_words_ = {
    "program", "int", "real", "string", "boolean", "if", "else",
    "do", "while", "read", "write", "for", "break", "continue", "true",
    "false", "not", "and", "or", "goto"
};

const set<string> Scanner::delimeters_ = {
    " ", "\n", "\t", "*", "%", "+", "-", "<", ">", "=", "!", ",",
    ";", ":", "(", ")", "/", "@"
};

Lex Scanner::GetLex() {
    
