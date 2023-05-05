#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <variant>

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
    LEX_STR, LEX_PERCENT, LEX_QUOTE,

    LEX_IDENT,
};

class Lex {
    public:
        Lex(TypeOfLex t = LEX_NULL, int v = 0, 
            const string &holder = string(""))
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
        Ident(const string &name) : name_(name), declare_(false),
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
        variant<int, bool, string> get_value() const {
            return value_;
        }
        void set_value(const variant<int, bool, string> &v) {
            value_ = v;
        }
    private:
        string name_;
        bool declare_;
        bool assign_;
        TypeOfLex type_;
        variant<int, bool, string> value_;
};

vector<Ident> TID;

class Scanner {
    public:
        static map<string, TypeOfLex> reserved_words_;
        static set<char> stop_chars_;
        static map<string, TypeOfLex> delimeters_;

        Scanner(const string &file);
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
        string GetStr();
};

Scanner::Scanner(const string &file_name) {
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
    cout << com_end << '\n';
    if (com_end != data_.npos && com_st < com_end) {
        string comment = data_.substr(com_st, com_end - com_st);
        string::size_type st = 0;
        while (comment.find("\n", st) != data_.npos) {
            line_count++;
            cout << "Line " << line_count << '\n';
            st = comment.find("\n", st) + 1;
        }
        ptr_ = com_end + 2;
        cur_char_ = data_[ptr_];
    } else {
        //error handling
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
    if (((cur_char_ == '<' || cur_char_ == '>' || cur_char_ == '=') &&
        data_[ptr_+1] == '=') || cur_char_ == '!') {
        ptr_ += 2;
        cur_char_ = data_[ptr_];
        return data_.substr(st_pos, 2);
    }
    ptr_++;
    cur_char_ = data_[ptr_];
    return data_.substr(st_pos, 1);
}

string Scanner::GetStr() {
    string buf;
    while (data_[++ptr_] != '"') {
        buf.push_back(data_[ptr_]);
        if (ptr_ == '\n' || ptr_ == data_.size()) {
            //error handling
        }
    }
    ptr_++;
    cur_char_ = data_[ptr_];
    return buf;
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
    ';', ':', '(', ')', '/', '"'
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
    {"%", LEX_PERCENT},
    {"\"", LEX_QUOTE}
};

Lex Scanner::GetLex() {
    SkipSpaces();
    //cout << "CHAR: " << cur_char_ << '\n';
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
        cout << "DeleteComment" << '\n';
        DeleteComment();
        return GetLex();
    }
    if (cur_char_ == '"') {
        return Lex(LEX_STR, (int) LEX_STR, GetStr());
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

class Parser {
    public:
        vector<Lex> poliz;
        Parser(const string &file) : scan_(file) {}
        void StartAnalysis();
    private:
        Lex cur_lex_;
        TypeOfLex c_type_;
        int c_val_;
        Scanner scan_;
        stack<int> st_int_;
        stack<TypeOfLex> st_lex_;
        void ReadProgram();
        void ReadDeclarations();
        void ReadOperators();
        void Declaration();
        void Var();
        void Operator();
        void Expression();

        void GetNextLex() {
            cur_lex_ = scan_.GetLex();
            c_type_ = cur_lex_.get_type();
            c_val_ = cur_lex_.get_value();
        }
};

void Parser::StartAnalysis() {
    GetNextLex();
    cout << "1\n";
    ReadProgram();
    for (const Ident &it : TID) {
        cout << it.get_name() << ' '
             << it.get_declare() << ' '
             << it.get_assign() << ' ';
        visit([](auto&& arg){ std::cout << arg; }, it.get_value());
    }
    if (c_type_ != LEX_FIN) {}
}

void Parser::ReadProgram() {
    if (c_type_ == LEX_PROGRAM) {
        GetNextLex();
    } else {
        //error handling
    }
    if (c_type_ == LEX_LCURL_BRACKET) {
        GetNextLex();
    } else {
        //error handling
    }
    ReadDeclarations();
    ReadOperators();
    if (c_type_ != LEX_RCURL_BRACKET) {
        //error handling
    }
}

void Parser::ReadDeclarations() {
    while (c_type_ == LEX_INT || c_type_ == LEX_STRING ||
           c_type_ == LEX_BOOL) {
        Declaration();
        if (c_type_ != LEX_SEMICOLON) {
            //error handling
        }
        GetNextLex();
    }
}

void Parser::Declaration() {
    TypeOfLex decl_type = c_type_;
    GetNextLex();
    do {
        if (c_type_ != LEX_IDENT) {
            //error handling
        } else {
            int index = cur_lex_.get_value();
            if (TID[index].get_declare()) {
                //error handling
            } else {
                TID[index].set_declare();
                TID[index].set_type(decl_type);
            }
            GetNextLex();
            if (c_type_ == LEX_EQ) {
                GetNextLex();
                if (c_type_ == LEX_MINUS || c_type_ == LEX_PLUS) {
                    if (decl_type == LEX_INT) {
                        GetNextLex();
                        if (c_type_ != LEX_NUM) {
                            //error handling
                        }
                        TID[index].set_value((c_type_ == LEX_MINUS ? -1 : 1) *
                                              cur_lex_.get_value());
                        TID[index].set_assign();
                    } else {
                        //error handling
                    }
                } else if (c_type_ == LEX_STR) {
                    if (decl_type == LEX_STRING) {
                        TID[index].set_value(cur_lex_.get_value());
                        TID[index].set_assign();
                    } else {
                        //error handling
                    }
                } else if (c_type_ == LEX_NUM) {
                    if (c_type_ != LEX_NUM) {
                        //error handling
                    }
                    TID[index].set_value(cur_lex_.get_value());
                    TID[index].set_assign();
                } else if (c_type_ == LEX_TRUE || c_type_ == LEX_FALSE) {
                    TID[index].set_value((c_type_ == LEX_TRUE ? true : false));
                    TID[index].set_assign();
                } else {
                    //error handling
                }
                GetNextLex();
            }
        }
    } while (c_type_ == LEX_COMMA);
}

void Parser::ReadOperators() {
}

int main() {
    Scanner prog("tests/test1");
    Lex cur_lex = prog.GetLex();
    while (cur_lex.get_type() != LEX_FIN) {
        cout << cur_lex;
        cur_lex = prog.GetLex();
    }
    Parser test_prog("tests/test2");
    test_prog.StartAnalysis();
    cout << "end\n";
}
