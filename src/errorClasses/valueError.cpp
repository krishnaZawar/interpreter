#include "error.h"

#ifndef INPUT_ERROR_CLASS
#define INPUT_ERROR_CLASS

class ValueError : public Error{
    public:
    ValueError(std::string message, int errorLine) : Error("ValueError", message, errorLine) {}
};

#endif //  INPUT_ERROR_CLASS