#pragma once

#include <variant>
#include <span>

namespace Token {
    struct Num {
        int value{};
    };

    struct Operation {
        char type{};
    };

    struct Grouping {
        char type{};
    };

    struct Eof {
    };

    using Token = std::variant<Num, Operation, Grouping, Eof>;
    using TokenVec = std::vector<Token>;
    using TokenSpan = std::span<Token>;
};

