#include <iostream>
#include <vector>
#include <variant>
#include <cctype>
#include <exception>

struct Num {
    int value {};
};

struct Operation {
    char type {};
};

struct Eof {};

using Token = std::variant<Num, Operation, Eof>;
using TokenVec = std::vector<Token>;

auto parseLine(const std::string& line) -> TokenVec {
    TokenVec tokens;

    auto i = std::cbegin(line);
    while (i != std::cend(line)) {
        if (*i == ' ') {
            i++;
            continue;
        } else if (*i == '+' || *i == '-' || *i == '*' || *i == '/') {
            tokens.emplace_back(Token{Operation{*i}});
            i++;
            continue;
        } else if (std::isdigit(*i) > 0) {
            auto e = std::find_if_not(i, std::end(line), [](auto c) -> bool { return std::isdigit(c) > 0; });
            auto num = std::stoi(std::string{i, e});
            tokens.emplace_back(Token{Num{num}});
            if (e == std::cend(line)) {
                break;
            } else {
                i = ++e;
                continue;
            }
        }

        throw std::logic_error("unexpected character");
    }

    tokens.emplace_back(Eof{});
    return tokens;
}

auto expectNum(Token token) -> int {
    if (const Num* p = std::get_if<Num>(&token)) {
        return p->value;
    } else {
        throw std::logic_error("error: expects a number");
    }
}

auto match(Token token, char ch) -> bool {
    if (const Operation* op = std::get_if<Operation>(&token)) {
       return op->type == ch;
    }
    return false;
}

void generateCode(TokenVec& tokens) {
    std::cout << ".intel_syntax noprefix\n";
    std::cout << ".global main\n";
    std::cout << " main:\n";

    auto i = std::begin(tokens);
    auto num = expectNum(*i++);
    std::cout << " mov ax, " << num << "\n";

    while (i != tokens.end()) {
        if (match(*i, '+')) {
            i++;
            num = expectNum(*i);
            std::cout << " add ax, " << num << " \n";
            continue;
        }

        if (match(*i, '-')) {
            i++;
            num = expectNum(*i);
            std::cout << " sub ax, " << num << "\n";
            continue;
        }

        i++;
    }
    std::cout << " ret\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
       std::cout << "error: no arguments were specified";
       return 1;
    }
    try {
        auto tokens = parseLine(argv[1]);
        generateCode(tokens);

        return 0;
    } catch (const std::exception& ex) {
        std::cout << ex.what();
        return 1;
    }
}
