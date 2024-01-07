#include <iostream>
#include <string>
#include <regex>
#include <regexp.h>

int main(int argc, char *argv[]) {
    std::string regex = "^(b)*(b|ea)ac$";
    regexp::Automaton automaton(1);
    std::cout << "source regex: " << regex << std::endl << std::endl;
    std::string academicRegex = regexp::convertToAcademicRegex(regex, &automaton);
    std::cout << automaton.toString() << std::endl;
    std::cout << "result regex: " << academicRegex;
    return 0;
}

/*
    std::string str = "ab";
    std::cmatch result;
    std::regex regular("(a*)b");

    if (std::regex_match(str.c_str(), result, regular)) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
*/