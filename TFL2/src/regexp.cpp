#include <regexp.h>
#include <string>
#include <regex>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <random>
#include <chrono>
#include <boost/regex.hpp>

using namespace regexp;

#define ALPHABET_SIZE 5
#define MAX_LETTERS 10
#define STAR_HEIGHT 3
#define NUM_LOOKAHEADS 3
#define NUM_LOOKBEHINDS 3

Automaton::Automaton(int statesNumber) {
    this->statesNumber = statesNumber;
    initialStateMatrix = Matrix<int>(statesNumber, 1);
    initialStateMatrix.setAll(0);
    transitionMatrix = Matrix<std::string>(statesNumber, statesNumber);
    transitionMatrix.setAll("0");
    finalStateMatrix = Matrix<int>(statesNumber, 1);
    finalStateMatrix.setAll(0);
}

regexp::Matrix<int>* Automaton::getInitialStateMatrix() {
    return &initialStateMatrix;
}

regexp::Matrix<std::string>* Automaton::getTransitionMatrix() {
    return &transitionMatrix;
}

regexp::Matrix<int>* Automaton::getFinalStateMatrix() {
    return &finalStateMatrix;
}

int Automaton::getStatesNumber() {
    return statesNumber;
}

std::string Automaton::toString() {
    std::stringstream ss;
    ss << "digraph automaton {\n";
    ss << "\trankdir=LR\n";
    ss << "\tnode [shape = doublecircle]";

    bool exist = false;
    for (int i = 0; i < finalStateMatrix.getN(); i++) {
        if (finalStateMatrix.get(i, 0) == 1) {
            if (!exist) {
                ss << "; " << i;
                exist = true;
            } else {
                ss << " " << i;
            }
        }
    }
    ss << ";\n";

    ss << "\tnode [shape = circle];\n";

    for (int i = 0; i < initialStateMatrix.getN(); i++) {
        if (initialStateMatrix.get(i, 0) == 1) {
            ss << "\t" << i << " [color = blue];\n";
        }
    }

    for (int i = 0; i < transitionMatrix.getN(); i++) {
        for (int j = 0; j < transitionMatrix.getM(); j++) {
            if (transitionMatrix.get(i, j) != "0") {
                ss << "\t" << i << " -> " << j << " [label = \"" << transitionMatrix.get(i, j) << "\"];\n";
            }
        }
    }

    ss << "}\n";

    return ss.str();
}
    
regexp::Automaton Automaton::getUnion(Automaton& a1, Automaton& a2) {
    Automaton automaton(a1.getStatesNumber() + a2.getStatesNumber() - 1);

    automaton.getInitialStateMatrix()->set(0, 0, 1);

    automaton.getFinalStateMatrix()->set(0, 0, a1.getFinalStateMatrix()->get(0, 0) + a2.getFinalStateMatrix()->get(0, 0) > 0 ? 1 : 0);

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        automaton.getFinalStateMatrix()->set(i, 0, a1.getFinalStateMatrix()->get(i, 0));
    }

    for (int i = 1; i < a2.getStatesNumber(); i++) {
        automaton.getFinalStateMatrix()->set(i + a1.getStatesNumber() - 1, 0, a2.getFinalStateMatrix()->get(i, 0));
    }

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        automaton.getTransitionMatrix()->set(0, i, a1.getTransitionMatrix()->get(0, i));
    }
    for (int i = 1; i < a2.getStatesNumber(); i++) {
        automaton.getTransitionMatrix()->set(0, i + a1.getStatesNumber() - 1, a2.getTransitionMatrix()->get(0, i));
    }

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        for (int j = 1; j < a1.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i, j, a1.getTransitionMatrix()->get(i, j));
        }
    }
    for (int i = 1; i < a2.getStatesNumber(); i++) {
        for (int j = 1; j < a2.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i + a1.getStatesNumber() - 1, j + a1.getStatesNumber() - 1, a2.getTransitionMatrix()->get(i, j));
        }
    }

    return automaton;
}

