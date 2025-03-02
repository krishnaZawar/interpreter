#include "error.h"

#ifndef TOKEN_ERROR_CLASS
#define TOKEN_ERROR_CLASS

class TokenError : public Error{
    public:
        TokenError(std::string message, int errorLine) : Error("TokenError", message, errorLine) {}
};

#endif  // TOKEN_ERROR_CLASS