#include <string>
#include <vector>
#include "../lexer/token.h"

#ifndef NODE_CLASS
#define NODE_CLASS

class Node{
    public:
        Token token;
        std::vector<Node*> children;

        Node(){}

        Node(Token _token){
            token = _token;
        }

        ~Node(){
            for(auto child : children){
                delete child;
            }
            delete this;
        }
};

#endif // NODE_CLASS