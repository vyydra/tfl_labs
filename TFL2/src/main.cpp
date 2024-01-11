#include <iostream>
#include <string>
#include <regex>
#include <regexp.h>

int main(int argc, char *argv[]) {
    std::string regex = "^(d(?<=ab)(d)*c|a(?<=aa)cab)a(?<=d)b(de)*de$";
    regexp::Automaton automaton(1);
    std::cout << "source regex: " << regex << std::endl << std::endl;
    std::string academicRegex = regexp::convertToAcademicRegex(regex, &automaton);
    std::cout << automaton.toString() << std::endl;
    std::cout << "result regex: " << academicRegex;
    return 0;
}