regexp::Automaton Automaton::getConcatenation(Automaton& a1, Automaton& a2) {
    Automaton automaton(a1.getStatesNumber() + a2.getStatesNumber() - 1);

    automaton.getInitialStateMatrix()->set(0, 0, 1);

    for (int i = 0; i < a1.getStatesNumber(); i++) {
        automaton.getFinalStateMatrix()->set(i, 0, a1.getFinalStateMatrix()->get(i, 0) + a2.getFinalStateMatrix()->get(0, 0) == 2 ? 1 : 0);
    }

    for (int i = 1; i < a2.getStatesNumber(); i++) {
        automaton.getFinalStateMatrix()->set(i + a1.getStatesNumber() - 1, 0, a2.getFinalStateMatrix()->get(i, 0));
    }

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        automaton.getTransitionMatrix()->set(0, i, a1.getTransitionMatrix()->get(0, i));
    }

    for (int i = 1; i < a2.getStatesNumber(); i++) {
        if (a1.getFinalStateMatrix()->get(0, 0) != 0) {
            automaton.getTransitionMatrix()->set(0, i + a1.getStatesNumber() - 1, a2.getTransitionMatrix()->get(0, i));
        }
    }

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        for (int j = 1; j < a1.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i, j, a1.getTransitionMatrix()->get(i, j));
        }
    }
    for (int i = 1; i < a2.getStatesNumber(); i++) {
        for (int j = 1; j < a2.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i + a1.getStatesNumber() - 1, j + a1.getStatesNumber() - 1, a2.getTransitionMatrix()->get(i, j));
        }
    }

    for (int i = 1; i < a1.getStatesNumber(); i++) {
        if (a1.getFinalStateMatrix()->get(i, 0) != 0) {
            for (int j = 1; j < a2.getStatesNumber(); j++) {
                automaton.getTransitionMatrix()->set(i, j + a1.getStatesNumber() - 1, a2.getTransitionMatrix()->get(0, j));
            }
        }
    }

    return automaton;
}

regexp::Automaton Automaton::getKleeneClosure(Automaton& a) {
    Automaton automaton(a.getStatesNumber());

    automaton.getInitialStateMatrix()->set(0, 0, 1);

    automaton.getFinalStateMatrix()->set(0, 0, 1);

    for (int i = 1; i < a.getStatesNumber(); i++) {
        automaton.getFinalStateMatrix()->set(i, 0, a.getFinalStateMatrix()->get(i, 0));
    }

    for (int i = 1; i < a.getStatesNumber(); i++) {
        automaton.getTransitionMatrix()->set(0, i, a.getTransitionMatrix()->get(0, i));
    }

    for (int i = 1; i < automaton.getStatesNumber(); i++) {
        for (int j = 1; j < automaton.getStatesNumber(); j++) {
            std::string hij = "0";
            std::string jj = a.getTransitionMatrix()->get(0, j);
            int ui = a.getFinalStateMatrix()->get(i, 0);
            std::string fij = a.getTransitionMatrix()->get(i, j);

            if (ui == 0) {
                hij = fij;
            } else if (fij == "0") {
                hij = jj;
            } else if (fij != "0" && jj == "0") {
                hij = fij;
            } else if (jj != "0") {
                hij = "(" + jj + "|" + fij + ")";
            }

            automaton.getTransitionMatrix()->set(i, j, hij);
        }
    }

    return automaton;
}

regexp::Automaton Automaton::getIntersection(Automaton& a1, Automaton& a2) {
    Automaton automaton(a1.getStatesNumber() * a2.getStatesNumber());

    automaton.getInitialStateMatrix()->set(0, 0, 1);

    for (int i = 0; i < a1.getStatesNumber(); i++) {
        for (int j = 0; j < a2.getStatesNumber(); j++) {
            if (a1.getFinalStateMatrix()->get(i, 0) + a2.getFinalStateMatrix()->get(j, 0) == 2) {
                automaton.getFinalStateMatrix()->set(i * a2.getStatesNumber() + j, 0, 1);
            }
        }
    }

    for (int i = 0; i < a1.getStatesNumber(); i++) {
        for (int j = 0; j < a2.getStatesNumber(); j++) {
            for (int k = 0; k < a1.getStatesNumber(); k++) {
                for (int h = 0; h < a2.getStatesNumber(); h++) {
                    if (a1.getTransitionMatrix()->get(i, k) != "0") {
                        if (a1.getTransitionMatrix()->get(i, k) == a2.getTransitionMatrix()->get(j, h)) {
                            automaton.getTransitionMatrix()->set(i * a2.getStatesNumber() + j, k * a2.getStatesNumber() + h, a1.getTransitionMatrix()->get(i, k));
                        }
                    }
                }
            }
        }
    }

    Matrix<bool> visitedMatrix1(automaton.getStatesNumber(), 1);
    Matrix<bool> visitedMatrix2(automaton.getStatesNumber(), 1);
    visitedMatrix1.setAll(false);
    visitedMatrix2.setAll(false);
    std::set<int> badVertices;
    std::vector<int> dVertices;
    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (automaton.getFinalStateMatrix()->get(i, 0) == 1) {
            dVertices.push_back(i);
        }
    }

    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (badVertices.find(i) != badVertices.end()) {
            continue;
        }
        std::vector<int> dVertice;
        dVertice.push_back(i);

        if (!(dfs(automaton.getTransitionMatrix(), &visitedMatrix1, i, &dVertices) && dfs(automaton.getTransitionMatrix(), &visitedMatrix2, 0, &dVertice))) {
            badVertices.insert(i);
        }
        
        visitedMatrix1.setAll(false);
        visitedMatrix2.setAll(false);
    }

    if (badVertices.size() == automaton.getStatesNumber()) {
        Automaton a(1);
        a.getInitialStateMatrix()->set(0, 0, 1);
        return Automaton(1);
    }

    Automaton a(automaton.getStatesNumber() - badVertices.size());

    a.getInitialStateMatrix()->set(0, 0, 1);
    int index = 0;
    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (badVertices.find(i) == badVertices.end()) {
            a.getFinalStateMatrix()->set(index, 0, automaton.getFinalStateMatrix()->get(i, 0));
            int index2 = 0;
            for (int j = 0; j < automaton.getStatesNumber(); j++) {
                if (badVertices.find(j) == badVertices.end()) {
                    a.getTransitionMatrix()->set(index2, index, automaton.getTransitionMatrix()->get(j, i));
                    index2++;
                }
            }
            index++;
        }
    }

    return a;
}

