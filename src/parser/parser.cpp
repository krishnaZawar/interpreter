#include<string>
#include "ast.h"
#include "../lexer/lexer.cpp"
#include "../lexer/token.h"
#include "../lexer/tokenType.h"
#include "../errorClasses/errorClasses.h"

#ifndef PARSER_CLASS
#define PARSER_CLASS

class Parser{
    private:
        Lexer lexer;
        Token curToken;

        inline void throwSyntaxError(std::string message){
            throw SyntaxError(message, curToken.line);
        }

    // -----------------------------------------------------------generic functions---------------------------------------------------------

        /*
            checks if the current token is of the expected type and proceeds to the next token in input
        */
        inline void eat(TokenType type){
            if(curToken.type == type){
                curToken = lexer.getNextToken();
            }
            else{
                throwSyntaxError("invalid syntax");
            }
        }

        /*
            checks if the current token is of a given type
        */
        inline bool peek(TokenType type){
            return curToken.type == type;
        }
        /*
            checks if the current token holds the expected value
        */
        inline bool curTokenValue(std::string value){
            return curToken.value == value;
        }

        inline void parseParenthesis(std::string value){
            if(curToken.value == value){
                eat(curToken.type);
            }
            else{
                throwSyntaxError("Expected " + value);
            }
        }

        inline void parseEndOfLine(){
            if(peek(ENDOFLINE)){
                eat(ENDOFLINE);
            }
            else{
                throwSyntaxError("Expected \';\'");
            }
        }

    //-------------------------------------------------------------------parse arithmetic expressions----------------------------------------------------

    Node* parseArithmeticSubExpr(){
        Node* root;
        if(curTokenValue("-")){
            root = new Node(curToken);
            eat(ARITHMETICOPERATOR);
            root->children.push_back(new Node(Token("0", NUMERICLITERAL, curToken.line)));
            root->children.push_back(parseArithmeticSubExpr());
        }
        else if(curTokenValue("(")){
            parseParenthesis("(");
            root = parseAddSubExpr();
            parseParenthesis(")");
        }
        else if(peek(NUMERICLITERAL) || peek(IDENTIFIER) || peek(STRINGLITERAL)) {
            root = new Node(curToken);
            eat(curToken.type);
        }
        else if(peek(KEYWORD)){
            root = parseBuiltin();
        }
        else if(!peek(ENDOFLINE)){
            throwSyntaxError("unexpected token " + curToken.value);
        }
        return root;
    }
    Node* parseMulDivModExpr(){
        Node* root = parseArithmeticSubExpr();

        while(curTokenValue("*") || curTokenValue("/") || curTokenValue("%")){
            Node* newRoot = new Node(curToken);
            eat(ARITHMETICOPERATOR);
            newRoot->children.push_back(root);
            newRoot->children.push_back(parseMulDivModExpr());
            root = newRoot;
        }
        return root;
    }
    Node* parseAddSubExpr(){
        Node* root = parseMulDivModExpr();

        while(curTokenValue("+") || curTokenValue("-")){
            Node* newRoot = new Node(curToken);
            eat(ARITHMETICOPERATOR);
            newRoot->children.push_back(root);
            newRoot->children.push_back(parseAddSubExpr());
            root = newRoot;
        }
        return root;
    }

    // -----------------------------------------------------------------parse boolean expression----------------------------------------------

    Node* parseBooleanCondition(){
        Node* root;
        Node* leftExpr = parseAddSubExpr();
        if(peek(RELATIONALOPERATOR)){
            root = new Node(curToken);
            eat(RELATIONALOPERATOR);
        }else{
            throwSyntaxError("Boolean Condition should have comparison operator");
        }
        Node* rightExpr = parseAddSubExpr();

        root->children.push_back(leftExpr);
        root->children.push_back(rightExpr);

        return root;
    }
    Node* parseBooleanSubExpr(){
        Node* root;
        if(curTokenValue("(")){
            eat(OPENPARENTHESIS);
            root = parseBooleanORExpr();
            parseParenthesis(")");
        }
        else{
            root = parseBooleanCondition();
        }
        return root;
    }
    Node* parseBooleanANDExpr(){
        Node* root = parseBooleanSubExpr();

        while(curTokenValue("and")){
            Node* newRoot = new Node(curToken);
            eat(KEYWORD);
            newRoot->children.push_back(root);
            newRoot->children.push_back(parseBooleanANDExpr());
            root = newRoot;
        }

        return root;
    }
    Node* parseBooleanORExpr(){
        Node* root = parseBooleanANDExpr();

        while(curTokenValue("or")){
            Node* newRoot = new Node(curToken);
            eat(KEYWORD);
            newRoot->children.push_back(root);
            newRoot->children.push_back(parseBooleanORExpr());
            root = newRoot;
        }
    
        return root;
    }

    // ------------------------------------------------------------------parse builtins-----------------------------------------------

    Node* parseInputFunction(){
        Node* root = new Node(Token("input", ACTIVITY, curToken.line));
        
        eat(KEYWORD);
        parseParenthesis("(");
        if(curTokenValue(")")){
            return root;
        }
        root->children.push_back(parseAddSubExpr());
        while(curTokenValue(",")){
            eat(SPECIALCHARACTER);
            root->children.push_back(parseAddSubExpr());
        }
        parseParenthesis(")");

        return root;
    }

