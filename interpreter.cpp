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

size_t line_count = 0;
enum ErrorType {
    FILE_BAD = -1,
    COMM_NO_CLOSE,
    WRONG_STR_CONST,
    UNKNOWN_SYM,
    SYNT_LEX_AFTER_END,
    SYNT_NOPROG,
    SYNT_NO_OPCURL_BRAC,
    SYNT_NO_CLCURL_BRAC,
    SYNT_NO_SEMICOLON,
    SYNT_NO_OPPAREN,
    SYNT_NO_CLPAREN,
    SYNT_IF_ERR,
    SYNT_FOR_ERR,
    WRONG_IDENT_NAME,
    WRONG_EXPR,
    WRONG_OP,
    SEM_PREV_DECL,
    SEM_WRONG_TYPE,
    SEM_NOT_DECLARE,
    SEM_WRONG_BREAK,
    SEM_ASSIGN_TO_UNMUT,
    SEM_WRONG_LABLE,
    SEM_UNKNW_LABLE
};

vector<pair<ErrorType, size_t>> err_stk;

int ErrorHandler() {
    for (auto it : err_stk) {
        ErrorType er = it.first;
        switch(er) {
            case FILE_BAD:
                cout << "Can't open the file\n";
                break;
            case COMM_NO_CLOSE:
                cout << "Syntax error: no close symbols for comment\n";
                break;
            case WRONG_STR_CONST:
                cout << "Missing \" on line " << it.second << '\n';
                break;
            case UNKNOWN_SYM:
                cout << "Unknown symbol on line " << it.second << '\n';
                break;
            case SYNT_LEX_AFTER_END:
                cout << "Syntax error: find lexemes after end" << '\n';
                break;
            case SYNT_NOPROG:
                cout << "Syntax error: no word \"program\" at the beginning.\n";
                break;
            case SYNT_NO_OPCURL_BRAC:
                cout << "Syntax error: can't find '{' bracket on line " <<
                    it.second << '\n';
                break;
            case SYNT_NO_CLCURL_BRAC:
                cout << "Syntax error: can't find corresponding '}' on line " <<
                    it.second << '\n';
                break;
            case SYNT_NO_SEMICOLON:
                cout << "Syntax error: can't find ';' on line " <<
                    it.second << '\n';
                break;
            case SYNT_NO_OPPAREN:
                cout << "Syntax error: can't find '(' bracket on line " <<
                    it.second << '\n';
                break;
            case SYNT_NO_CLPAREN:
                cout << "Syntax error: can't find corresponding ')' on line " <<
                    it.second << '\n';
                break;
            case SYNT_IF_ERR:
                cout << "Syntax error: something wrong with IF on line " <<
                    it.second << '\n';
                break;
            case SYNT_FOR_ERR:
                cout << "Syntax error: something wrong with FOR on line " <<
                     it.second << '\n';
                break;
            case WRONG_IDENT_NAME:
                cout << "Wrong identifier found on line " << it.second << '\n';
                break;
            case WRONG_EXPR:
                cout << "Wrong expression found on line " << it.second << '\n';
                break;
            case WRONG_OP:
                cout << "Wrong operator on line " << it.second << '\n';
                break;
            case SEM_PREV_DECL:
                cout << "Semantic error: double declaration on line " <<
                    it.second << '\n';
                break;
            case SEM_WRONG_TYPE:
                cout <<
                    "Semantic error: Wrong type used in expression on line " <<
                    it.second << '\n';
                break;
            case SEM_NOT_DECLARE:
                cout <<
                    "Semantic error: use of undeclared identifier on line " <<
                    it.second << '\n';
                break;
            case SEM_WRONG_BREAK:
                cout <<
                    "Semantic error: use of break outside cycle on line " <<
                    it.second << '\n';
                break;
            case SEM_ASSIGN_TO_UNMUT:
                cout <<
                    "Semantic error: assigning to unmutable object on line " <<
                    it.second << '\n';
                break;
            case SEM_WRONG_LABLE:
                cout <<
                    "Semantic error: wrong lable on line " <<
                    it.second << '\n';
                break;
            case SEM_UNKNW_LABLE:
                cout <<
                    "I've done my best to find lable for some goto" <<
                    ", but I didn't manage to. I can't even tell you " <<
                    "the line with problem. Sorry user :(\n";
            default:
                break;
        }
    }
    exit(0);
}

