#include <iostream>
#include <string>
#include <regex>
#include <regexp.h>

int main(int argc, char *argv[]) {
    regexp::testRegularExpressions("output.txt", 200, 125);
    
    return 0;
}