std::vector<Lexer::Token> Lexer::tokenize(std::string source, std::set<char> alphabet) {
    std::vector<Token> vector;
    
    for (size_t i = 0; i < source.length(); i++) {
        if (i > 0 && (alphabet.find(source[i - 1]) != alphabet.end() || source[i - 1] == ')') &&
                        (alphabet.find(source[i]) != alphabet.end() || source[i] == '(')) {
            vector.push_back(Token("_", Token::kConcatenation));
        }

        if (alphabet.find(source[i]) != alphabet.end()) {
            vector.push_back(Token(std::string(1, source[i]), Token::kLetter));
            continue;
        }

        switch (source[i]) {
            case '(':
                vector.push_back(Token(std::string(1, source[i]), Token::kOpenParenthesis));
                break;
            case ')':
                vector.push_back(Token(std::string(1, source[i]), Token::kClosedParenthesis));
                break;
            case '*':
                vector.push_back(Token(std::string(1, source[i]), Token::kKleeneStar));
                if (i < source.length() - 1 && source[i + 1] != ')' && source[i + 1] != '|' && source[i + 1] != '&') {
                    vector.push_back(Token("+", Token::kConcatenation));
                }
                break;
            case '&':
                vector.push_back(Token(std::string(1, source[i]), Token::kIntersection));
                break;
            case '|':
                vector.push_back(Token(std::string(1, source[i]), Token::kAlternative));
                break;
            default:
                vector.push_back(Token(std::string(1, source[i]), Token::kNone));
                break;
        }
    }

    return vector;
}

Lexer::Token::Token(std::string data, int type) {
    this->data = data;
    this->type = type;
}

std::string Lexer::Token::getData() {
    return data;
}

int Lexer::Token::getType() {
    return type;
}

bool Lexer::Token::isOperator() {
    return (type & (Token::kKleeneStar | Token::kConcatenation | Token::kIntersection | Token::kAlternative)) != 0;
}

bool Lexer::Token::hasHigherPriority(Token token) {
    return type < token.getType();
}

void regexp::replaceAll(std::string& source, std::string const& toReplace, std::string const& replaceWith) {
    std::string buffer;
    std::size_t position = 0;
    std::size_t previousPosition;
    buffer.reserve(source.size());
    while (true) {
        previousPosition = position;
        position = source.find(toReplace, position);
        if (position == std::string::npos) {
            break;
        }
        buffer.append(source, previousPosition, position - previousPosition);
        buffer += replaceWith;
        position += toReplace.size();
    }
    buffer.append(source, previousPosition, source.size() - previousPosition);
    source.swap(buffer);
}

void regexp::replaceDots(std::string& source, std::set<char> alphabet) {
    bool is_more = false;
    std::stringstream ss;
    ss << '(';
    for (std::set<char>::iterator it = alphabet.begin(); it != alphabet.end(); it++) {
        if (!is_more) {
            ss << *it;
        } else {
            ss << '|' << *it;
        }
        is_more = true;
    }
    ss << ')';
    replaceAll(source, ".", ss.str());
}

std::set<char> regexp::getAlphabetOf(std::string source) {
    std::set<char> alphabet;
    for (size_t i = 0; i < source.length(); i++)
    {
        if (source[i] != '$' && source[i] != '^' && source[i] != '*' && source[i] != '.' && source[i] != '|' &&
            source[i] != '?' && source[i] != '=' && source[i] != '(' && source[i] != ')' && source[i] != '<')
        {
            alphabet.insert(source[i]);
        }
    }

    return alphabet;
}