enum TypeOfLex {
    LEX_NULL,

    LEX_AND, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE,
    LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE,
    LEX_WHILE, LEX_WRITE, LEX_STRING, LEX_FIN, LEX_CONTINUE,
    LEX_BREAK, LEX_GOTO, LEX_FOR, LEX_LABLE,

    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN,
    LEX_LCURL_BRACKET, LEX_RCURL_BRACKET, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, 
    LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_NUM,
    LEX_STR, LEX_PERCENT, LEX_QUOTE,

    LEX_IDENT,

    POLIZ_GO, POLIZ_FGO, POLIZ_LABEL, POLIZ_ADDRESS,
    LEX_UN_MINUS, LEX_UN_PLUS
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
        size_t ptr_;
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
    ifstream file(file_name, ios::binary);
    if (file.fail()) {
        err_stk.push_back({FILE_BAD, 0});
        ErrorHandler();
    }
    stringstream buf;
    buf << file.rdbuf();
    data_ = buf.str();
    cout << data_ << '\n';
    ptr_ = 0;
    file.close();
}

void Scanner::DeleteComment() {
    string::size_type com_st = ptr_;
    string::size_type com_end = data_.find("*/", ptr_);
    cout << com_end << '\n';
    if (com_end != data_.npos) {
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
        err_stk.push_back({COMM_NO_CLOSE, line_count});
        ErrorHandler();
    }
}

string Scanner::GetWord() {
    size_t st_pos = ptr_;
    while (stop_chars_.find(data_[ptr_]) == stop_chars_.end()) {
        ptr_++;
    }
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
            err_stk.push_back({WRONG_STR_CONST, line_count});
            ErrorHandler();
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
    {"goto", LEX_GOTO},
    {"for", LEX_FOR}
    //"program", "int", "real", "string", "boolean", "if", "else",
    //"do", "while", "read", "write", "for", "break", "continue", "true",
    //"false", "not", "and", "or", "goto"
};

set<char> Scanner::stop_chars_ = {
    ' ', '\n', '\t', '*', '%', '+', '-', '<', '>', '=', '!', ',',
    ';', ':', '(', ')', '/', '"', '{', '}'
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
    if (isalpha(cur_char_)) {
        string buf = GetWord();
        if (reserved_words_.find(buf) != reserved_words_.end()) {
            if (reserved_words_[buf] == LEX_TRUE || 
                reserved_words_[buf] == LEX_FALSE) {
                return Lex(reserved_words_[buf], 
                           reserved_words_[buf] == LEX_TRUE ? 1 : 0);
            } else {
                return Lex((TypeOfLex) reserved_words_[buf],
                           (int) reserved_words_[buf], buf);
            }
        } else {
            vector<Ident>::iterator it;
            if ((it = find(TID.begin(), TID.end(), buf)) != TID.end()) {
                return Lex(LEX_IDENT, it - TID.begin(), buf);
            } else {
                TID.emplace_back(Ident(buf));
                return Lex(LEX_IDENT, TID.size() - 1, buf);
            }
        }
    } else if (isdigit(cur_char_)) {
        int value = 0;
        value = cur_char_ - '0';
        NextSymbol();
        while (isdigit(cur_char_)) {
            value = value * 10 + cur_char_ - '0';
            NextSymbol();
        }
        return Lex(LEX_NUM, value, "NUMBER");
    } else if (cur_char_ == '/' && data_[ptr_ + 1] == '*') {
        cout << "DeleteComment" << '\n';
        DeleteComment();
        return GetLex();
    } else if (cur_char_ == '"') {
        return Lex(LEX_STR, (int) LEX_STR, GetStr());
    } else if (ptr_ == data_.size()) {
        return Lex(LEX_FIN, 0, "FINAL");
    } else if (stop_chars_.find(data_[ptr_]) != stop_chars_.end()) {
        string buf = GetDelim();
        return Lex((TypeOfLex) delimeters_[buf], (int) delimeters_[buf], buf);
    } else {
        err_stk.push_back({UNKNOWN_SYM, line_count});
        ErrorHandler();
        return Lex(LEX_NULL);
    }
}

