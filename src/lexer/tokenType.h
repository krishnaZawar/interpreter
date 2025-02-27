#ifndef TOKENTYPE_ENUM
#define TOKENTYPE_ENUM

enum TokenType{
    OPENPARENTHESIS,
    CLOSEPARENTHESIS,
    KEYWORD,
    NUMERICLITERAL,
    STRINGLITERAL,
    IDENTIFIER,
    ARITHMETICOPERATOR,  
    RELATIONALOPERATOR,
    ASSIGNMENT,
    SPECIALCHARACTER,
    ENDOFLINE,
    ENDOFFILE,
    
    /* 
    represents the type of statement for easier interpretation
    it is like an abstract token
    */
    ACTIVITY
};

#endif  // TOKENTYPE_ENUM