std::string regexp::convertLookaheadsToIntersections(std::string& source, int start, int end) {

    if (start > end) {
        return "";
    }

    int n = 0;
    
    for (int i = start; i <= end; i++) {
        if (source[i] == '(') {
            n++;
        } else if (source[i] == ')') {
            n--;
        } else if (n == 0 && source[i] == '|') {
            return "(" + convertLookaheadsToIntersections(source, start, i - 1) + "|" + convertLookaheadsToIntersections(source, i + 1, end) + ")";
        }
    }

    std::string result = "(";
    std::string r2 = "";
    n = 1;
    int i = start;
    bool existEndSymbol = false;
    while (i <= end) {
        if (i <= end - 2 && source[i + 1] == '?' && source[i + 2] == '=') {
            i += 3;
            int j = i;
            while (n > 0) {
                if (source[j] == '(') {
                    n++;
                }
                else if (source[j] == ')') {
                    n--;
                }
                if (n == 0 && source[j - 1] == '$') {
                    existEndSymbol = true;
                } 
                j++;
            }
            r2 = convertLookaheadsToIntersections(source, i, j - 2);
            i = j;
            break;
        }

        result += source[i];
        i++;
    }

    if (!r2.empty()) {
        result += "(" + r2;
        if (!existEndSymbol) {
            result += ".*";
        }

        result += "&" + convertLookaheadsToIntersections(source, i, end) + ")";
        
    }

    result += ")";

    return result;
}

std::string regexp::convertLookaheadsToIntersections(std::string& source) {
    return convertLookaheadsToIntersections(source, 0, source.length() - 1);
}

void regexp::removeFrom(std::string& source, std::string const& toRemove) {
    size_t start = source.find(toRemove);
    while (start != std::string::npos) {
        source.erase(start, toRemove.length()); 
        start = source.find(toRemove, start + toRemove.length());
    }
}

std::vector<Lexer::Token> regexp::convertToPostfixForm(std::vector<Lexer::Token>& tokens) {
    std::stack<Lexer::Token> stack;
    std::vector<Lexer::Token> vector;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].getType() == Lexer::Token::kLetter) {
            vector.push_back(tokens[i]);
            continue;
        }

        if (tokens[i].isOperator()) {
            if (stack.empty() || stack.top().getType() == Lexer::Token::kOpenParenthesis) {
                stack.push(tokens[i]);
            } else if (tokens[i].hasHigherPriority(stack.top())) {
                stack.push(tokens[i]);
            } else {
                while (!tokens[i].hasHigherPriority(stack.top()) && stack.top().getType() != Lexer::Token::kOpenParenthesis) {
                    vector.push_back(stack.top());
                    stack.pop();
                }
                stack.push(tokens[i]);
            }
            continue;
        }

        if (tokens[i].getType() == Lexer::Token::kOpenParenthesis) {
            stack.push(tokens[i]);
            continue;
        }

        if (tokens[i].getType() == Lexer::Token::kClosedParenthesis) {
            while (stack.top().getType() != Lexer::Token::kOpenParenthesis) {
                vector.push_back(stack.top());
                stack.pop();
            }
            stack.pop();
            continue;
        }
    }

    while (!stack.empty()) {
        vector.push_back(stack.top());
        stack.pop();
    }

    return vector;
}

Automaton regexp::buildGlushkovAutomaton(std::string symbol) {
    /*
    first: symbol
    last: symbol
    follow: null
    S (initial) -> symbol (final)
    automaton: (1 0) (0 symbol \n 0 0) (0 1)
    */
    Automaton automaton(2);
    automaton.getInitialStateMatrix()->set(0, 0, 1);
    automaton.getTransitionMatrix()->set(0, 1, symbol);
    automaton.getFinalStateMatrix()->set(1, 0, 1);

    return automaton;
}

