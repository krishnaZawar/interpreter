#include <unordered_map>
#include <string>
#include <iostream>
#include "../parser/parser.cpp"
#include "../parser/ast.h"
#include "../lexer/tokenType.h"
#include "../errorClasses/errorClasses.h"
#include "../interrupts/interrupts.h"


#ifndef INTERPRETER_CLASS
#define INTERPRETER_CLASS

class Interpreter{
    private:
        std::unordered_map<std::string, char> datatype;
        std::unordered_map<std::string, int> numValue;

        Parser parser;

        inline void throwKeyError(std::string message, int line){
            throw KeyError(message, line);
        }
        inline void throwDivisionByZeroError(std::string message, int line){
            throw DivisionByZeroError(message, line);
        }
        inline void throwValueError(std::string message, int line){
            throw ValueError(message, line);
        }
        inline void throwSyntaxError(std::string message, int line){
            throw SyntaxError(message, line);
        }
    // --------------------------------------------------------generics-------------------------------------------------------

        inline std::string interpretIdentifierNode(Node* root){
            return root->token.value;
        }

        inline std::string evaluateStringLiteralNode(Node* root){
            return root->token.value;
        }

        inline int evaluateNumericLiteralNode(Node* root){
            return std::stoi(root->token.value);
        }

        inline bool isOfDatatype(std::string var, char type){
            if(datatype.find(var) == datatype.end()){
                return false;
            }
            return datatype[var] == type;
        }

        inline int evaluateNumericIdentifier(Node* root){
            std::string var = interpretIdentifierNode(root);
            if(!isOfDatatype(var, 'n')){
                std::string message = "Undeclared variable ";
                message += + "\'" + var + "\' used";
                throwKeyError(message, root->token.line);
            }
            return numValue[var];
        }

    //-----------------------------------------------------arithmetic expression---------------------------------------------

        int evaluateOperatorNode(Node* root){
            Node* leftExpr = root->children[0];
            Node* rightExpr = root->children[1];

            int leftVal, rightVal;

            // evaluate left side
            if(leftExpr->token.type == NUMERICLITERAL){
                leftVal = evaluateNumericLiteralNode(leftExpr);
            }
            else if(leftExpr->token.type == IDENTIFIER){
                leftVal = evaluateNumericIdentifier(leftExpr);
            }
            else if(leftExpr->token.type == ACTIVITY){
                leftVal = interpretInputFunction(leftExpr);
            }
            else{
                leftVal = evaluateOperatorNode(leftExpr);
            }

            // evaluate right side
            if(rightExpr->token.type == NUMERICLITERAL){
                rightVal = evaluateNumericLiteralNode(rightExpr);
            }
            else if(rightExpr->token.type == IDENTIFIER){
                rightVal = evaluateNumericIdentifier(rightExpr);
            }
            else if(rightExpr->token.type == ACTIVITY){
                rightVal = interpretInputFunction(rightExpr);
            }
            else{
                rightVal = evaluateOperatorNode(rightExpr);
            }
            
            if(root->token.value == "%"){
                if(rightVal == 0){
                    throwDivisionByZeroError("Cannot mod a value by 0", root->token.line);
                }
                return leftVal % rightVal;
            }
            if(root->token.value == "/"){
                if(rightVal == 0){
                    throwDivisionByZeroError("Cannot divide a value by 0", root->token.line);
                }
                return leftVal / rightVal;
            }
            if(root->token.value == "+"){
                return leftVal + rightVal;
            }
            if(root->token.value == "-"){
                return leftVal - rightVal;
            }
            return leftVal * rightVal;
        }
        int evaluateArithmeticExpression(Node* root){
            if(root->token.type == NUMERICLITERAL){
                return evaluateNumericLiteralNode(root);
            }

            if(root->token.type == IDENTIFIER){
                return evaluateNumericIdentifier(root);
            }

            if(root->token.type == ACTIVITY){
                return interpretInputFunction(root);
            }

            return evaluateOperatorNode(root);
        }

    // ----------------------------------------------------boolean expression-----------------------------------------------
        
