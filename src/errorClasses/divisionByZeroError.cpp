#include "error.h"

#ifndef DIVISION_BY_ZERO_ERROR_CLASS
#define DIVISION_BY_ZERO_ERROR_CLASS

class DivisionByZeroError : public Error{
    public:
    DivisionByZeroError(std::string message, int errorLine) : Error("DivisionByZeroError", message, errorLine) {}
};

#endif //  DIVISION_BY_ZERO_ERROR_CLASS