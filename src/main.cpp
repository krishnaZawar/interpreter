#include <iostream>
#include<fstream>
#include "interpreter/interpreter.cpp"

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
    catch(std::string &e){
        std::cout<<e<<std::endl;
    }

    file.close();
    
    return 0;
}