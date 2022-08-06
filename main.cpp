#include <iostream>
#include <variant>
#include <exception>
#include <sstream>

#include "Token.h"
#include "Ast.h"
#include "Codegen.h"
#include "Scanner.h"

// expr = rel ("==" rel | "!=" rel)*
// rel = add (">" add | "<" add | ">=" add | "<=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = primary ("*" unary | "/" unary)*
// unary = ("+" | "-")? primary
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

template<typename T>
auto consume(Token::TokenSpan& s, std::string_view value) -> bool {
    if (s.empty())
        return false;
    if (const auto p = std::get_if<T>(&s.front())) {
        if (p->type == value) {
            s = s.subspan(1);
            return true;
        }
    }

    return false;
}

auto rel(Token::TokenSpan& tokens) -> Ast::Node;
auto add(Token::TokenSpan& tokens) -> Ast::Node;
auto mul(Token::TokenSpan& tokens) -> Ast::Node;
auto unary(Token::TokenSpan& tokens) -> Ast::Node;
auto primary(Token::TokenSpan& s) -> Ast::Node;
auto expectNum(Token::TokenSpan& token) -> int;

template<typename T>
void expect(Token::TokenSpan& tokens, char ch);

auto expr(Token::TokenSpan& s) -> Ast::Node {
    auto node = rel(s);
    for (;;) {
       if (consume<Token::Condition>(s, "=="))
           node = Ast::Node { Ast::Condition {
                   .kind = Ast::Condition::Eq,
                   .left =std::make_unique<Ast::Node>(std::move(node)),
                   .right = std::make_unique<Ast::Node>(rel(s)) }};
       else if (consume<Token::Condition>(s, "!="))
           node = Ast::Node { Ast::Condition{
                   .kind = Ast::Condition::NotEq,
                   .left = std::make_unique<Ast::Node>(std::move(node)),
                   .right = std::make_unique<Ast::Node>(rel(s)) }};
       else
           return node;
    }
// expr = rel ("==" rel | "!=" rel)*
}

auto rel(Token::TokenSpan& s) -> Ast::Node {
    auto node = add(s);

    for (;;) {
        if (consume<Token::Condition>(s, ">="))
            node = Ast::Node { Ast::Condition {
                    .kind = Ast::Condition::GreaterEq,
                    .left =std::make_unique<Ast::Node>(std::move(node)),
                    .right = std::make_unique<Ast::Node>(add(s)) }};
        else if (consume<Token::Condition>(s, ">"))
            node = Ast::Node { Ast::Condition{
                    .kind = Ast::Condition::Greater,
                    .left = std::make_unique<Ast::Node>(std::move(node)),
                    .right = std::make_unique<Ast::Node>(add(s)) }};
        else if (consume<Token::Condition>(s, "<="))
            node = Ast::Node { Ast::Condition {
                    .kind = Ast::Condition::LessEq,
                    .left =std::make_unique<Ast::Node>(std::move(node)),
                    .right = std::make_unique<Ast::Node>(add(s)) }};
        else if (consume<Token::Condition>(s, "<"))
            node = Ast::Node { Ast::Condition{
                    .kind = Ast::Condition::Less,
                    .left = std::make_unique<Ast::Node>(std::move(node)),
                    .right = std::make_unique<Ast::Node>(add(s)) }};
        else
            return node;
    }

// rel = add (">" add | "<" add | ">=" add | "<=" add)*
}

auto add(Token::TokenSpan& s) ->Ast::Node {
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
    auto node = unary(tokens);

    for (;;) {
        if (consume<Token::Operation>(tokens, '*'))
            node = Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Mul,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(unary(tokens)) }};
        else if (consume<Token::Operation>(tokens, '/'))
            node = Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Div,
                .left = std::make_unique<Ast::Node>(std::move(node)),
                .right = std::make_unique<Ast::Node>(unary(tokens)) }};
        else
            return node;
    }
}

auto unary(Token::TokenSpan& tokens) -> Ast::Node {
    if (consume<Token::Operation>(tokens, '+'))
        return primary(tokens);

    if (consume<Token::Operation>(tokens, '-'))
        return Ast::Node { Ast::Operation {
            .kind = Ast::Operation::Sub,
            .left = std::make_unique<Ast::Node>(Ast::Node { Ast::Num{.value = 0}}),
            .right = std::make_unique<Ast::Node>(std::move(primary(tokens))) }};

    return primary(tokens);
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

auto expectNum(Token::TokenSpan& tokens) -> int {
    if (const Token::Num* p = std::get_if<Token::Num>(&tokens.front())) {
        tokens = tokens.subspan(1);
        return p->value;
    } else {
        throw std::logic_error("expects a number");
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
       std::cout << "no arguments were specified \n";
       return 1;
    }
    try {
        Scanner scanner(argv[1]);
        auto tokens = scanner.scan();
        Token::TokenSpan sp(tokens);
        auto ast = expr(sp);

        Codegen generator;
        generator.generate(ast);

        return 0;
    } catch (const std::exception& ex) {
        std::cout << ex.what();
        return 1;
    }
}
