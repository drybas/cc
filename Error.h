#pragma once

namespace Error {
    enum ErrorKind {
        UnexpectedSymbol
    };

    void raise(ErrorKind e, char ch);
}