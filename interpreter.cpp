#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

enum Error {
};

std::unordered_set<std::string> reserved_words {
    "program", "int", "real", "string", "boolean", "if", "else",
    "while", "read", "write", "for", "break", "true", "false"
};

std::unordered_set<std::string> types {
    "int", "real", "boolean"
};

std::unordered_set<char> delimeters {
    ' ', '\n', '\t', '*', '%', '+', '-', '<', '>', '=', '!', ',',
    ';', ':', '(', ')', '/'
};

class Identifier {
    public:
        std::string name, type;
        std::variant<int, bool, std::string> value;
};

int main() {
    return 0;
}
