#pragma once

#include <string>
#include <tuple>
#include "Token.h"

class Scanner {
public:
    Scanner(const std::string& line);
    auto scan() -> Token::TokenVec;
private:
    auto match(char ch) -> bool;
    auto peek() -> std::tuple<bool, char>;

    auto get_var_or_keyword(const std::string& text) -> Token::Token;
private:
    std::string m_source;
    std::string::const_iterator m_it;
    std::string::const_iterator m_end;
};