ostream &operator<<(ostream &s, Lex lexeme) {
    s << lexeme.get_type() << ' ' <<
         lexeme.get_value() << ' ' <<
         lexeme.get_holder() << '\n';
    return s;
}

template <class T, class T_EL>
void GetFromSt(T &st, T_EL &i) {
    i = st.top();
    st.pop();
}

map<int, vector<int>> unknw_lables;

class Parser {
    public:
        vector<Lex> poliz;
        Parser(const string &file) : scan_(file), cycle_count(0) {}
        void StartAnalysis();
    private:
        Lex cur_lex_;
        Lex prev_lex_;
        bool have_next_lex_;

        TypeOfLex c_type_;
        int c_val_;
        Scanner scan_;
        stack<TypeOfLex> type_stk_;
        int cycle_count;
        void CheckIdent();
        void CheckOp();
        void CheckUnary();
        void EqBool();

        void ReadProgram();
        void ReadDeclarations();
        void ReadOperators();
        void Declaration();
        void Operator();
        void ReadIf();
        void ReadFor();
        void ReadWhile();
        void Read();
        void Write();
        void ReadComplexOp();
        void Break();
        void Goto();

        bool mut_;
        void Expression();
        void E1();
        void E2();
        void E3();
        void E4();
        void E5();
        void T();
        void F();

        void GetNextLex() {
            if (!have_next_lex_) {
                prev_lex_ = cur_lex_;
                cur_lex_ = scan_.GetLex();
            } else {
                cur_lex_ = prev_lex_;
                have_next_lex_ = false;
            }
            c_type_ = cur_lex_.get_type();
            c_val_ = cur_lex_.get_value();
            cout << cur_lex_;
        }
        void GetPrevLex() {                                                     //this function only need for marked op handling
            Lex buf = cur_lex_;
            cur_lex_ = prev_lex_;
            prev_lex_ = buf;
            have_next_lex_ = true;
            c_type_ = cur_lex_.get_type();
            c_val_ = cur_lex_.get_value();
        }
};

void Parser::StartAnalysis() {
    GetNextLex();
    ReadProgram();
    for (const Ident &it : TID) {
        cout << "-----------\n";
        cout << it.get_name() << ' '
             << it.get_declare() << ' '
             << it.get_assign() << ' ';
        visit([](auto&& arg){ std::cout << arg; }, it.get_value());
        cout << '\n';
    }
    GetNextLex();
    if (c_type_ != LEX_FIN) {
        err_stk.push_back({SYNT_LEX_AFTER_END, line_count});
        ErrorHandler();
    }
    if (!unknw_lables.empty()) {
        err_stk.push_back({SEM_UNKNW_LABLE, line_count});
        ErrorHandler();
    }
}

void Parser::ReadProgram() {
    cout << "ReadProg\n";
    if (c_type_ == LEX_PROGRAM) {
        GetNextLex();
    } else {
        err_stk.push_back({SYNT_NOPROG, line_count});
        ErrorHandler();
    }
    if (c_type_ == LEX_LCURL_BRACKET) {
        GetNextLex();
    } else {
        err_stk.push_back({SYNT_NO_OPCURL_BRAC, line_count});
        ErrorHandler();
    }
    ReadDeclarations();
    ReadOperators();
    if (c_type_ != LEX_RCURL_BRACKET) {
        err_stk.push_back({SYNT_NO_CLCURL_BRAC, line_count});
        ErrorHandler();
    }
    cout << "-----------------------------\n";
    for (Lex l : poliz) {
        cout << l;
    }
}

