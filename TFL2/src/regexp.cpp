#include <regexp.h>
#include <string>
#include <regex>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <set>

using namespace regexp;

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
    std::string result = "(";
    std::string r2 = "";
    int n = 1;
    int i = start;
    bool existEndSymbol = false;
    while (i <= end) {
        if (i <= end - 2 && source[i + 1] == '?' && source[i + 2] == '=') {
            result += source[i];
            i += 3;
            int j = i;
            while (n > 0)
            {
                if (source[j] == '(')
                {
                    n++;
                }
                else if (source[j] == ')')
                {
                    n--;
                }
                if (n == 0 && source[j - 1] == '$')
                {
                    existEndSymbol = true;
                } 
                j++;
            }
            r2 = convertLookaheadsToIntersections(source, i, j - 2);
            i = j;
            break;
        }
        else
        {
            result += source[i];
        }

        i++;
    }

    if (!r2.empty())
    {
        result += r2;
        if (!existEndSymbol)
        {
            result += ".*";
        }
        result += "&" + convertLookaheadsToIntersections(source, i, end) + ")";
    }

    result += ")";

    return result;
}

std::string regexp::convertLookaheadsToIntersections(std::string& source) {
    return convertLookaheadsToIntersections(source, 1, source.length() - 1);
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
        automaton.getTransitionMatrix()->set(i, i, "0"); //last update
        for (int j = 0; j < automaton.getStatesNumber(); j++) {
            if (j == i) {
                continue;
            }
            std::string data2 = automaton.getTransitionMatrix()->get(i, j);
            if (data2 != "0") {
                std::string data4 = "";
                if (automaton.getTransitionMatrix()->get(0, j) != "0") {
                    data4 = "(" + automaton.getTransitionMatrix()->get(0, j) + ")|(";
                }
                if (data3 != "0") {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + data1 + "(" + data3 + ")*" + data2 + ")");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, data1 + "(" + data3 + ")*" + data2);
                    }
                } else {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + data1 + data2 + ")");
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
                                automaton.getTransitionMatrix()->set(j, k, "(" + data7 + ")|(" + data5 + "(" + data3 + ")*" + data6 + ")");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, data5 + "(" + data3 + ")*" + data6);
                            }
                        } else {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "(" + data7 + ")|(" + data5 + data6 + ")");
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
                    data4 = "(" + automaton.getTransitionMatrix()->get(0, j) + ")";
                }
                if (data3 != "0") {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + "|" + "(" + data1 + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + data2 + ")");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, "(" + data1 + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + ")" + "|" + "(" + data1 + "(" + data3 + ")*" + data2 + ")");
                    }
                } else {
                    if (data4 != "") {
                        automaton.getTransitionMatrix()->set(0, j, data4 + "|" + "(" + data1 + ")" + "|" + "(" + data1 + data2 + ")");
                    } else {
                        automaton.getTransitionMatrix()->set(0, j, "(" + data1 + ")" + "|" + "(" + data1 + data2 + ")");
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
                                automaton.getTransitionMatrix()->set(j, k, "(" + data7 + ")" + "|" + "(" + data5 + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + data6 + ")");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, "(" + data5 + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + ")" + "|" + "(" + data5 + "(" + data3 + ")*" + data6 + ")");
                            }
                        } else {
                            if (data7 != "0") {
                                automaton.getTransitionMatrix()->set(j, k, "(" + data7 + ")" + "|" + "(" + data5 + ")" + "|" + "(" + data5 + data6 + ")");
                            } else {
                                automaton.getTransitionMatrix()->set(j, k, "(" + data5 + ")" + "|" + "(" + data5 + data6 + ")");
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
                regex += "(" + automaton.getTransitionMatrix()->get(0, i) + "(" + automaton.getTransitionMatrix()->get(i, i) + ")*" + ")";
            } else {
                regex += "(" + automaton.getTransitionMatrix()->get(0, i) + ")";
            }
        }
    }

    return regex;
}

