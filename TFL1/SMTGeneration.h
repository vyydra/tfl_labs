#ifndef FLT1_SMTGENERATION_H
#define FLT1_SMTGENERATION_H

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include "LinearFunction.h"
#include "InequalitiesGeneration.h"
#include "LinearFunctionsGeneration.h"

std::pair<std::string, std::string> parseInput(const std::string& input) {
    size_t arrow_pos = input.find("->");
    std::string lhs = input.substr(0, arrow_pos);
    std::string rhs = input.substr(arrow_pos + 2);

    lhs = lhs.substr(lhs.find_first_not_of(' '));
    lhs = lhs.substr(0, lhs.find_last_not_of(' ') + 1);

    rhs = rhs.substr(rhs.find_first_not_of(' '));
    rhs = rhs.substr(0, rhs.find_last_not_of(' ') + 1);

    return { lhs, rhs };
}

void generateRequirements(std::ofstream& smtFile, std::vector<char>& symbols, const std::pair<std::string, std::string>& sides, const std::map<std::pair<int, bool>, std::string>& lhs, const std::map<std::pair<int, bool>, std::string>& rhs) {

    for (auto symbol : sides.first) {
        if (std::find(symbols.begin(), symbols.end(), symbol) == symbols.end()) {
            smtFile << "(declare-fun a_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun b_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun c_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun d_" << symbol << " () Int)" << std::endl;
            smtFile << "(assert (> a_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> b_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> c_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> d_" << symbol << " 0))" << std::endl;
            symbols.emplace_back(symbol);
        }
    }

    for (auto symbol : sides.second) {
        if (std::find(symbols.begin(), symbols.end(), symbol) == symbols.end()) {
            smtFile << "(declare-fun a_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun b_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun c_" << symbol << " () Int)" << std::endl;
            smtFile << "(declare-fun d_" << symbol << " () Int)" << std::endl;
            smtFile << "(assert (> a_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> b_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> c_" << symbol << " 0))" << std::endl;
            smtFile << "(assert (> d_" << symbol << " 0))" << std::endl;
            symbols.emplace_back(symbol);
        }
    }

    std::vector<std::string> inequalities = generateInequalities(lhs, rhs);

    for (const auto& inequality : inequalities) {
        smtFile << "(assert " << inequality << ")" << std::endl;
    }

}

bool executeSMTSolver(const std::string& smtFile, std::string& output) {
    std::stringstream command;
    command << R"(C:\z3-4.12.2-x64-win\bin\z3.exe -smt2 )" << smtFile;

    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[128];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr) {
            output += buffer;
        }
    }

    pclose(pipe);

    return true;
}

void generateSMT() {
    std::fstream testFile;
    std::ofstream smtFile;
    std::vector<char> symbols = {};
    testFile.open("test.txt", std::ios::in);
    smtFile.open("inequalities.smt2");
    if (smtFile.is_open()) {
        smtFile << "(set-logic QF_NIA)" << std::endl;
        if (testFile.is_open()) {
            std::string line;
            while (std::getline(testFile, line)) {
                std::pair<std::string, std::string> sides = parseInput(line);
                std::pair<Node*, Node*>* functions = generateLinearFunctions(sides.first, sides.second);
                std::cout << functions->first->to_string() << std::endl;
                std::cout << functions->second->to_string() << std::endl;
                std::map<std::pair<int, bool>, std::string> lhs = extractCoefficients(functions->first);
                std::map<std::pair<int, bool>, std::string> rhs = extractCoefficients(functions->second);
                generateRequirements(smtFile, symbols, sides, lhs, rhs);
            }
        }
        testFile.close();
        smtFile << "(check-sat)" << std::endl;
        smtFile << "(get-model)" << std::endl;
    }
    smtFile.close();

    std::string solverOutput;
    if (executeSMTSolver("inequalities.smt2", solverOutput)) {
        if (solverOutput.find("unsat") != std::string::npos) {
            std::cout << "The inequalities are unsatisfiable (unsat)." << std::endl;
        } else if (solverOutput.find("sat") != std::string::npos) {
            std::cout << "The inequalities are satisfiable (sat)." << std::endl;

            std::istringstream iss(solverOutput);
            std::string line;
            while (std::getline(iss, line)) {
                std::cout << line << std::endl;
            }
        } else {
            std::cout << "Unable to determine the result." << std::endl;
        }
    } else {
        std::cout << "Failed to execute the Z3 solver." << std::endl;
    }
}

#endif //FLT1_SMTGENERATION_H
