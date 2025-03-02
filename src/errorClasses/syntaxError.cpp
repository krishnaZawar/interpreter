#include "error.h"

#ifndef SYNTAX_ERROR_CLASS
#define SYNTAX_ERROR_CLASS

class SyntaxError : public Error{
    public:
        SyntaxError(std::string message, int errorLine) : Error("SyntaxError", message, errorLine) {}
};

#endif //  SYNTAX_ERROR_CLASS