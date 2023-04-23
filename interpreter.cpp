#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <fstream>

enum class Error {
    FILE_NO = -1,
    FILE_BAD = -2
};

std::vector<std::pair<Error, size_t>> error_stack;

std::string data;
size_t size;

std::unordered_set<std::string> reserved_words {
    "program", "int", "real", "string", "boolean", "if", "else",
    "do", "while", "read", "write", "for", "break", "continue", "true",
    "false", "not", "and", "or", "goto"
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

int ShowErrors();
void skip_whitespace();

int main(int argc, char **argv) {        
    if (argc < 2) {
        error_stack.push_back({ Error::FILE_NO, 0 });
        return ShowErrors();
    }

    std::ifstream file(argv[1], std::ios::binary);
    std::stringstream buf;
    buf << file.rdbuf();
    data = buf.str();
    if (file.fail()) {
        error_stack.push_back({ Error::FILE_BAD, 0 });
        return ShowErrors();
    }
    std::cout << data << "\n";

    return 0;
}

int ShowErrors() {
    return 0;
}

void skip_whitespace() {
}
