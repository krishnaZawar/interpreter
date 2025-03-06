#include "../lexer/token.h"

#ifndef LOOP_CONTROL_INTERRUPT_CLASS
#define LOOP_CONTROL_INTERRUPT_CLASS

class LoopControlInterrupt{
    public:
        Token token;

        LoopControlInterrupt(Token _token){
            token = _token;
        }
};

#endif // LOOP_CONTROL_INTERRUPT_CLASS