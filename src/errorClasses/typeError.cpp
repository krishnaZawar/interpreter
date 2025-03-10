#include "error.h"

#ifndef TYPE_ERROR_CLASS
#define TYPE_ERROR_CLASS

class TypeError : public Error{
    public:
    TypeError(std::string message, int errorLine) : Error("TypeError", message, errorLine) {}
};

#endif //  TYPE_ERROR_CLASS