void Parser::ReadDeclarations() {
    cout << "ReadDecl\n";
    while (c_type_ == LEX_INT || c_type_ == LEX_STRING ||
           c_type_ == LEX_BOOL) {
        Declaration();
        if (c_type_ != LEX_SEMICOLON) {
            err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
            return;
        }
        GetNextLex();
    }
}

void Parser::Declaration() {
    cout << "Decl\n";
    TypeOfLex decl_type = c_type_;
    do {
        GetNextLex();
        if (c_type_ != LEX_IDENT) {
            err_stk.push_back({WRONG_IDENT_NAME, line_count});
            ErrorHandler();
        } else {
            int index = cur_lex_.get_value();
            if (TID[index].get_declare()) {
                err_stk.push_back({SEM_PREV_DECL, line_count});
                ErrorHandler();
            } else {
                TID[index].set_declare();
                TID[index].set_type(decl_type);
            }
            GetNextLex();
            if (c_type_ == LEX_ASSIGN) {
                GetNextLex();
                if (c_type_ == LEX_MINUS || c_type_ == LEX_PLUS) {
                    if (decl_type == LEX_INT) {
                        GetNextLex();
                        if (c_type_ != LEX_NUM) {
                            err_stk.push_back({SEM_WRONG_TYPE, line_count});
                            ErrorHandler();
                        }
                        TID[index].set_value((c_type_ == LEX_MINUS ? -1 : 1) *
                                              cur_lex_.get_value());
                        TID[index].set_assign();
                    } else {
                        err_stk.push_back({SEM_WRONG_TYPE, line_count});
                        ErrorHandler();
                    }
                } else if (c_type_ == LEX_STR) {
                    if (decl_type == LEX_STRING) {
                        TID[index].set_value(cur_lex_.get_holder());
                        TID[index].set_assign();
                    } else {
                        err_stk.push_back({SEM_WRONG_TYPE, line_count});
                        ErrorHandler();
                    }
                } else if (c_type_ == LEX_NUM) {
                    if (decl_type != LEX_INT) {
                        err_stk.push_back({SEM_WRONG_TYPE, line_count});
                        ErrorHandler();
                    }
                    TID[index].set_value(cur_lex_.get_value());
                    TID[index].set_assign();
                } else if (c_type_ == LEX_TRUE || c_type_ == LEX_FALSE) {
                    TID[index].set_value((c_type_ == LEX_TRUE ? true : false));
                    TID[index].set_assign();
                } else {
                    err_stk.push_back({SEM_WRONG_TYPE, line_count});
                    ErrorHandler();
                }
                GetNextLex();
            }
        }
    } while (c_type_ == LEX_COMMA);
}

void Parser::ReadOperators() {
    cout << "ReadOperators\n";
    while (c_type_ == LEX_IF || c_type_ == LEX_WHILE ||
           c_type_ == LEX_READ || c_type_ == LEX_WRITE ||
           c_type_ == LEX_LCURL_BRACKET || c_type_ == LEX_FOR ||
           c_type_ == LEX_BREAK || c_type_ == LEX_GOTO ||
           c_type_ == LEX_IDENT || c_type_ == LEX_NUM ||
           c_type_ == LEX_TRUE || c_type_ == LEX_FALSE ||
           c_type_ == LEX_STR || c_type_ == LEX_NOT ||
           c_type_ == LEX_MINUS || c_type_ == LEX_PLUS ||
           c_type_ == LEX_LPAREN) {
        Operator();
    }
}

