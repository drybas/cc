#pragma once

#include <variant>
#include <span>
#include <vector>

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

    struct Grouping {
        char type{};
    };

    struct Eof {
    };

    using Token = std::variant<Num, Operation, Grouping, Condition, Eof>;
    using TokenVec = std::vector<Token>;
    using TokenSpan = std::span<Token>;
};