std::string regexp::convertAutomatonToRegex(Automaton automaton) {
    std::string regex = "";

    bool isEmpty = true;
    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (automaton.getFinalStateMatrix()->get(i, 0) == 1) {
            isEmpty = false;
            break;
        }
    }

    if (isEmpty) {
        return "()";
    }

    for (int i = 1; i < automaton.getStatesNumber(); i++) {
        if (automaton.getFinalStateMatrix()->get(i, 0) == 1) {
            continue;
        }
        std::string data1 = automaton.getTransitionMatrix()->get(0, i);
        automaton.getTransitionMatrix()->set(0, i, "0");
        std::string data3 = automaton.getTransitionMatrix()->get(i, i);
        automaton.getTransitionMatrix()->set(i, i, "0");
        for (int j = 0; j < automaton.getStatesNumber(); j++) {
            if (j == i) {
                continue;
            }
            std::string data2 = automaton.getTransitionMatrix()->get(i, j);
            if (data1 != "0" && data2 != "0") {
                std::string data4 = "";
                if (automaton.getTransitionMatrix()->get(0, j) != "0") {
                    data4 = "((" + automaton.getTransitionMatrix()->get(0, j) + ")|(";
                }
                if (data3 != "0") {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + data1 + "(" + data3 + ")*" + data2 + "))");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, data1 + "(" + data3 + ")*" + data2);
                    }
                } else {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + data1 + data2 + "))");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, data1 + data2);
                    }
                }
            }

            std::string data5 = automaton.getTransitionMatrix()->get(j, i);
            if (data5 != "0") {
                for (int k = 0; k < automaton.getStatesNumber(); k++) {
                    if (k == i) {
                        continue;
                    }
                    std::string data6 = automaton.getTransitionMatrix()->get(i, k);
                    if (data6 != "0") {
                        std::string data7 = automaton.getTransitionMatrix()->get(j, k);
                        if (data3 != "0") {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data7 + ")|(" + data5 + "(" + data3 + ")*" + data6 + "))");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, data5 + "(" + data3 + ")*" + data6);
                            }
                        } else {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data7 + ")|(" + data5 + data6 + "))");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, data5 + data6);
                            }
                        }
                    }
                }
                automaton.getTransitionMatrix()->set(j, i, "0");
            }
        }
        for (int j = 0; j < automaton.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i, j, "0");
        }
        automaton.getTransitionMatrix()->set(0, i, "0");
    }

    for (int i = 1; i < automaton.getStatesNumber(); i++) {
        if (automaton.getFinalStateMatrix()->get(i, 0) == 0) {
            continue;
        }
        bool isGoodVertex = false;
        for (int j = 1; j < automaton.getStatesNumber(); j++) {
            if (i == j) {
                continue;
            }
            if (automaton.getFinalStateMatrix()->get(j, 0) == 0) {
                continue;
            }
            if (automaton.getTransitionMatrix()->get(i, j) != "0") {
                isGoodVertex = true;
                break;
            }
        }
        if (!isGoodVertex) {
            continue;
        }
        std::string data1 = automaton.getTransitionMatrix()->get(0, i);
        automaton.getTransitionMatrix()->set(0, i, "0");
        std::string data3 = automaton.getTransitionMatrix()->get(i, i);
        automaton.getTransitionMatrix()->set(i, i, "0");
        for (int j = 0; j < automaton.getStatesNumber(); j++) {
            if (j == i || automaton.getFinalStateMatrix()->get(j, 0) == 0) {
                continue;
            }
            std::string data2 = automaton.getTransitionMatrix()->get(i, j);
            if (data2 != "0") {
                std::string data4 = "";
                if (automaton.getTransitionMatrix()->get(0, j) != "0") {
                    data4 = "((" + automaton.getTransitionMatrix()->get(0, j) + ")";
                }
                if (data3 != "0") {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + "|" + "(" + data1 + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + data2 + "))");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, "((" + data1 + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + data2 + "))");
                    }
                } else {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + "|" + "(" + data1 + ")" + "|" + "(" + data1 + data2 + "))");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, "((" + data1 + ")" + "|" + "(" + data1 + data2 + "))");
                    }
                }
            }

            std::string data5 = automaton.getTransitionMatrix()->get(j, i);
            if (data5 != "0") {
                for (int k = 0; k < automaton.getStatesNumber(); k++) {
                    if (k == i || automaton.getFinalStateMatrix()->get(k, 0) == 0) {
                        continue;
                    }
                    std::string data6 = automaton.getTransitionMatrix()->get(i, k);
                    if (data6 != "0") {
                        std::string data7 = automaton.getTransitionMatrix()->get(j, k);
                        if (data3 != "0") {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data7 + ")" + "|" + "(" + data5 + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + data6 + "))");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data5 + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + data6 + "))");
                            }
                        } else {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data7 + ")" + "|" + "(" + data5 + ")" + "|" + "(" + data5 + data6 + "))");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, "((" + data5 + ")" + "|" + "(" + data5 + data6 + "))");
                            }
                        }
                    }
                }
                automaton.getTransitionMatrix()->set(j, i, "0");
            }
        }
        for (int j = 0; j < automaton.getStatesNumber(); j++) {
            automaton.getTransitionMatrix()->set(i, j, "0");
        }
        automaton.getTransitionMatrix()->set(0, i, "0");
    }

    if (automaton.getFinalStateMatrix()->get(0, 0) == 1) {
        regex += "E";
    }

    for (int i = 1; i < automaton.getStatesNumber(); i++) {
        if (automaton.getFinalStateMatrix()->get(i, 0) == 0) {
            continue;
        }
        if (automaton.getTransitionMatrix()->get(0, i) != "0") {
            if (regex != "") {
                regex += "|";
            }
            if (automaton.getTransitionMatrix()->get(i, i) != "0") {
                regex += automaton.getTransitionMatrix()->get(0, i) + "(" + automaton.getTransitionMatrix()->get(i, i) + ")*";
            } else {
                regex += automaton.getTransitionMatrix()->get(0, i);
            }
        }
    }

    return regex;
}

