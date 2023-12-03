#include <iostream>
#include <string>
#include <regex>
#include <regexp.h>

int main(int argc, char *argv[]) {
    std::string regex = "^(?=a)a(b|c)$";
    std::string academicRegex = regexp::convertToAcademicRegex(regex);
    std::cout << academicRegex;
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