    Node* parseIntFunction(){
        Node* root = new Node(Token("int", ACTIVITY, curToken.line));
        eat(KEYWORD);
        parseParenthesis("(");
        root->children.push_back(parseAddSubExpr());
        parseParenthesis(")");
        return root;
    }

    Node* parseStringFunction(){
        Node* root = new Node(Token("string", ACTIVITY, curToken.line));
        eat(KEYWORD);
        parseParenthesis("(");
        root->children.push_back(parseAddSubExpr());
        parseParenthesis(")");
        return root;
    }
    
    Node* parseLenFunction(){
        Node* root = new Node(Token("len", ACTIVITY, curToken.line));
        eat(KEYWORD);
        parseParenthesis("(");
        root->children.push_back(parseAddSubExpr());
        parseParenthesis(")");
        return root;
    }

    inline Node* parseBuiltin(){
        Node* root;

        if(curTokenValue("input")){
            root = parseInputFunction();
        }
        else if(curTokenValue("int")){
            root = parseIntFunction();
        }
        else if(curTokenValue("string")){
            root = parseStringFunction();
        }
        else if(curTokenValue("len")){
            root = parseLenFunction();
        }
        else{
            throwSyntaxError("invalid syntax");
        }

        return root;
    }

    //-------------------------------------------------------------------parse assignment statement-------------------------------------------

    Node* parseAssignmentStatement(){
        Node* root = new Node(Token("assignment", ACTIVITY, curToken.line));
        Node* identifierNode = new Node(curToken);
        eat(IDENTIFIER);
        if(peek(ASSIGNMENT)){
            eat(ASSIGNMENT);
            root->children.push_back(identifierNode);
            root->children.push_back(parseAddSubExpr());
            parseEndOfLine();
        }
        else{
            throwSyntaxError("invalid syntax");
        }
        return root;
    }

    // -----------------------------------------------------------------parse print statement-----------------------------------------------

    Node* parsePrintStatement(){
        Node* root = new Node(Token("print", ACTIVITY, curToken.line));
        eat(KEYWORD);
        parseParenthesis("(");
        if(curTokenValue(")")){
            return root;
        }
        root->children.push_back(parseAddSubExpr());
        while(curTokenValue(",")){
            eat(SPECIALCHARACTER);
            root->children.push_back(parseAddSubExpr());
        }
        parseParenthesis(")");
        parseEndOfLine();

        return root;
    }

    // ------------------------------------------------------------------if else if block--------------------------------------------------

    Node* parseIfElseIfStatement(){
        Node* root = new Node(Token("if", ACTIVITY, curToken.line));
        eat(KEYWORD);

        parseParenthesis("(");
        root->children.push_back(parseBooleanORExpr());
        parseParenthesis(")");

        parseParenthesis("{");
        root->children.push_back(parseStatementList(Token("}", CLOSEPARENTHESIS, curToken.line)));
        parseParenthesis("}");

        if(curTokenValue("else")){
            eat(KEYWORD);
            if(curTokenValue("if")){
                root->children.push_back(parseIfElseIfStatement());
            }
            else{
                parseParenthesis("{");
                root->children.push_back(parseStatementList(Token("}", CLOSEPARENTHESIS, curToken.line)));
                parseParenthesis("}");
            }
        }

        return root;
    }

    // ------------------------------------------------------------------parse while loop---------------------------------------------------
    Node* parseLoopControlStatemet(){
        Node* root = new Node(Token(curToken.value, ACTIVITY, curToken.line));
        eat(KEYWORD);
        parseEndOfLine();
        return root;
    }
    
    Node* parseWhileLoop(){
        Node* root = new Node(Token("while", ACTIVITY, curToken.line));
        
        eat(KEYWORD);
        parseParenthesis("(");
        root->children.push_back(parseBooleanORExpr());
        parseParenthesis(")");

        parseParenthesis("{");
        root->children.push_back(parseStatementList(Token("}", CLOSEPARENTHESIS, curToken.line)));
        parseParenthesis("}");

        return root;
    }

    //-------------------------------------------------------------------parse program-------------------------------------------------------
    Node* parseStatement(){
        Node* root = NULL;

        while(peek(ENDOFLINE)){
            eat(ENDOFLINE);
        }
        
        if(peek(IDENTIFIER)){
            root = parseAssignmentStatement();
        }
        else if(curTokenValue("print")){
            root = parsePrintStatement();
        }
        else if(curTokenValue("if")){
            root = parseIfElseIfStatement();
        }
        else if(curTokenValue("while")){
            root = parseWhileLoop();
        }
        else if(curTokenValue("break") || curTokenValue("continue")){
            root = parseLoopControlStatemet();
        }
        else if(!(peek(ENDOFFILE))){
            throwSyntaxError("invalid syntax");
        }
        return root;
    }
    Node* parseStatementList(Token endToken){
        Node* root = new Node();
        while(curToken != endToken){
            if(curToken != endToken){
                root->children.push_back(parseStatement());
            }
        }
        return root;
    }

    
    public:
        Parser(){
            lexer = Lexer();
        }

        /*
            sets the parser to parse the given input
        */
        Node* parse(std::string text){
            lexer.tokenize(text);

            curToken = lexer.getNextToken();

            return parseStatementList(Token("", ENDOFFILE, curToken.line));
        }
};

#endif // PARSER_CLASS