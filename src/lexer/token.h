#include <string>
#include "tokenType.h"

#ifndef TOKEN_CLASS
#define TOKEN_CLASS

class Token{
    public:
        std::string value;
        TokenType type;
        int line;

        Token() {}
        
        Token(std::string _value, TokenType _type, int _line){
            value = _value;
            type = _type;
            line = _line;
        }

        Token(const Token &t){
            value = t.value;
            type = t.type;
            line = t.line;
        }

        const bool operator !=(const Token &token){
            return token.type != type || (token.type == type && token.value != value);
        }
};

#endif  // TOKEN_CLASS