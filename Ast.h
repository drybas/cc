#pragma once

#include <memory>
#include <variant>

namespace Ast {

struct Num {
    int value {};
};

struct Node;
using NodePtr = std::unique_ptr<Node>;

struct Operation {
    enum { Add, Sub, Mul, Div} kind;
    NodePtr left;
    NodePtr right;
};

struct Condition {
    // == != < > <= >=
    enum { Eq, NotEq, Less, Greater, LessEq, GreaterEq } kind;
    NodePtr left;
    NodePtr right;
};

using NodeVar = std::variant<Num, Operation, Condition>;
struct Node {
    NodeVar var;
};

};