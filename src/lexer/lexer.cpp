#include <string>
#include<vector>
#include<unordered_map>
#include "tokenType.h"
#include "token.h"
#include "../errorClasses/errorClasses.h"

#ifndef LEXER_CLASS
#define LEXER_CLASS

class Lexer{
    private:
        int pointer;
        int curLine;
        std::string text;

        std::vector<std::string> keywords;

        std::vector<char> arithmeticOperators;

        std::vector<char> openParenthesis;
        std::vector<char> closeParenthesis;
        std::vector<char> specialCharacters;

        std::vector<char> skippables;
        
        std::unordered_map<char, char> escapeSequenceMap;

        inline void throwTokenError(std::string message){
            throw TokenError(message, curLine);
        }
        
        /*
            checks if the given character is a digit
        */ 
        inline bool isDigit(){
            return text[pointer] >= '0' && text[pointer] <= '9';
        }

        /*
            checks if the given character is an alphabet
        */ 
        inline bool isAlpha(){
            return (text[pointer] >= 'a' && text[pointer] <= 'z') || (text[pointer] >= 'A' && text[pointer] <= 'Z');
        }
        
        /*
            checks if a value(of type t) exists in a list<t>
        */
        template<class t>
        inline bool exists(const t &data, const std::vector<t> &list){
            for(auto item : list){
                if(data == item){
                    return true;
                }
            }
            return false;
        }

        
    public:

        Lexer(){
            pointer = 0;
            curLine = 1;
            text = "";

            keywords = {
                "print",
                "and",
                "or",
                "if",
                "else",
                "input",
                "while",
                "break",
                "continue",
                "int",
                "string",
                "len"
            };

            arithmeticOperators = {
                '+', '-', '/', '*', '%' 
            };

            openParenthesis = {
                '(', '{', '['
            };
            closeParenthesis ={
                ')', '}', ']'
            };

            specialCharacters = {
                ','
            };

            skippables = {
                '\n', ' ', '\t'
            };

            //build escape sequence map
            escapeSequenceMap['n'] = '\n';
            escapeSequenceMap['t'] = '\t';
            escapeSequenceMap['\\'] = '\\';
            escapeSequenceMap['\''] = '\'';
            escapeSequenceMap['\"'] = '\"';
        }
        
        /*
            set the lexer to tokenize the text from start
        */ 
        void tokenize(std::string _text){
            text = _text;
            pointer = 0;
            curLine = 1;
        }

        /*
            return the next token in the input
        */ 
        Token getNextToken(){
            Token curToken;

            while(exists(text[pointer], skippables)){
                if(text[pointer] == '\n'){
                    curLine++;
                }
                pointer++;
            }

            if(pointer >= text.length()){
                curToken = Token("", ENDOFFILE, curLine);
            }

            else if(text[pointer] == ';'){
                pointer++;
                curToken = Token(";", ENDOFLINE, curLine);
            }

            else if(text[pointer] == '\"'){
                pointer++;
                std::string value = "";
                // buffer indicates if there is a backslash encountered or not
                bool buffer = false;
                while(pointer < text.length() && (buffer || text[pointer] != '\"')){
                    if(buffer){
                        if(escapeSequenceMap.find(text[pointer]) != escapeSequenceMap.end()){
                            value += escapeSequenceMap[text[pointer]];
                            buffer = false;
                        }
                        else{
                            std::string message = "Unrecognised escape sequence \\";
                            message += text[pointer];
                            throwTokenError(message);
                        }
                    }
                    else{
                        if(text[pointer] == '\\'){
                            buffer = true;
                        }
                        else{
                            value += text[pointer];
                        }
                    }
                    pointer++;
                }
                //indicating the string literal was not terminated properly
                if(buffer || (pointer >= text.length() && text[pointer - 1] != '\"')){
                    throwTokenError("Expected \"");
                }
                pointer++;
                curToken = Token(value, STRINGLITERAL, curLine);
            }

            else if(exists(text[pointer], specialCharacters)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, SPECIALCHARACTER, curLine);
            }

            else if(exists(text[pointer], openParenthesis)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, OPENPARENTHESIS, curLine);
            }
            else if(exists(text[pointer], closeParenthesis)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, CLOSEPARENTHESIS, curLine);
            }

            else if(exists(text[pointer], arithmeticOperators)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, ARITHMETICOPERATOR, curLine);
            }

            else if(text[pointer] == '='){
                pointer++;
                if(pointer < text.length() && text[pointer] == '='){
                    pointer++;
                    curToken = Token("==", RELATIONALOPERATOR, curLine);
                }
                else{
                    curToken = Token("=", ASSIGNMENT, curLine);
                }
            }
            else if(text[pointer] == '>' || text[pointer] == '<'){
                std::string value = "";
                value += text[pointer];
                pointer++;
                if(pointer < text.length() && text[pointer] == '='){
                    value += text[pointer];
                    pointer++;
                }
                curToken = Token(value, RELATIONALOPERATOR, curLine);
            }
            else if(text[pointer] == '!'){
                std::string value = "";
                value += text[pointer];
                pointer++;
                if(pointer < text.length() && text[pointer] != '='){
                    throwTokenError("Unrecognised token \'!\'");
                }
                pointer++;
                curToken = Token("!=", RELATIONALOPERATOR, curLine);
            }

            else if(isDigit()){
                std::string value = "";
                while(pointer < text.length() && isDigit()){
                    value += text[pointer];
                    pointer++;
                }
                curToken = Token(value, NUMERICLITERAL, curLine);
            }

            else if(isAlpha()){
                std::string value = "";
                while(pointer < text.length() && (isAlpha() || isDigit())){
                    value += text[pointer];
                    pointer++;
                }

                if(exists(value, keywords)){
                    curToken = Token(value, KEYWORD, curLine);
                }
                else{
                    curToken = Token(value, IDENTIFIER, curLine);
                }
            }
            else{
                std::string message = "Unrecognised token ";
                message += text[pointer];
                throwTokenError(message);
            }
            return curToken;
        }
};

#endif  // LEXER_CLASS