std::string regexp::convertToAcademicRegex(std::string& source, Automaton* automaton) {
    std::set<char> alphabet = getAlphabetOf(source);
    std::string expression = convertLookaheadsToIntersections(source);
    removeFrom(expression, "$");
    removeFrom(expression, "^");
    replaceDots(expression, alphabet);
    Lexer lexer;
    std::vector<Lexer::Token> infixTokens = lexer.tokenize(expression, alphabet);
    std::vector<Lexer::Token> postfixTokens = convertToPostfixForm(infixTokens);

    std::stack<Automaton> stack;
    Automaton a(1);
    Automaton b(1);

    for (int i = 0; i < postfixTokens.size(); i++) {
        switch (postfixTokens[i].getType()) {
            case Lexer::Token::kLetter:
                stack.push(buildGlushkovAutomaton(postfixTokens[i].getData()));
                break;
            case Lexer::Token::kKleeneStar:
                a = stack.top();
                stack.pop();
                stack.push(Automaton::getKleeneClosure(a));
                break;
            case Lexer::Token::kIntersection:
                b = stack.top();
                stack.pop();
                a = stack.top();
                stack.pop();
                stack.push(Automaton::getIntersection(a, b));
                break;
            case Lexer::Token::kAlternative:
                b = stack.top();
                stack.pop();
                a = stack.top();
                stack.pop();
                stack.push(Automaton::getUnion(a, b));
                break;
            case Lexer::Token::kConcatenation:
                b = stack.top();
                stack.pop();
                a = stack.top();
                stack.pop();
                stack.push(Automaton::getConcatenation(a, b));
                break;
        }
    }

    *automaton = stack.top();

    std::string academicRegex = convertAutomatonToRegex(*automaton);

    return academicRegex;
}

bool regexp::dfs(Matrix<std::string>* transitionMatrix, Matrix<bool>* visitMatrix, int sVertex, std::vector<int>* dVertices) {
    for (int i = 0; i < dVertices->size(); i++) {
        if (sVertex == dVertices->at(i)) {
            return true;
        }
    }
	if (visitMatrix->get(sVertex, 0))
        return false;

	visitMatrix->set(sVertex, 0, true);

	for (int i = 0; i < visitMatrix->getN(); i++) {
        if (transitionMatrix->get(sVertex, i) == "0") {
            continue;
        }
		if (!visitMatrix->get(i, 0)) {
			bool reached = dfs(transitionMatrix, visitMatrix, i, dVertices);
			if (reached) {
                return true;
            }
		}
	}
	return false;
}

void generateWordsHelper(Automaton& automaton, std::string currentWord, std::vector<std::string>& words, int currentState, std::vector<int>& visited, int quantity, int limit) {
    if (words.size() >= quantity || visited[currentState] >= limit) {
        return;
    }

    if (automaton.getFinalStateMatrix()->get(currentState, 0) == 1) {
        words.push_back(currentWord);
    }

    std::vector<int> possibleNextStates;
    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (automaton.getTransitionMatrix()->get(currentState, i) != "0") {
            possibleNextStates.push_back(i);
        }
    }

    if (!possibleNextStates.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, possibleNextStates.size() - 1);
        int randomIndex = distrib(gen);
        int nextState = possibleNextStates[randomIndex];

        std::string transition = automaton.getTransitionMatrix()->get(currentState, nextState);
        std::set<std::string> symbols;
        std::string temp;

        for (char c : transition) {
            if (c == '(' || c == ')' || c == '|') {
                if (!temp.empty()) {
                    symbols.insert(temp);
                    temp.clear();
                }
            }
            else {
                temp += c;
            }
        }
        if (!temp.empty()) {
            symbols.insert(temp);
        }

        if (symbols.empty()) {
            return;
        }

        std::uniform_int_distribution<> distribSymb(0, symbols.size() - 1);
        int randomSymbIndex = distribSymb(gen);
        std::string nextSymbol = *std::next(symbols.begin(), randomSymbIndex);

        std::string nextWord = currentWord + nextSymbol;

        visited[currentState]++;
        generateWordsHelper(automaton, nextWord, words, nextState, visited, quantity, limit);
        visited[currentState]--;
    } else {
        return;
    }
}

