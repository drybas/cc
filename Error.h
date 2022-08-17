#pragma once

namespace Error {
    enum ErrorKind {
        UnexpectedSymbol,
        ExpectedSymbol,
        ExpectPrimary,
    };

    void raise(ErrorKind e, char ch = '\0');
}