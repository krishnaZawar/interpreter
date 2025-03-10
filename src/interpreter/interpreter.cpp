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
        std::unordered_map<std::string, int> intValue;
        std::unordered_map<std::string, std::string> strValue;

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
        inline void throwTypeError(std::string message, int line){
            throw TypeError(message, line);
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

        inline int evaluateNumericIdentifier(Node* root){
            std::string var = interpretIdentifierNode(root);
            if(datatype.find(var) == datatype.end()){
                std::string message = "";
                message += "Undeclared variable " + var + " is used";
                throwKeyError(message, root->token.line);
            }
            if(datatype[var] != 'n'){
                std::string message = "cannot implicitly convert string to integer";
                throwTypeError(message, root->token.line);
            }
            return intValue[var];
        }

        inline std::string evaluateStringIdentifier(Node* root){
            std::string var = interpretIdentifierNode(root);
            if(datatype.find(var) == datatype.end()){
                std::string message = "";
                message += "Undeclared variable " + var + " is used";
                throwKeyError(message, root->token.line);
            }
            if(datatype[var] != 's'){
                std::string message = "cannot implicitly convert integer to string";
                throwTypeError(message, root->token.line);
            }
            return strValue[var];
        }

        bool isArithmeticExpression(Node* exprHead){
            if(exprHead->token.type == NUMERICLITERAL){
                return true;
            }
            else if(exprHead->token.type == STRINGLITERAL){
                return false;
            }
            else if(exprHead->token.type == IDENTIFIER){
                if(datatype.find(exprHead->token.value) == datatype.end()){
                    throwKeyError("Undeclared variable " + exprHead->token.value, exprHead->token.line);
                }
                return datatype[exprHead->token.value] == 'n';
            }
            else if(exprHead->token.type == ARITHMETICOPERATOR){
                if(exprHead->token.value == "+"){
                    Node* temp = exprHead->children[0];
                    while(temp && temp->token.type == ARITHMETICOPERATOR){
                        temp = temp->children[0];
                    }
                    if(temp->token.type == NUMERICLITERAL){
                        return true;
                    }
                    else if(temp->token.type == STRINGLITERAL){
                        return false;
                    }
                    else if(temp->token.type == IDENTIFIER){
                        std::string tempVar = interpretIdentifierNode(temp);
                        if(datatype.find(tempVar) == datatype.end()){
                            throwKeyError("undeclared variable " + tempVar, temp->token.line);
                        }
                        return datatype[tempVar] == 'n';
                    }
                    else{
                        return (exprHead->token.value == "input" || exprHead->token.value == "string")? false : true;
                    }
                }
                else{
                    return true;
                }
            }
            return (exprHead->token.value == "input" || exprHead->token.value == "string")? false : true;
        }

    //-------------------------------------------------------evalaution for arithmetic and string expression-----------------------------------------------
        
        // --------------------------------------------------------arithmetic expression-----------------------------------------
        int evaluateArithmeticOperatorNode(Node* root){
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
                if(leftExpr->token.value == "int"){
                    leftVal = interpretIntFunction(leftExpr);
                }
                else if(leftExpr->token.value == "len"){
                    leftVal = interpretLenFunction(leftExpr);
                }
                else if(leftExpr->token.value == "string" || leftExpr->token.value == "input"){
                    throwTypeError("expected an integer value", leftExpr->token.line);
                }
                else{
                    throwSyntaxError("invalid syntax", leftExpr->token.line);
                }
            }
            else if(leftExpr->token.type == STRINGLITERAL){
                throwTypeError("cannot perform arithmetic operations on strings", leftExpr->token.line);
            }
            else{
                leftVal = evaluateArithmeticOperatorNode(leftExpr);
            }

            // evaluate right side
            if(rightExpr->token.type == NUMERICLITERAL){
                rightVal = evaluateNumericLiteralNode(rightExpr);
            }
            else if(rightExpr->token.type == IDENTIFIER){
                rightVal = evaluateNumericIdentifier(rightExpr);
            }
            else if(rightExpr->token.type == ACTIVITY){
                if(rightExpr->token.value == "int"){
                    rightVal = interpretIntFunction(rightExpr);
                }
                else if(rightExpr->token.value == "len"){
                    rightVal = interpretLenFunction(rightExpr);
                }
                else if(rightExpr->token.value == "string" || rightExpr->token.value == "input"){
                    throwTypeError("expected an integer value", rightExpr->token.line);
                }
                else{
                    throwSyntaxError("invalid syntax", rightExpr->token.line);
                }
            }
            else if(rightExpr->token.type == STRINGLITERAL){
                throwTypeError("cannot perform arithmetic operations on strings", rightExpr->token.line);
            }
            else{
                rightVal = evaluateArithmeticOperatorNode(rightExpr);
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

            if(root->token.type == STRINGLITERAL){
                throwTypeError("cannot perform arithmetic operations on strings", root->token.line);
            }

            if(root->token.type == IDENTIFIER){
                return evaluateNumericIdentifier(root);
            }

            if(root->token.type == ACTIVITY){
                if(root->token.value == "int"){
                    return interpretIntFunction(root);
                }
                else if(root->token.value == "len"){
                    return interpretLenFunction(root);
                }
                else if(root->token.value == "string" || root->token.value == "input"){
                    throwTypeError("expected an integer value", root->token.line);
                }
                else{
                    throwSyntaxError("invalid syntax", root->token.line);
                }
            }

            return evaluateArithmeticOperatorNode(root);
        }

        // -----------------------------------------------------string expression--------------------------------------------------
        
        std::string evaluateStringOperatorNode(Node* root){
            if(root->token.value != "+"){
                throwSyntaxError("Cannot perform \'" + root->token.value + "\' operation on strings", root->token.line);
            }
            std::string leftVal = evaluateStringExpression(root->children[0]);
            std::string rightVal = evaluateStringExpression(root->children[1]);

            return leftVal + rightVal;
        }
        std::string evaluateStringExpression(Node* root){
            if(root->token.type == STRINGLITERAL){
                return evaluateStringLiteralNode(root);
            }
            else if(root->token.type == IDENTIFIER){
                return evaluateStringIdentifier(root);
            }
            else if(root->token.type == ACTIVITY){
                if(root->token.value == "string"){
                    return interpretStringFunction(root);
                }
                else if(root->token.value == "input"){
                    return interpretInputFunction(root);
                }
                else if(root->token.value == "int" || root->token.value == "len"){
                    throwTypeError("expected a string value", root->token.line);
                }
            }
            else if(root->token.type == NUMERICLITERAL){
                throwTypeError("cannot add a string and an int", root->token.line);
            }
            return evaluateStringOperatorNode(root);
        }


    // ----------------------------------------------------boolean expression-----------------------------------------------
        
        bool evaluateBooleanCondition(Node* root){
            Node* leftExpr = root->children[0];
            Node* rightExpr = root->children[1];
            
            bool isLeftArithmetic = isArithmeticExpression(leftExpr);
            bool isRightArithmetic = isArithmeticExpression(rightExpr);
            if(isLeftArithmetic != isRightArithmetic){
                throwTypeError("values of the different datatypes cannot be compared", root->token.line);
            }
            std::string leftVal;
            std::string rightVal;
            if(isLeftArithmetic){
                leftVal = std::to_string(evaluateArithmeticExpression(leftExpr));
                rightVal = std::to_string(evaluateArithmeticExpression(rightExpr));
            }
            else{
                leftVal = evaluateStringExpression(leftExpr);
                rightVal = evaluateStringExpression(rightExpr);
            }

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

    // ----------------------------------------------------builtins-------------------------------------------------------
        inline std::string interpretInputFunction(Node* root){
            for(auto child : root->children){
                std::cout<<child->token.value;
            }
            std::string value;
            std::cin>>value;
            return value;
        }

        inline int interpretIntFunction(Node* root){
            int value;
            try{
                value = std::stoi(evaluateStringExpression(root->children[0]));
            }
            catch(...){
                throwTypeError("\'int\' function accepts only string as an argument", root->token.line);
            }
            return value;
        }
        inline std::string interpretStringFunction(Node* root){
            std::string value;
            try{
                value = std::to_string(evaluateArithmeticExpression(root->children[0]));
            }
            catch(...){
                throwTypeError("\'string\' function accepts only integer as an argument", root->token.line);
            }
            return value;
        }
        inline int interpretLenFunction(Node* root){
            int value;
            try{
                value = evaluateStringExpression(root->children[0]).length();
            }
            catch(...){
                throwTypeError("\'len\' function accepts only string as an argument", root->token.line);
            }
            return value;
        }
    //-----------------------------------------------------assignment statement----------------------------------------------

        void interpretAssignmentStatement(Node* root){
            std::string var = interpretIdentifierNode(root->children[0]);
            
            Node* exprHead = root->children[1];

            if(isArithmeticExpression(exprHead)){
                datatype[var] = 'n';
                intValue[var] = evaluateArithmeticExpression(exprHead);
            }
            else{
                datatype[var] = 's';
                strValue[var] = evaluateStringExpression(exprHead);
            }
        }

    // -----------------------------------------------------print statement---------------------------------------------------

        void interpretPrintStatement(Node* root){
            std::string output = "";
            for(auto child : root->children){
                if(isArithmeticExpression(child)){
                    output += std::to_string(evaluateArithmeticExpression(child));
                }
                else{
                    output += evaluateStringExpression(child);
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
            intValue.clear();
            strValue.clear();

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