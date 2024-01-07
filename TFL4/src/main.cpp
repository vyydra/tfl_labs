#include <iostream>
#include <string>
#include <regex>
#include <slr1.h>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
    std::vector<slr1::ProductionRule> rules;

    std::ifstream file("input.txt");

    std::string s;

    while(std::getline(file, s)) {
        std::stringstream ss(s);  
        std::string leftPart;
        std::string str;
        std::string rightPart;
        ss >> leftPart;
        ss >> str;
        ss >> rightPart;
        rules.push_back(slr1::ProductionRule(leftPart, rightPart));
    }

    file.close();

    slr1::Grammar grammar;

    std::set<char> nonterminals;

    for (auto rule : rules) {
        nonterminals.insert(rule.getLeftPart()[0]);
    }

    std::set<char> terminals;
    for (auto rule : rules) {
        for (auto symbol : rule.getRightPart()) {
            if (!nonterminals.count(symbol) && symbol != 'E') {
                terminals.insert(symbol);
            }
        }
    }

    // E - EPSILON
    grammar.setTerminals(terminals);
    grammar.setNonterminals(nonterminals);
    grammar.setStartSymbol('S');
    grammar.setProductionRules(rules);

    slr1::parse(grammar, "word");
    
    return 0;
}