std::vector<std::string> regexp::generateWords(Automaton& automaton, int quantity) {
    std::vector<std::string> words;

    std::vector<std::vector<int>> data = *(automaton.getFinalStateMatrix()->getData());
    bool allZero = true;
    for (const auto& row : data) {
        if (row.at(0) != 0) {
            allZero = false;
            break;
        }
    }

    if (automaton.getTransitionMatrix()->toString() == "0 \n" || allZero) {
        return words;
    }

    int startState = 0;
    std::vector<int> visited(automaton.getStatesNumber(), 0);

    while (words.size() < quantity) {
        std::string s = "";
        generateWordsHelper(automaton, s, words, startState, visited, quantity, 5);
    }

    return words;
}


char generateRandomLetter() {
    std::string alphabet = "abcde";
    static std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, ALPHABET_SIZE - 1);

    return alphabet[dist(engine)];
}

std::string generateRandomUnary(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind) {
    if (lookaheadInAlternative || depth - 1 <= 0) {
        return "";
    }

    std::string regex = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    
    if (regex == "") {
        return regex;
    }
    
    if (currentStarHeight >= STAR_HEIGHT) {
        std::string letter;
        letter += generateRandomLetter();
        return letter;
    }

    if (regex.find("(?=") != std::string::npos || regex.find("(?<=") != std::string::npos) {
        return "";
    }

    currentStarHeight++;
    prefixBinary = true;

    return "(" + regex + ")*";
}

std::string generateRandomLookahead(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind) {
    if (lookaheadInAlternative) {
        return "";
    }

    std::string regex = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);

    prefixBinary = true;

    if (currentNumLookaheads >= NUM_LOOKAHEADS || regex == "") {
        std::string letter;
        letter += generateRandomLetter();
        return letter;
    }

    currentNumLookaheads++;

    static std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distBool(0, 1);
    bool appendDollarSign = distBool(engine);

    std::string lookahead = "(?=" + regex;

    if (appendDollarSign) {
        lookahead += "$)";
    }
    else {
        lookahead += ")";
    }

    if (depth - 1 > 0) {
        regex = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
        if (regex == "") {
            return lookahead + generateRandomLetter();
        } else {
            return lookahead + regex;
        }
    } else {
        return lookahead + generateRandomLetter();
    }
}

std::string generateRandomLookbehind(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind) {
    if (lookaheadInAlternative) {
        return "";
    }

    std::string regex = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);

    prefixBinary = true;

    if (currentNumLookaheads >= NUM_LOOKBEHINDS || regex == "") {
        std::string letter;
        letter += generateRandomLetter();
        return letter;
    }

    currentNumLookbehinds++;

    static std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distBool(0, 1);
    bool appendCaretSign = distBool(engine);

    std::string lookbehind = "";

    if (regex.find("*") != std::string::npos || regex.find("|") != std::string::npos) {
        unaryBinaryInLookbehind = true;
    }

    if (appendCaretSign) {
        lookbehind += "(?<=^" + regex + ")";
    } else {
        lookbehind += "(?<=" + regex + ")";
    }

    if (depth - 1 > 0) {
        regex = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
        if (regex == "") {
            return generateRandomLetter() + lookbehind;
        } else {
            return regex + lookbehind;
        }
    } else {
        return generateRandomLetter() + lookbehind;
    }

}

std::string generateRandomBinary(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind) {
    if (lookaheadInAlternative || depth - 1 <= 0) {
        return "";
    }
    
    if (depth <= 0) {
        std::string letter;
        letter += generateRandomLetter();
        prefixBinary = true;
        return letter;
    }

    bool temp = prefixBinary;

    std::string left = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    std::string right = generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);

    if (temp && (left.find("(?<=") != std::string::npos || right.find("(?<=") != std::string::npos)) {
        return "";
    }

    if (left == "" || right == "") {
        return "";
    }

    if (left.find("(?=") != std::string::npos || right.find("(?=") != std::string::npos) {
        lookaheadInAlternative = true;
    }

    prefixBinary = true;
    
    return "(" + left + "|" + right + ")";
}

