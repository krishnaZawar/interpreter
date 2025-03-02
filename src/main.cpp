#include <iostream>
#include<fstream>
#include "interpreter/interpreter.cpp"
#include "errorClasses/errorClasses.h"

int main(int argc, char* argv[]){
    std::ifstream file (argv[1]);

    std::string input = "";
    std::string buffer = "";

    while(getline(file, buffer)){
        input += buffer + "\n";
    }
    
    Interpreter interpreter = Interpreter();
    try{
        interpreter.interpret(input);
    }
    catch(Error &e){
        std::cout<<e.getMessage()<<std::endl;
    }

    file.close();
    
    return 0;
}