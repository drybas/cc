#pragma once

#include <variant>
#include <span>
#include <vector>
#include <string>

namespace Token {
    struct Num {
        int value{};
    };

    struct Operation {
        char type{};
    };

    struct Condition {
        std::string type{};
    };

    struct Variable {
       std::string name{};
       size_t offset = 0;
    };

    struct Punctuation {
        char type{};
    };

    struct Assignment {
        char type { '=' };
    };

    struct Eof {
    };

    struct Return {
    };

    using Token = std::variant<Num, Operation, Punctuation, Assignment, Variable, Condition, Return, Eof>;
    using TokenVec = std::vector<Token>;
    using TokenSpan = std::span<Token>;
};

