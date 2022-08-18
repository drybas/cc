//
// Created by Dmitry Rybas on 16.08.2022.
//

#pragma once

#include "Token.h"
#include "Ast.h"

#include <string_view>
#include <list>

struct LVar
{
   std::string name;
   size_t      index;
};

class Parser {
    typedef std::vector<LVar> Vars;

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

    size_t locals_count() const { return m_locals.size(); }
private:
    template<typename T>
    auto consume(Token::TokenSpan& s, char ch) -> bool;

    template<typename T>
    auto consume(Token::TokenSpan& s, std::string_view value) -> bool;

    template<typename T>
    void expect(Token::TokenSpan& tokens, char ch);

private:
    auto find_local_var(const std::string& name) const -> std::pair<bool, Vars::const_iterator>;
    auto add_local_var(const std::string& name) -> LVar&;

private:
    Vars m_locals;
};