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

    LEX_AND, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE,
    LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE,
    LEX_WHILE, LEX_WRITE, LEX_STRING, LEX_FIN, LEX_CONTINUE,
    LEX_BREAK, LEX_GOTO,

    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN,
    LEX_LCURL_BRACKET, LEX_RCURL_BRACKET, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, 
    LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_NUM,
    LEX_PERCENT,

    LEX_ID,
};

class Lex {
    public:
        Lex(TypeOfLex t = LEX_NULL, int v = 0, string holder = "")
            : t_lex_(t), v_lex_(v), holder_(holder) {}
        TypeOfLex get_type() const {
            return t_lex_;
        }
        int get_value() const {
            return v_lex_;
        }
        string get_holder() const {
            return holder_;
        }
        friend ostream &operator<<(ostream &s, Lex lexeme);
    private:
        TypeOfLex t_lex_;
        int v_lex_;
        string holder_;
};

class Ident {
    public:
        Ident() : declare_(false), assign_(false) {}
        Ident(const string name) : name_(name), declare_(false),
                                   assign_(false) {}
        bool operator==(const string& s) const {
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
        static set<char> stop_chars_;
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
            if (ptr_ == data_.size()) {
                return;
            }
            while (data_[ptr_] == ' ' || data_[ptr_] == '\n' ||
                   data_[ptr_] == '\r' ||data_[ptr_] == '\t') {
                if (data_[ptr_] == '\n') {
                    line_count++;
                    cout << "Line " << line_count << '\n';
                }
                ptr_++;
            }
            cur_char_ = data_[ptr_];
        }
        void DeleteComment();
        string GetWord();
        string GetDelim();
};

Scanner::Scanner(string file_name) {
    ifstream file(file_name, std::ios::binary);
    stringstream buf;
    buf << file.rdbuf();
    data_ = buf.str();
    cout << data_ << '\n';
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
            cout << "Line " << line_count << '\n';
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
    while (stop_chars_.find(data_[ptr_]) == stop_chars_.end()) {
        ptr_++;
    }
    cout << "AAA " << st_pos << ' ' << ptr_ << '\n';
    return data_.substr(st_pos, ptr_ - st_pos);
}

string Scanner::GetDelim() {
    size_t st_pos = ptr_;
    while (stop_chars_.find(data_[ptr_]) != stop_chars_.end() &&
           data_[ptr_] != ' ' && data_[ptr_] != '\n' && data_[ptr_] != '\t') {
        ptr_++;
    }
    return data_.substr(st_pos, ptr_ - st_pos);
}

map<string, TypeOfLex> Scanner::reserved_words_ = {
    {"and", LEX_AND},
    {"boolean", LEX_BOOL},
    {"do", LEX_DO},
    {"else", LEX_ELSE},
    {"end", LEX_END},
    {"if", LEX_IF},
    {"false", LEX_FALSE},
    {"int", LEX_INT},
    {"not", LEX_NOT},
    {"or", LEX_OR},
    {"program", LEX_PROGRAM},
    {"read", LEX_READ},
    {"then", LEX_THEN},
    {"true", LEX_TRUE},
    {"while", LEX_WHILE},
    {"write", LEX_WRITE},
    {"string", LEX_STRING},
    {"continue", LEX_CONTINUE},
    {"break", LEX_BREAK},
    {"goto", LEX_GOTO}
    //"program", "int", "real", "string", "boolean", "if", "else",
    //"do", "while", "read", "write", "for", "break", "continue", "true",
    //"false", "not", "and", "or", "goto"
};

set<char> Scanner::stop_chars_ = {
    ' ', '\n', '\t', '*', '%', '+', '-', '<', '>', '=', '!', ',',
    ';', ':', '(', ')', '/'
};

map<string, TypeOfLex> Scanner::delimeters_ = {
    {";", LEX_SEMICOLON},
    {",", LEX_COMMA},
    {":", LEX_COLON},
    {"=", LEX_ASSIGN},
    {"(", LEX_LPAREN},
    {")", LEX_RPAREN},
    {"{", LEX_LCURL_BRACKET},
    {"}", LEX_RCURL_BRACKET},
    {"==", LEX_EQ},
    {"<", LEX_LSS},
    {">", LEX_GTR},
    {"+", LEX_PLUS}, 
    {"-", LEX_MINUS},
    {"*", LEX_TIMES},
    {"/", LEX_SLASH},
    {"<=", LEX_LEQ},
    {"!=", LEX_NEQ},
    {">=", LEX_GEQ},
    {"%", LEX_PERCENT}
};

Lex Scanner::GetLex() {
    SkipSpaces();
    if (isalpha(cur_char_)) {
        string buf = GetWord();
        if (reserved_words_.find(buf) != reserved_words_.end()) {
            return Lex((TypeOfLex) reserved_words_[buf],
                       (int) reserved_words_[buf], buf);
        } else {
            vector<Ident>::iterator it;
            if ((it = find(TID.begin(), TID.end(), buf)) != TID.end()) {
                return Lex(LEX_IDENT, it - TID.begin(), buf);
            } else {
                TID.emplace_back(Ident(buf));
                return Lex(LEX_IDENT, TID.size() - 1, buf);
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
        return Lex(LEX_NUM, value, "NUMBER");
    }
    if (cur_char_ == '/' && data_[ptr_ + 1] == '*') {
        DeleteComment();
        return GetLex();
    }
    if (ptr_ == data_.size()) {
        return Lex(LEX_FIN, 0, "FINAL");
    }
    if (stop_chars_.find(data_[ptr_]) != stop_chars_.end()) {
        string buf = GetDelim();
        cout << "GetDelim: " << '\'' << buf <<'\'' << '\n';
        cout << delimeters_[buf] << '\n';
        return Lex((TypeOfLex) delimeters_[buf], (int) delimeters_[buf], buf);
    }
    else {
        //error handling
        return Lex(LEX_NULL, 0, "NULL");
    }
}

ostream &operator<<(ostream &s, Lex lexeme) {
    s << lexeme.get_type() << ' ' <<
         lexeme.get_value() << ' ' <<
         lexeme.get_holder() << '\n';
    return s;
}


int main() {
    Scanner prog("tests/test");
    Lex cur_lex = prog.GetLex();
    while (cur_lex.get_type() != LEX_FIN) {
        cout << cur_lex;
        cur_lex = prog.GetLex();
    }
}
