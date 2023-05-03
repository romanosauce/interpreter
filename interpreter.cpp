#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

enum class TypeOfLex {
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
    private:
        TypeOfLex t_lex_;
        int v_lex_;

