#include <iostream>
#include <vector>
#include <variant>
#include <cctype>
#include <exception>
#include <sstream>

#include "Token.h"
#include "Ast.h"
#include "Codegen.h"

// expr = mul ("+" mul | "-" mul)*
// mul = primary ("*" primary | "/" primary)*
// primary = num | "(" expr ")"

template<typename T>
auto consume(Token::TokenSpan& s, char ch) -> bool {
    if (s.empty())
        return false;
    if (const auto p = std::get_if<T>(&s.front())) {
        if (p->type == ch) {
            s = s.subspan(1);
            return true;
        }
    }

    return false;
}

auto mul(Token::TokenSpan& tokens) -> Ast::Node;
auto primary(Token::TokenSpan& s) -> Ast::Node;
auto expectNum(Token::TokenSpan& token) -> int;

template<typename T>
void expect(Token::TokenSpan& tokens, char ch);

auto expr(Token::TokenSpan& s) -> Ast::Node {
    auto node = mul(s);

    for (;;) {
        if (consume<Token::Operation>(s, '+'))
            node = Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Add,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(mul(s)) }};
        else if (consume<Token::Operation>(s, '-'))
            node = Ast::Node { Ast::Operation{
                .kind = Ast::Operation::Sub,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(std::move(mul(s))) }};
        else
            return node;
    }
}

auto mul(Token::TokenSpan& tokens) -> Ast::Node {
    auto node = primary(tokens);

    for (;;) {
        if (consume<Token::Operation>(tokens, '*'))
            node = Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Mul,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(primary(tokens)) }};
        else if (consume<Token::Operation>(tokens, '/'))
            node = Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Div,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(primary(tokens)) }};
        else
            return node;
    }
}

auto primary(Token::TokenSpan& tokens) -> Ast::Node {
    if (consume<Token::Grouping>(tokens, '(')) {
        auto node = expr(tokens);
        expect<Token::Grouping>(tokens, ')');
        return node;
    }

    return Ast::Node { Ast::Num{ .value = expectNum(tokens) }};
}

template<typename T>
void expect(Token::TokenSpan& tokens, char ch) {
    if (const auto p = std::get_if<T>(&tokens.front())) {
        if (p->type == ch) {
            tokens = tokens.subspan(1);
            return;
        }
    }
    std::stringstream msg;
    msg << "expected " << ch;
    throw std::logic_error(msg.str());
}

auto parseLine(const std::string& line) -> Token::TokenVec {
    Token::TokenVec tokens;

    auto i = std::cbegin(line);
    while (i != std::cend(line)) {
        if (*i == ' ') {
            i++;
            continue;
        } else if (*i == '+' || *i == '-' || *i == '*' || *i == '/') {
            tokens.emplace_back(Token::Token{Token::Operation{*i}});
            i++;
            continue;
        } else if (*i == '(' || *i == ')') {
            tokens.emplace_back(Token::Token{Token::Grouping{*i}});
            i++;
            continue;
        } else if (std::isdigit(*i) > 0) {
            auto e = std::find_if_not(i, std::end(line), [](auto c) -> bool { return std::isdigit(c) > 0; });
            auto num = std::stoi(std::string{i, e});
            tokens.emplace_back(Token::Token{Token::Num{num}});
            if (e == std::cend(line)) {
                break;
            } else {
                i = e;
                continue;
            }
        }

        throw std::logic_error("unexpected character");
    }

    tokens.emplace_back(Token::Eof{});
    return tokens;
}

auto expectNum(Token::TokenSpan& tokens) -> int {
    if (const Token::Num* p = std::get_if<Token::Num>(&tokens.front())) {
        tokens = tokens.subspan(1);
        return p->value;
    } else {
        throw std::logic_error("expects a number");
    }
}

auto match(Token::Token token, char ch) -> bool {
    if (const Token::Operation* op = std::get_if<Token::Operation>(&token)) {
       return op->type == ch;
    }
    return false;
}

/*void generateCode(Token::TokenVec& tokens) {
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
} */

int main(int argc, char* argv[]) {
    if (argc == 1) {
       std::cout << "error: no arguments were specified";
       return 1;
    }
    try {
        auto tokens = parseLine(argv[1]);
        Token::TokenSpan sp(tokens);
        auto ast = expr(sp);

        Codegen generator;
        generator.generate(ast);
        //generateCode(tokens);

        return 0;
    } catch (const std::exception& ex) {
        std::cout << ex.what();
        return 1;
    }
}
