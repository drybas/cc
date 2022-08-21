#pragma once

namespace Error {
    enum ErrorKind {
        UnexpectedSymbol,
        ExpectedSymbol,
        ExpectPrimary,
        ExpectLvalue,
    };

    void raise(ErrorKind e, char ch = '\0');
}