void Parser::Operator() {
    if (c_type_ == LEX_IF) {
        GetNextLex();
        ReadIf();
    } else if (c_type_ == LEX_FOR) {
        GetNextLex();
        ReadFor();
    } else if (c_type_ == LEX_WHILE) {
        GetNextLex();
        ReadWhile();
    } else if (c_type_ == LEX_READ) {
        GetNextLex();
        Read();
    } else if (c_type_ == LEX_WRITE) {
        GetNextLex();
        Write();
    } else if (c_type_ == LEX_LCURL_BRACKET) {
        GetNextLex();
        ReadComplexOp();
    } else if (c_type_ == LEX_BREAK) {
        Break();
    } else if (c_type_ == LEX_GOTO) {
        GetNextLex();
        Goto();
    } else if (c_type_ == LEX_IDENT) {
        Lex lex_copy = cur_lex_;
        GetNextLex();
        if (c_type_ == LEX_COLON) {
            size_t i = lex_copy.get_value();
            if (TID[i].get_declare()) {
                err_stk.push_back({SEM_WRONG_LABLE, line_count});
                ErrorHandler();
            } else {
                TID[i].set_type(LEX_LABLE);
                TID[i].set_value((int) poliz.size());
                TID[i].set_declare();
                TID[i].set_assign();
                for (auto it : unknw_lables[i]) {
                    poliz[it] = Lex(POLIZ_LABEL, poliz.size());
                }
                unknw_lables.erase(i);
            }
            GetNextLex();
            Operator();
        } else {
            GetPrevLex();
            Expression();
            if (c_type_ != LEX_SEMICOLON) {
                err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
                ErrorHandler();
            } else {
                GetNextLex();
            }
        }
    } else if (c_type_ == LEX_NUM || c_type_ == LEX_RPAREN ||
               c_type_ == LEX_TRUE || c_type_ == LEX_FALSE ||
               c_type_ == LEX_STR || c_type_ == LEX_NOT ||
               c_type_ == LEX_MINUS || c_type_ == LEX_PLUS) {
        Expression();
        if (c_type_ != LEX_SEMICOLON) {
            err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
            ErrorHandler();
        } else {
            GetNextLex();
        }
    } else {
        err_stk.push_back({WRONG_OP, line_count});
        ErrorHandler();
    }
}

void Parser::ReadIf() {
    int pl0, pl1;
    if (c_type_ != LEX_LPAREN) {
        err_stk.push_back({SYNT_NO_OPPAREN, line_count});
        ErrorHandler();
    } else {
        GetNextLex();
        Expression();
        EqBool();
        pl0 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type_ != LEX_RPAREN) {
            err_stk.push_back({SYNT_NO_CLPAREN, line_count});
            ErrorHandler();
        } else {
            GetNextLex();
            Operator();
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl0] = Lex(POLIZ_LABEL, poliz.size());
            if (c_type_ != LEX_ELSE) {
                err_stk.push_back({SYNT_IF_ERR, line_count});
                ErrorHandler();
            }
            GetNextLex();
            Operator();
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        }
    }
}

vector<vector<int>> break_stack;
int cycle_depth = 0;

void Parser::ReadFor() {
    int sec_exp_st;
    int lable_to_jmp_after_sec_exp;
    int lable_to_skip_thrd_exp;
    int thrd_exp_st;
    if (c_type_ != LEX_LPAREN) {
        err_stk.push_back({SYNT_NO_OPPAREN, line_count});
        ErrorHandler();
    } else {
        cycle_count++;
        if (cycle_depth < cycle_count) {
            break_stack.push_back({});
            cycle_depth++;
        }
        GetNextLex();
        if (c_type_ != LEX_SEMICOLON) {
            Expression();
            if (c_type_ != LEX_SEMICOLON) {
                err_stk.push_back({SYNT_FOR_ERR, line_count});
                ErrorHandler();
            }
        }
        sec_exp_st = poliz.size();
        GetNextLex();
        if (c_type_ != LEX_SEMICOLON) {
            Expression();
            EqBool();
            if (c_type_ != LEX_SEMICOLON) {
                err_stk.push_back({SYNT_FOR_ERR, line_count});
                ErrorHandler();
            }
        } else {
            poliz.push_back(Lex(LEX_TRUE));
        }
        lable_to_jmp_after_sec_exp = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        lable_to_skip_thrd_exp = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_GO));
        thrd_exp_st = poliz.size();
        GetNextLex();
        if (c_type_ != LEX_RPAREN) {
            Expression();
            if (c_type_ != LEX_RPAREN) {
                err_stk.push_back({SYNT_FOR_ERR, line_count});
                ErrorHandler();
            }
        }
        poliz.push_back(Lex(POLIZ_LABEL, sec_exp_st));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[lable_to_skip_thrd_exp] = Lex(POLIZ_LABEL, poliz.size());
        GetNextLex();
        Operator();
        poliz.push_back(Lex(POLIZ_LABEL, thrd_exp_st));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[lable_to_jmp_after_sec_exp] = Lex(POLIZ_LABEL, poliz.size());
        for (int i : break_stack[cycle_count-1]) {
            poliz[i] = Lex(POLIZ_LABEL, poliz.size());
        }
        break_stack[cycle_count-1] = {};
        cycle_count--;
    }
}

