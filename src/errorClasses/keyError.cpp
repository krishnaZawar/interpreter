#include "error.h"

#ifndef KEY_ERROR_CLASS
#define KEY_ERROR_CLASS

class KeyError : public Error{
    public:
    KeyError(std::string message, int errorLine) : Error("KeyError", message, errorLine) {}
};

#endif //  KEY_ERROR_CLASS