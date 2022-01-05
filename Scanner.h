#pragma once
#include <string>
#include "Token.h"

class Scanner {
public:
    Scanner(const std::string& line);
    auto scan() -> Token::TokenVec;
private:
    auto match(char ch) -> bool;
    std::string m_source;
    std::string::const_iterator m_it;
    std::string::const_iterator m_end;
};