void Parser::ReadWhile() {
    int pl0, pl1;
    if (c_type_ != LEX_LPAREN) {
        err_stk.push_back({SYNT_NO_OPPAREN, line_count});
        ErrorHandler();
    } else {
        cycle_count++;
        pl0 = poliz.size();
        GetNextLex();
        Expression();
        EqBool();
        pl1 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type_ != LEX_RPAREN) {
            err_stk.push_back({SYNT_NO_CLPAREN, line_count});
            ErrorHandler();
        }
        GetNextLex();
        Operator();
        poliz.push_back(Lex(POLIZ_LABEL, pl0));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        cycle_count--;
    }
}

void Parser::Goto() {
    if (c_type_ != LEX_IDENT) {
        err_stk.push_back({WRONG_IDENT_NAME, line_count});
        ErrorHandler();
    } else {
        if (TID[c_val_].get_declare()) {
            if (TID[c_val_].get_type() != LEX_LABLE) {
                err_stk.push_back({SEM_WRONG_LABLE, line_count});
                ErrorHandler();
            } else {
                poliz.push_back(Lex(POLIZ_LABEL,
                                get<int>(TID[c_val_].get_value())));
            }
        } else {
            unknw_lables[c_val_].push_back(poliz.size());
            poliz.push_back(Lex());
        }
        poliz.push_back(Lex(POLIZ_GO));
        GetNextLex();
        if (c_type_ != LEX_SEMICOLON) {
            err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
            ErrorHandler();
        }
        GetNextLex();
    }
}

void Parser::Break() {
    if (!cycle_count) {
        err_stk.push_back({SEM_WRONG_BREAK, line_count});
        ErrorHandler();
    }
    break_stack[cycle_count-1].push_back(poliz.size());
    poliz.push_back(Lex());
    poliz.push_back(Lex(POLIZ_GO));
    GetNextLex();
    if (c_type_ != LEX_SEMICOLON) {
        err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
        ErrorHandler();
    }
    GetNextLex();
}

void Parser::Read() {
    if (c_type_ != LEX_LPAREN) {
        err_stk.push_back({SYNT_NO_OPPAREN, line_count});
        ErrorHandler();
    } else {
        GetNextLex();
        if (c_type_ != LEX_IDENT) {
            err_stk.push_back({WRONG_IDENT_NAME, line_count});
            ErrorHandler();
        }
        if (!TID[c_val_].get_declare()) {
            err_stk.push_back({SEM_NOT_DECLARE, line_count});
            ErrorHandler();
        }
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val_));
        GetNextLex();
        if (c_type_ != LEX_RPAREN) {
            err_stk.push_back({SYNT_NO_CLPAREN, line_count});
            ErrorHandler();
        }
        poliz.push_back(Lex(LEX_READ));
        GetNextLex();
        if (c_type_ != LEX_SEMICOLON) {
            err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
            ErrorHandler();
        }
        GetNextLex();
    }
}

