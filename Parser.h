//
// Created by Dmitry Rybas on 16.08.2022.
//

#pragma once

#include "Token.h"
#include "Ast.h"

#include <string_view>

class Parser {
public:
    auto stmt(Token::TokenSpan& s) -> Ast::Node;
    auto expr(Token::TokenSpan& s) -> Ast::Node;
    auto assign(Token::TokenSpan& s) -> Ast::Node;
    auto eq(Token::TokenSpan& s) -> Ast::Node;
    auto rel(Token::TokenSpan& tokens) -> Ast::Node;
    auto add(Token::TokenSpan& tokens) -> Ast::Node;
    auto mul(Token::TokenSpan& tokens) -> Ast::Node;
    auto unary(Token::TokenSpan& tokens) -> Ast::Node;
    auto primary(Token::TokenSpan& s) -> Ast::Node;

private:
    template<typename T>
    auto consume(Token::TokenSpan& s, char ch) -> bool;

    template<typename T>
    auto consume(Token::TokenSpan& s, std::string_view value) -> bool;

    template<typename T>
    void expect(Token::TokenSpan& tokens, char ch);
};