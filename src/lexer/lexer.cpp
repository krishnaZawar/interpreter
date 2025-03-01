#include <string>
#include<vector>
#include<unordered_map>
#include "tokenType.h"
#include "token.h"

#ifndef LEXER_CLASS
#define LEXER_CLASS

class Lexer{
    private:
        int pointer;
        std::string text;

        std::vector<std::string> keywords;

        std::vector<char> arithmeticOperators;

        std::vector<char> openParenthesis;
        std::vector<char> closeParenthesis;
        std::vector<char> specialCharacters;
        
        std::unordered_map<char, char> escapeSequenceMap;

        void error(){
            throw std::string("unrecognized token");
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
        bool exists(const t &data, const std::vector<t> &list){
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
            text = "";

            keywords = {
                "print",
                "and",
                "or",
                "if",
                "else",
                "input",
                "while"
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
        }

        /*
            return the next token in the input
        */ 
        Token getNextToken(){
            Token curToken;

            while(text[pointer] == ' ' || text[pointer] == '\t' || text[pointer] == '\n'){
                pointer++;
            }

            if(pointer >= text.length()){
                curToken = Token("", ENDOFFILE);
            }

            else if(text[pointer] == ';'){
                pointer++;
                curToken = Token(";", ENDOFLINE);
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
                            error();
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
                if(buffer){
                    error();
                }
                pointer++;
                curToken = Token(value, STRINGLITERAL);
            }

            else if(exists(text[pointer], specialCharacters)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, SPECIALCHARACTER);
            }

            else if(exists(text[pointer], openParenthesis)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, OPENPARENTHESIS);
            }
            else if(exists(text[pointer], closeParenthesis)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, CLOSEPARENTHESIS);
            }

            else if(exists(text[pointer], arithmeticOperators)){
                std::string value = "";
                value += text[pointer];
                pointer++;
                curToken = Token(value, ARITHMETICOPERATOR);
            }

            else if(text[pointer] == '='){
                pointer++;
                if(pointer < text.length() && text[pointer] == '='){
                    pointer++;
                    curToken = Token("==", RELATIONALOPERATOR);
                }
                else{
                    curToken = Token("=", ASSIGNMENT);
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
                curToken = Token(value, RELATIONALOPERATOR);
            }
            else if(text[pointer] == '!'){
                std::string value = "";
                value += text[pointer];
                pointer++;
                if(pointer < text.length() && text[pointer] != '='){
                    error();
                }
                pointer++;
                curToken = Token("!=", RELATIONALOPERATOR);
            }

            else if(isDigit()){
                std::string value = "";
                while(pointer < text.length() && isDigit()){
                    value += text[pointer];
                    pointer++;
                }
                curToken = Token(value, NUMERICLITERAL);
            }

            else if(isAlpha()){
                std::string value = "";
                while(pointer < text.length() && (isAlpha() || isDigit())){
                    value += text[pointer];
                    pointer++;
                }

                if(exists(value, keywords)){
                    curToken = Token(value, KEYWORD);
                }
                else{
                    curToken = Token(value, IDENTIFIER);
                }
            }
            else{
                error();
            }
            return curToken;
        }
};

#endif  // LEXER_CLASS