#include <string>

#ifndef ERROR_CLASS
#define ERROR_CLASS

class Error{
    protected:
        std::string name;
        std::string message;
        int errorLine;
    public:
        Error(std::string _name, std::string _message, int _errorLine){
            name = _name;
            message = _message;
            errorLine = _errorLine;
        }

        std::string getMessage(){
            return name + ": line " + std::to_string(errorLine) + ": " + message;
        }
};

#endif  // ERROR_CLASS