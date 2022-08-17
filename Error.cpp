#include "Error.h"
#include <sstream>

void Error::raise(Error::ErrorKind e, char ch) {
    std::stringstream ss;
    switch (e) {
        case Error::UnexpectedSymbol:
            ss << "Unexpected symbol '" << ch << "'";
            break;
        case Error::ExpectedSymbol:
            ss << "Expects " << ch << "";
            break;
        case Error::ExpectPrimary:
            ss << "Expects primary";
            break;
    }

    throw std::logic_error(ss.str().c_str());
}