        bool evaluateBooleanCondition(Node* root){
            int leftVal = evaluateArithmeticExpression(root->children[0]);
            int rightVal = evaluateArithmeticExpression(root->children[1]);

            if(root->token.value == ">"){
                return leftVal > rightVal;
            }
            if(root->token.value == "<"){
                return leftVal < rightVal;
            }
            if(root->token.value == ">="){
                return leftVal >= rightVal;
            }
            if(root->token.value == "<="){
                return leftVal <= rightVal;
            }
            if(root->token.value == "!="){
                return leftVal != rightVal;
            }
            return leftVal == rightVal;
        }
        bool evaluateBooleanOperator(Node* root){
            Node* leftExpr = root->children[0];
            Node* rightExpr = root->children[1];

            bool leftVal, rightVal;

            // evaluate left side
            if(leftExpr->token.type == RELATIONALOPERATOR){
                leftVal = evaluateBooleanCondition(leftExpr);
            }
            else{
                leftVal = evaluateBooleanOperator(leftExpr);
            }
            
            // evaluate right side
            if(rightExpr->token.type == RELATIONALOPERATOR){
                rightVal = evaluateBooleanCondition(rightExpr);
            }
            else{
                rightVal = evaluateBooleanOperator(rightExpr);
            }

            return (root->token.value == "and")? (leftVal && rightVal) : (leftVal || rightVal);
        }
        bool evaluateBooleanExpression(Node* root){
            if(root->token.type == RELATIONALOPERATOR){
                return evaluateBooleanCondition(root);
            }
            return evaluateBooleanOperator(root);
        }

    // ----------------------------------------------------input function--------------------------------------------------
        int interpretInputFunction(Node* root){
            for(auto child : root->children){
                std::cout<<child->token.value;
            }
            int value;
            try{
                std::string tempVal;
                std::cin>>tempVal;
                value = std::stoi(tempVal);
            }
            catch(...){
                throwValueError("cannot assign a non-integer to an integer variable.", root->token.line);
            }
            return value;
        }
    //-----------------------------------------------------assignment statement----------------------------------------------

        void interpretAssignmentStatement(Node* root){
            std::string variableToAssignValueTo = interpretIdentifierNode(root->children[0]);
            int value = evaluateArithmeticExpression(root->children[1]);

            datatype[variableToAssignValueTo] = 'n';
            numValue[variableToAssignValueTo] = value;
        }

    // -----------------------------------------------------print statement---------------------------------------------------

        void interpretPrintStatement(Node* root){
            std::string output = "";
            for(auto child : root->children){
                if(child->token.type == STRINGLITERAL){
                    output += evaluateStringLiteralNode(child);
                }
                else{
                    output += std::to_string(evaluateArithmeticExpression(child));
                }
            }
            std::cout<<output;
        }

    // -----------------------------------------------------------if else if block------------------------------------------------
        
        void interpretIfElseIfBlock(Node* root){
            if(root->token.value == "if"){
                bool value = evaluateBooleanExpression(root->children[0]);

                if(value){
                    interpretStatementList(root->children[1]);
                }
                else if(root->children.size() == 3){
                    if(root->children[2]->token.value == "if"){
                        interpretIfElseIfBlock(root->children[2]);
                    }
                    else{
                        interpretStatementList(root->children[2]);
                    }
                }
            }
        }

    // ---------------------------------------------------------------interpret while loop-------------------------------------------------
    
    void interpretWhileLoop(Node* root){
        Node* booleanExpression = root->children[0];
        Node* statementList = root->children[1];

        while(evaluateBooleanExpression(booleanExpression)){
            try{   
                interpretStatementList(statementList);
            }
            catch(LoopControlInterrupt &loopControl){
                if(loopControl.token.value == "break"){
                    return;
                }
                else if(loopControl.token.value == "continue"){
                    //pass
                }
            }
        }
    }

    // ----------------------------------------------------------------interpret program-----------------------------------------------------

        void interpretStatementList(Node* root){
            for(auto child : root->children){
                if(child->token.value == "assignment"){
                    interpretAssignmentStatement(child);
                }
                if(child->token.value == "print"){
                    interpretPrintStatement(child);
                }
                if(child->token.value == "if"){
                    interpretIfElseIfBlock(child);
                }
                if(child->token.value == "while"){
                    interpretWhileLoop(child);
                }
                if(child->token.value == "break" || child->token.value == "continue"){
                    throw LoopControlInterrupt(child->token);
                }
            }
        }

    public:
        Interpreter(){
            parser = Parser();
        }

        void interpret(std::string text){
            datatype.clear();
            numValue.clear();

            Node* ast = parser.parse(text);
            try{
                interpretStatementList(ast);
            }
            catch(LoopControlInterrupt &loopControl){
                throwSyntaxError(loopControl.token.value + " cannot be used outside a loop", loopControl.token.line);
            }
        }
};

#endif // INTERPRETER_CLASS