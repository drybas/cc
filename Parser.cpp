//
// Created by Dmitry Rybas on 16.08.2022.
//

#include "Parser.h"
#include "Error.h"

#include <sstream>

// stmt = expr ";"
// expr = assign
// assign = eq ("=" assign)?
// eq = rel ("==" rel | "!=" rel)*
// rel = add (">" add | "<" add | ">=" add | "<=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = primary ("*" unary | "/" unary)*
// unary = ("+" | "-")? primary
// primary = num | "(" expr ")"

// term rel | term rel
// const char* , token type, function const char*
//
//

template<typename T>
auto Parser::consume(Token::TokenSpan& s, char ch) -> bool {
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
auto Parser::consume(Token::TokenSpan& s, std::string_view value) -> bool {
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

template<typename T>
void expect(Token::TokenSpan& tokens, char ch);

auto Parser::stmt(Token::TokenSpan& s) -> Ast::Node {
    auto node = expr(s);
    if (consume<Token::Punctuation>(s, ';')) {
        return node;
    } else {
        Error::raise(Error::ExpectedSymbol, ';');
    }

    return Ast::Node{};
}

auto Parser::expr(Token::TokenSpan& s) -> Ast::Node {
    auto node = Parser::assign(s);
    return node;
}

auto Parser::assign(Token::TokenSpan& s) -> Ast::Node {
    auto node = Parser::eq(s);
    for (;;) {
        if (consume<Token::Assignment>(s, '=')) {
            node = Ast::Node { Ast::Assign {
                    .left = std::make_unique<Ast::Node>(std::move(node)),
                    .right = std::make_unique<Ast::Node>(std::move(assign(s))) }};
        } else {
            return node;
        }
    }
}

auto Parser::eq(Token::TokenSpan& s) -> Ast::Node {
    // eq = rel ("==" rel | "!=" rel)*
    auto node = Parser::rel(s);
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
}

auto Parser::rel(Token::TokenSpan& s) -> Ast::Node {
    auto node = Parser::add(s);

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

auto Parser::add(Token::TokenSpan& s) ->Ast::Node {
    auto node = Parser::mul(s);

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

/*template<typename F0, typename TokenKind, typename >
auto t_mul(Token::TokenSpan& tokens) -> Ast::Node {
    auto node = F0(tokens);
    for(;;) {
       if (consume<TokenKind>()) {

       }
    }
    return Ast::Node{ };
}*/

auto Parser::mul(Token::TokenSpan& tokens) -> Ast::Node {
    auto node = Parser::unary(tokens);

    for (;;) {
        if (consume<Token::Operation>(tokens, '*')) //
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

auto Parser::unary(Token::TokenSpan& tokens) -> Ast::Node {
    if (consume<Token::Operation>(tokens, '+'))
        return primary(tokens);

    if (consume<Token::Operation>(tokens, '-'))
        return Ast::Node { Ast::Operation {
                .kind = Ast::Operation::Sub,
                .left = std::make_unique<Ast::Node>(Ast::Node { Ast::Num{.value = 0}}),
                .right = std::make_unique<Ast::Node>(std::move(primary(tokens))) }};

    return primary(tokens);
}

auto Parser::primary(Token::TokenSpan& tokens) -> Ast::Node {
    if (consume<Token::Punctuation>(tokens, '(')) {
        auto node = expr(tokens);
        expect<Token::Punctuation>(tokens, ')');
        return node;
    }

    if (const Token::Num* p = std::get_if<Token::Num>(&tokens.front())) {
        tokens = tokens.subspan(1);
        return Ast::Node { Ast::Num{ .value = p->value }};
    } else if (const Token::Variable* v = std::get_if<Token::Variable>(&tokens.front())) {
        tokens = tokens.subspan(1);
        return Ast::Node { Ast::Lval{ .name = v->name }};
    }

    Error::raise(Error::ExpectPrimary);
}

template<typename T>
void Parser::expect(Token::TokenSpan& tokens, char ch) {
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