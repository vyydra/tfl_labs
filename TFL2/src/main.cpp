#include <iostream>
#include <string>
#include <regex>
#include <regexp.h>

int main(int argc, char *argv[]) {
    std::string regex = "^(?=a$)b(?=a$)a(?=a$)a|c$";
    regexp::Automaton automaton(1);
    std::cout << "source regex: " << regex << std::endl << std::endl;
    std::string academicRegex = regexp::convertToAcademicRegex(regex, &automaton);
    std::cout << automaton.toString() << std::endl;
    std::cout << "result regex: " << academicRegex << std::endl;
    std::cout << "generated words:" << std::endl;
    std::set<std::string> words = regexp::generateWords(automaton, 50);
    for (const auto& word : words) {
        std::cout << word << std::endl;
    }
    regexp::checkWordsMatch(words, regex, academicRegex);
    return 0;
}