void Parser::Write() {
    if (c_type_ != LEX_LPAREN) {
        err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
        ErrorHandler();
    } else {
        int exp_num = 0;
        do {
            exp_num++;
            GetNextLex();
            Expression();
        } while (c_type_ == LEX_COMMA);
        if (c_type_ != LEX_RPAREN) {
            err_stk.push_back({SYNT_NO_CLPAREN, line_count});
            ErrorHandler();
        }
        poliz.push_back(Lex(LEX_WRITE, exp_num));
        GetNextLex();
        if (c_type_ != LEX_SEMICOLON) {
            err_stk.push_back({SYNT_NO_SEMICOLON, line_count});
            ErrorHandler();
        }
        GetNextLex();
    }
}

void Parser::ReadComplexOp() {
    cout << "ReadComplexOp\n";
    ReadOperators();
    if (c_type_ != LEX_RCURL_BRACKET) {
        err_stk.push_back({SYNT_NO_CLCURL_BRAC, line_count});
    }
    GetNextLex();
}

void Parser::Expression() {
    mut_ = false;
    E1();
    while (c_type_ == LEX_ASSIGN) {
        if (!mut_) {
            err_stk.push_back({SEM_ASSIGN_TO_UNMUT, line_count});
            ErrorHandler();
        }
        Lex ident_lex = poliz.back();
        poliz.pop_back();                                                       //used to remove LEX_IDENT from poliz stack and replace it with address
        poliz.push_back(Lex(POLIZ_ADDRESS, ident_lex.get_value()));
        type_stk_.push(c_type_);
        mut_ = false;
        GetNextLex();
        E1();
        CheckOp();
    }
}

void Parser::E1() {
    E2();
    while (c_type_ == LEX_OR) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        E2();
        CheckOp();
    }
}

void Parser::E2() {
    E3();
    while (c_type_ == LEX_AND) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        E3();
        CheckOp();
    }
}

void Parser::E3() {
    E4();
    if (c_type_ == LEX_EQ || c_type_ == LEX_NEQ) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        E4();
        CheckOp();
    }
}

void Parser::E4() {
    E5();
    if (c_type_ == LEX_LSS || c_type_ == LEX_GTR || c_type_ == LEX_LEQ ||
        c_type_ == LEX_GEQ) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        E5();
        CheckOp();
    }
}

void Parser::E5() {
    T();
    while (c_type_ == LEX_PLUS || c_type_ == LEX_MINUS) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        T();
        CheckOp();
    }
}

void Parser::T() {
    F();
    while (c_type_ == LEX_TIMES || c_type_ == LEX_SLASH) {
        mut_ = false;
        type_stk_.push(c_type_);
        GetNextLex();
        F();
        CheckOp();
    }
}

void Parser::F() {
    if (c_type_ == LEX_STR) {
        type_stk_.push(LEX_STRING);
        poliz.push_back(cur_lex_);
        GetNextLex();
    } else if (c_type_ == LEX_IDENT) {
        CheckIdent();
        mut_ = true;
        poliz.push_back(cur_lex_);
        GetNextLex();
    } else if (c_type_ == LEX_NUM) {
        type_stk_.push(LEX_INT);
        poliz.push_back(cur_lex_);
        GetNextLex();
    } else if (c_type_ == LEX_TRUE) {
        type_stk_.push(LEX_BOOL);
        poliz.push_back(cur_lex_);
        GetNextLex();
    } else if (c_type_ == LEX_FALSE) {
        type_stk_.push(LEX_BOOL);
        poliz.push_back(cur_lex_);
        GetNextLex();
    } else if (c_type_ == LEX_NOT) {
        mut_ = false;
        type_stk_.push(LEX_NOT);
        GetNextLex();
        F();
        CheckUnary();
    } else if (c_type_ == LEX_MINUS) {
        mut_ = false;
        type_stk_.push(LEX_MINUS);
        GetNextLex();
        F();
        CheckUnary();
    } else if (c_type_ == LEX_PLUS) {
        mut_ = false;
        type_stk_.push(LEX_PLUS);
        GetNextLex();
        F();
        CheckUnary();
    } else if (c_type_ == LEX_LPAREN) {
        mut_ = false;
        GetNextLex();
        Expression();
        if (c_type_ != LEX_RPAREN) {
            err_stk.push_back({SYNT_NO_CLPAREN, line_count});
            ErrorHandler();
        }
        GetNextLex();
    } else {
        err_stk.push_back({WRONG_EXPR, line_count});
    }
}