std::string regexp::generateRandomRegex(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind) {
    if (depth <= 0) {
        return "";
    }

    if (lookaheadInAlternative) {
        return generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    }

    std::string regex;
    static std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 100);
    int choice = dist(engine);

    if (choice < 20) {
        regex = generateRandomUnary(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    }
    else if (choice < 40 && !nestedLookahead) {
        nestedLookahead = true;
        regex = generateRandomLookahead(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
        nestedLookahead = false;
    }
    else if (choice < 60 && !nestedLookahead) {
        nestedLookahead = true;
        regex = generateRandomLookbehind(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
        nestedLookahead = false;
    }
    else if (choice < 80) {
        regex = generateRandomBinary(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    }
    else {
        prefixBinary = true;
        regex = generateRandomLetter();
    }

    return regex + generateRandomRegex(depth - 1, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
}

bool unaryBinaryInLookbehind = false;

std::string regexp::generateRandomRegex() {
    int currentStarHeight = 0;
    int currentNumLookaheads = 0;
    int currentNumLookbehinds = 0;
    bool nestedLookahead = false;
    bool lookaheadInAlternative = false;
    bool prefixBinary = false;

    std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(4, MAX_LETTERS);
    int depth = dist(engine);
    std::string regex = generateRandomRegex(depth, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);

    while (regex == "") {
        currentStarHeight = 0;
        currentNumLookaheads = 0;
        nestedLookahead = false;
        lookaheadInAlternative = false;
        prefixBinary = false;

        depth = dist(engine);
        regex = generateRandomRegex(depth, currentStarHeight, currentNumLookaheads, nestedLookahead, lookaheadInAlternative, currentNumLookbehinds, prefixBinary, unaryBinaryInLookbehind);
    }

    return "^" + regex + "$";
}

void regexp::testRegularExpressions(const std::string& outputFileName, int numExpressions, int numWords) {
    std::ofstream outFile(outputFileName);

    if (!outFile) {
        std::cerr << "Cannot open the output file." << std::endl;
        return;
    }

    for (int i = 0; i < numExpressions; ++i) {
        std::string originalRegex = generateRandomRegex();

        Automaton automaton(1);
        std::string academicRegex = convertToAcademicRegex(originalRegex, &automaton);

        outFile << "Original regex: " << originalRegex << "\n";
        outFile << "Academic regex: " << academicRegex << "\n";

        if (academicRegex.substr(0, 2) == "E|") {
            academicRegex = academicRegex.substr(2);
        }  

        std::vector<std::string> words = generateWords(automaton, numWords);

        std::set<std::string> mismatchesOriginal, mismatchesAcademic, complexWords;
        boost::regex originalPattern(originalRegex);
        boost::regex academicPattern(academicRegex);

        for (const auto& word : words) {
            if (!unaryBinaryInLookbehind) {
                try {
                    if ((!word.empty() && !boost::regex_match(word, originalPattern)) || (word.empty() && automaton.getFinalStateMatrix()->get(0, 0) != 1)) {
                        mismatchesOriginal.insert(word);
                    }
                } catch (const boost::regex_error& e) {
                    std::cerr << "Error matching word '" << word << "' against original pattern: " << e.what() << std::endl;
                    complexWords.insert(word);
                }
            }
s
            try {
                if ((!word.empty() && !boost::regex_match(word, academicPattern)) || (word.empty() && automaton.getFinalStateMatrix()->get(0, 0) != 1)) {
                    mismatchesAcademic.insert(word);
                }
            } catch (const boost::regex_error& e) {
                    std::cerr << "Error matching word '" << word << "' against academic pattern: " << e.what() << std::endl;
                    complexWords.insert(word);
            }
        }

        outFile << "Words generated: " << words.size() << "\n";
        outFile << "Words too complex to try matching with regexes: " << complexWords.size() << "\n";

        if (!mismatchesOriginal.empty() && !words.empty()) {
            outFile << "Words that do not match the original regex:\n";
            for (const auto& word : mismatchesOriginal) {
                outFile << word << "\n";
            }
        } else if (!words.empty()) {
            if (unaryBinaryInLookbehind) {
                outFile << "The original regex has lookbehind with alternative  or star: no check for word match is performed\n";
            } else {
                outFile << "All words match the original regex.\n";
            }        
        }
        if (!mismatchesAcademic.empty() && !words.empty()) {
            outFile << "Words that do not match the academic regex:\n";
            for (const auto& word : mismatchesAcademic) {
                outFile << word << "\n";
            }
        } else if (!words.empty()) {
            outFile << "All words match the academic regex.\n";
        }
        if (words.empty()) {
            outFile << "No words were generated.\n";
        }
        outFile << "-----------------------------\n";
    }
    outFile.close();
}
