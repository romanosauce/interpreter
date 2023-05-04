#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

int line_count = 0;

enum  TypeOfLex {
    LEX_NULL,
    LEX_IDENT,
    LEX_NUM
};

class Lex {
    public:
        Lex(TypeOfLex t = LEX_NULL, int v = 0) : t_lex_(t), v_lex_(v) {}
        TypeOfLex get_type() const {
            return t_lex_;
        }
        int get_name() const {
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

vector<Ident> TID;

class Scanner {
    public:
        static map<string, TypeOfLex> reserved_words_;
        static const set<char> stop_chars_;
        static map<string, TypeOfLex> delimeters_;

        Scanner(string file);
        Lex GetLex();
        void NextSymbol() {
            ptr_++;
            cur_char_ = data_[ptr_];
        }
    private:
        string data_;
        char cur_char_;
        int ptr_;
        void SkipSpaces() {
            while (data_[ptr_] == ' ' || data_[ptr_] == '\n' ||
                   data_[ptr_] == '\r' ||data_[ptr_] == '\t') {
                line_count += data_[ptr_] == '\n';
                ptr_++;
            }
            cur_char_ = data_[ptr_];
        }
        void DeleteComment();
        string GetWord();
};

Scanner::Scanner(string file_name) {
    ifstream file(file_name, std::ios::binary);
    stringstream buf;
    buf << file.rdbuf();
    data_ = buf.str();
    ptr_ = 0;
}

void Scanner::DeleteComment() {
    string::size_type com_st = ptr_;
    string::size_type com_end = data_.find("*/", ptr_);
    if (com_end != data_.npos && com_st < com_end) {
        string comment = data_.substr(com_st, com_end);
        string::size_type st = com_st;
        while (comment.find("\n", st) != data_.npos) {
            line_count++;
            st = comment.find("\n", st);
        }
        ptr_ = com_end + 2;
        cur_char_ = data_[ptr_];
    } else {
        //error handler
    }
}

string Scanner::GetWord() {
    size_t st_pos = ptr_;
    while (stop_chars_.find(data_[ptr_]) != stop_chars_.end()) {
        ptr_++;
    }
    return data_.substr(st_pos, ptr_);
}

map<string, TypeOfLex> Scanner::reserved_words_ = {
    {"abc", LEX_NULL}
    //"program", "int", "real", "string", "boolean", "if", "else",
    //"do", "while", "read", "write", "for", "break", "continue", "true",
    //"false", "not", "and", "or", "goto"
};

const set<char> Scanner::stop_chars_ = {
    ' ', '\n', '\t', '*', '%', '+', '-', '<', '>', '=', '!', ',',
    ';', ':', '(', ')', '/', '@'
};

map<string, TypeOfLex> Scanner::delimeters_ = {};

Lex Scanner::GetLex() {
    SkipSpaces();
    if (isalpha(cur_char_)) {
        string buf = GetWord();
        if (reserved_words_.find(buf) != reserved_words_.end()) {
            return Lex((TypeOfLex) reserved_words_[buf],
                       (int) reserved_words_[buf]);
        } else {
            vector<Ident>::iterator it;
            if ((it = find(TID.begin(), TID.end(), Ident(buf))) != TID.end()) {
                return Lex(LEX_IDENT, it - TID.begin());
            } else {
                TID.emplace_back(Ident(buf));
                return Lex(LEX_IDENT, TID.size() - 1);
            }
        }
    }
    if (isdigit(cur_char_)) {
        int value = 0;
        value = cur_char_ - '0';
        while (isdigit(cur_char_)) {
            value = value * 10 + cur_char_ - '0';
            NextSymbol();
        }
        return Lex(LEX_NUM, value);
    }
    if (cur_char_ == '/' && data_[ptr_ + 1] == '*') {
        DeleteComment();
    }
}