void Parser::CheckIdent() {
    if (TID[c_val_].get_declare()) {
        type_stk_.push(TID[c_val_].get_type());
    } else {
        err_stk.push_back({SEM_NOT_DECLARE, line_count});
        ErrorHandler();
    }
}

void Parser::CheckOp() {
    TypeOfLex t1, t2, op;
    GetFromSt(type_stk_, t2);
    GetFromSt(type_stk_, op);
    GetFromSt(type_stk_, t1);

    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES ||
        op == LEX_SLASH) {
        if (t1 == t2 && t1 == LEX_INT) {
            type_stk_.push(LEX_INT);
        } else if (t1 == t2 && t1 == LEX_STRING && op == LEX_PLUS) {
            type_stk_.push(LEX_STRING);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    } else if (op == LEX_LSS || op == LEX_GTR || op == LEX_LEQ ||
               op == LEX_GEQ || op == LEX_EQ || op == LEX_NEQ) {
        if (t1 == t2 && (t1 == LEX_INT || t1 == LEX_STRING)) {
            type_stk_.push(LEX_BOOL);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    } else if (op == LEX_AND || op == LEX_OR) {
        if (t1 == t2 && t1 == LEX_BOOL) {
            type_stk_.push(LEX_BOOL);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    } else if (op == LEX_ASSIGN) {
        if (t1 == t2) {
            type_stk_.push(t1);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    }
    poliz.push_back(Lex(op));
}

void Parser::CheckUnary() {
    TypeOfLex op, t1;
    GetFromSt(type_stk_, op);
    TypeOfLex un_op = op;
    GetFromSt(type_stk_, t1);
    if (op == LEX_NOT) {
        if (t1 == LEX_BOOL) {
            type_stk_.push(LEX_BOOL);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    } else if (op == LEX_MINUS || op == LEX_PLUS) {
        un_op = op == LEX_MINUS ? LEX_UN_MINUS : LEX_UN_PLUS;
        if (t1 == LEX_INT) {
            type_stk_.push(LEX_INT);
        } else {
            err_stk.push_back({SEM_WRONG_TYPE, line_count});
        }
    }
    poliz.push_back(Lex(un_op));
}

void Parser::EqBool() {
    if (type_stk_.top() != LEX_BOOL) {
        err_stk.push_back({SEM_WRONG_TYPE, line_count});
    }
}

class Executer {
    public:
        void execute(vector<Lex> &poliz);
};

//void Executer::execute(vector<Lex> &poliz) {
    //Lex cur_lex;
    //stack<variant<int, bool, string>> args;
    //size_t i, j, cur_i, size = poliz.size();
    //while (cur_i < size) {
        //cur_lex = poliz[cur_i];
        //switch (cur_lex.get_type()) {
            //case LEX_TRUE:
            //case LEX_FALSE:
            //case LEX_NUM:
            //case POLIZ_ADDRESS:
            //case POLIZ_LABEL:
                //args.push(cur_lex.get_value());
                //break;
            //case LEX_STR:
                //args.push(cur_lex.get_holder());
                //break;
            //case LEX_IDENT:
                //i = cur_lex.get_value();
                //if (TID[i].get_assign()) {
                    

int main() {
    Scanner prog("tests/test3");
    Lex cur_lex = prog.GetLex();
    while (cur_lex.get_type() != LEX_FIN) {
        cout << cur_lex;
        cur_lex = prog.GetLex();
    }
    TID.clear();
    line_count = 0;
    Parser test_prog("tests/test3");
    test_prog.StartAnalysis();
    cout << "end\n";
}
