#include <string>
#include "tokenType.h"

#ifndef TOKEN_CLASS
#define TOKEN_CLASS

class Token{
    public:
        std::string value;
        TokenType type;

        Token() {}
        
        Token(std::string _value, TokenType _type){
            value = _value;
            type = _type;
        }

        Token(const Token &t){
            value = t.value;
            type = t.type;
        }

        const bool operator !=(const Token &token){
            return token.type != type || (token.type == type && token.value != value);
        }
};

#endif  // TOKEN_CLASS