std::string regexp::convertToAcademicRegex(std::string& source, Automaton* automaton) {
    std::set<char> alphabet = getAlphabetOf(source);
    std::string expression = convertLookaheadsToIntersections(source);
    removeFrom(expression, "$");
    replaceDots(expression, alphabet);
    Lexer lexer;
    std::vector<Lexer::Token> infix_tokens = lexer.tokenize(expression, alphabet);
    std::vector<Lexer::Token> postfix_tokens = convertToPostfixForm(infix_tokens);

    std::stack<Automaton> stack;
    Automaton a(1);
    Automaton b(1);

    for (int i = 0; i < postfix_tokens.size(); i++) {
        switch (postfix_tokens[i].getType()) {
            case Lexer::Token::kLetter:
                stack.push(buildGlushkovAutomaton(postfix_tokens[i].getData()));
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

	for(int i = 0; i < visitMatrix->getN(); i++) {
        if (transitionMatrix->get(sVertex, i) == "0") {
            continue;
        }
		if(!visitMatrix->get(i, 0)) {
			bool reached = dfs(transitionMatrix, visitMatrix, i, dVertices);
			if (reached) {
                return true;
            }
		}
	}
	return false;
}

/*void Automaton::generateWords(int currentState, std::string currentWord, std::set<std::string>& words) {
   if (this->getFinalStateMatrix()->get(currentState, 0) == 1) {
       words.insert(currentWord);
   }

   for (int i = 0; i < this->getStatesNumber(); i++) {
       std::string transition = this->getTransitionMatrix()->get(currentState, i);
       if (transition != "0") {
           generateWords(i, currentWord + transition, words);
       }
   }
}

void Automaton::writeWordsToFile(const std::string& filename) {
   std::set<std::string> words;
   generateWords(0, "", words);

   std::ofstream file(filename);
   if (!file) {
       std::cerr << "Unable to open file " << filename << std::endl;
       return;
   }

   for (const std::string& word : words) {
       file << word << std::endl;
   }

   file.close();
}*/

/*std::vector<std::string> generateWords(Automaton& a) {
   std::vector<std::string> words;
   std::vector<int> visited(a.getStatesNumber(), 0);
   std::stack<std::pair<int, std::string>> stack;

   // Start from the initial state
   stack.push({0, ""});

   while (!stack.empty()) {
       auto current = stack.top();
       stack.pop();

       // If the current state is final, add the generated word to the list
       if (a.getFinalStateMatrix()->get(current.first, 0) == 1) {
           words.push_back(current.second);
       }

       // For each transition from the current state
       for (int i = 0; i < a.getStatesNumber(); i++) {
           std::string transition = a.getTransitionMatrix()->get(current.first, i);

           // If the transition is not empty and the next state has not been visited
           if (transition != "0" && visited[i] == 0) {
               visited[i] = 1;
               stack.push({i, current.second + transition});
           }
       }
   }

   return words;
}*/

// Function to find all paths from the initial state to a final state
/*std::vector<std::string> findPaths(Automaton& automaton, std::stack<int>& dfsStack) {
   std::vector<std::string> paths;
   while (!dfsStack.empty()) {
       int currentState = dfsStack.top();
       dfsStack.pop();

       // Continue building the current path
       std::vector<int> path;
       path.push_back(currentState);

       // Check if the current state is a final state
       if (automaton.getFinalStateMatrix()->get(currentState, 0) == 1) {
           // Convert the path to a string and add it to the list of paths
           std::string pathStr;
           for (const auto& state : path) {
               pathStr += automaton.getTransitionMatrix()->get(state, 0);
           }
           paths.push_back(pathStr);
       } else {
           // Push its neighboring states onto the stack
           for (int i = 0; i < automaton.getTransitionMatrix()->getN(); ++i) {
               if (automaton.getTransitionMatrix()->get(currentState, i) != "0") {
                  dfsStack.push(i);
               }
           }
       }
   }
   return paths;
}


std::vector<std::string> generateWords(Automaton& automaton, int quantity) {
  // Initialize DFS stack with the initial state
  std::stack<int> dfsStack;
  dfsStack.push(0);

  // Vector to store the generated words
  std::vector<std::string> generatedWords;

  // Run DFS
  for (int i = 0; i < quantity; ++i) {
      // Find all paths from the initial state to a final state
      std::vector<std::string> paths = findPaths(automaton, dfsStack);

      // Add the generated words to the vector
      for (const auto& path : paths) {
          generatedWords.push_back(path);
      }
  }

  // Return the vector of generated words
  return generatedWords;
}*/

void generateWordsHelper(Automaton& automaton, std::vector<int>& visitedStates, std::vector<int>& parent, std::string& currentWord, std::set<std::string>& words, int currentState, int quantity) {
    if (words.size() >= quantity) {
        return;
    }

    visitedStates[currentState] = 1;

    if (parent[currentState] != -1) {
        currentWord += automaton.getTransitionMatrix()->get(parent[currentState], currentState);
    }


    if (automaton.getFinalStateMatrix()->get(currentState, 0) == 1) {
        words.insert(currentWord);
    }

    for (int i = 0; i < automaton.getStatesNumber(); i++) {
        if (i != currentState && automaton.getTransitionMatrix()->get(currentState, i) != "0" && visitedStates[i] == 0) {
            parent[i] = currentState;
            std::string tempWord = currentWord;
            generateWordsHelper(automaton, visitedStates, parent, tempWord, words, i, quantity);
            parent[i] = -1;
        }
    }


    if (automaton.getTransitionMatrix()->get(currentState, currentState) != "0") {
        currentWord += automaton.getTransitionMatrix()->get(currentState, currentState);

        for (int i = 0; i < automaton.getStatesNumber(); i++) {
            if (i != currentState && automaton.getTransitionMatrix()->get(currentState, i) != "0") {
                parent[i] = currentState;
                std::string tempWord = currentWord;
                generateWordsHelper(automaton, visitedStates, parent, tempWord, words, i, quantity);
                parent[i] = -1;
            }
        }
    }

    visitedStates[currentState] = 0;
}

std::set<std::string> regexp::generateWords(Automaton& automaton, int quantity) {
    std::set<std::string> words;
    std::vector<int> visitedStates(automaton.getStatesNumber(), 0);
    std::vector<int> parent(automaton.getStatesNumber(), -1);
    
    int startState = 0;
    parent[startState] = -1;
    std::string currentWord = "";
    generateWordsHelper(automaton, visitedStates, parent, currentWord, words, startState, quantity);

    return words;
}

void regexp::checkWordsMatch(const std::set<std::string>& words, const std::string& originalRegex, const std::string& academicRegex) {
    std::regex originalPattern(originalRegex);
    std::regex academicPattern(academicRegex);

    for (const auto& word : words) {
        bool matchesOriginal = std::regex_match(word, originalPattern);
        bool matchesAcademic = std::regex_match(word, academicPattern);

        if (!matchesOriginal || !matchesAcademic) {
            std::cout << "Word '" << word << "' does not match ";
            if (!matchesOriginal) {
                std::cout << "the original expression; ";
            }
            if (!matchesAcademic) {
                std::cout << "the academic expression;";
            }
            std::cout << std::endl;
        }
    }
}
