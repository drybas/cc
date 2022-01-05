#include "Scanner.h"

#include<stdexcept>

Scanner::Scanner(const std::string& line) {
    m_source = line;
    m_it = std::cbegin(m_source);
    m_end = std::cend(m_source);
}

auto Scanner::scan() -> Token::TokenVec {
    Token::TokenVec tokens;
    while (m_it != m_end) {
        if (*m_it == ' ') {
            m_it++;
            continue;
        } else if (*m_it == '+' || *m_it == '-' || *m_it == '*' || *m_it == '/') {
            tokens.emplace_back(Token::Token{Token::Operation{*m_it}});
            m_it++;
            continue;
        } else if (*m_it == '=') {
            if (match('=')) {
                tokens.emplace_back(Token::Token{ Token::Condition { .type = '='}});
                continue;
            } else {
                throw std::logic_error("unexpected symbol");
            }
        } else if (*m_it == '(' || *m_it == ')') {
            tokens.emplace_back(Token::Token{Token::Grouping{*m_it}});
            m_it++;
            continue;
        } else if (std::isdigit(*m_it) > 0) {
            auto e = std::find_if_not(m_it, m_end, [](auto c) -> bool { return std::isdigit(c) > 0; });
            auto num = std::stoi(std::string{m_it, e});
            tokens.emplace_back(Token::Token{Token::Num{num}});
            if (e == m_end) {
                break;
            } else {
                m_it = e;
                continue;
            }
        }

        throw std::logic_error("unexpected character");
    }

    tokens.emplace_back(Token::Eof{});
    return tokens;
}

auto Scanner::match(char ch) -> bool {
    if (m_it != m_end && *m_it == ch) {
        *m_it++;
        return true;
    }

    return false;
}
