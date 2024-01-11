#include <regexp.h>
#include <string>
#include <regex>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <sstream>

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
                    vector.push_back(Token("_", Token::kConcatenation));
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

std::string regexp::getSubstring(std::string source, int start, int end) {
    return source.substr(start, end - start + 1);
} 

std::string regexp::removeExtraParentheses(std::string source) {
    if (source.length() <= 2) {
        return source;
    }
    // removing '(' and ')' in source = (...)
    // except:
    // source = (...)...(...)
    // source = (?=...)
    // source = (?<=...)
    if (source[0] == '(' && source[source.length() - 1] == ')') {
        if (source[1] != '?') {
            int n = 0;
            for (int i = 1; i < source.length() - 1; i++) {
                if (source[i] == '(') {
                    n++;
                } else if (source[i] == ')') {
                    n--;
                    if (n < 0) {
                        break;
                    }
                }
            }
            if (n == 0) {
                return removeExtraParentheses(getSubstring(source, 1, source.length() - 2));
            }
        }
    }

    // (?=...) and (?<=...)
    // except like (?...)...(...)*
    if (source[0] == '(' && source[source.length() - 1] == ')') {
        if (source.length() >= 4) {
            if (source[1] == '?' && source[2] == '=') {
                if (source.length() == 4) {
                    return source;
                } else {
                    int n = 0;
                    for (int i = 3; i < source.length() - 1; i++) {
                        if (source[i] == '(') {
                            n++;
                        } else if (source[i] == ')') {
                            n--;
                            if (n < 0) {
                                break;
                            }
                        }
                    }
                    if (n == 0) {
                        return "(?=" + removeExtraParentheses(getSubstring(source, 3, source.length() - 2)) + ")";
                    }
                }
            }
        } else if (source.length() >= 5) {
            if (source[1] == '?' && source[2] == '<' && source[3] == '=') {
                if (source.length() == 5) {
                    return source;
                } else {
                    int n = 0;
                    for (int i = 4; i < source.length() - 1; i++) {
                        if (source[i] == '(') {
                            n++;
                        } else if (source[i] == ')') {
                            n--;
                            if (n < 0) {
                                break;
                            }
                        }
                    }
                    if (n == 0) {
                        return "(?<=" + removeExtraParentheses(getSubstring(source, 4, source.length() - 2)) + ")";
                    }
                }
            }
        }
    }

    // source = (...)*
    // except like (...)...(...)*
    if (source.length() > 3) {
        if (source[0] == '(' && source[source.length() - 1] == '*') {
            if (source.length() == 4) {
                return getSubstring(source, 1, 1) + "*";
            } else {
                int n = 0;
                for (int i = 1; i < source.length() - 2; i++) {
                    if (source[i] == '(') {
                        n++;
                    } else if (source[i] == ')') {
                        n--;
                        if (n < 0) {
                            break;
                        }
                    }
                }
                if (n == 0) {
                    return "(" + removeExtraParentheses(getSubstring(source, 1, source.length() - 3)) + ")*";
                }
            }
        }
    }

    //source = ... | ...
    int n = 0;
    for (int i = 0; i < source.length(); i++) {
        if (source[i] == '(') {
            n++;
        } else if (source[i] == ')') {
            n--;
        } else if (n == 0 && source[i] == '|') {
            return removeExtraParentheses(getSubstring(source, 0, i - 1)) + "|" + removeExtraParentheses(getSubstring(source, i + 1, source.length() - 1));
        }
    }

    std::string output;

    n = 0;
    bool parentheses = false;
    int startIndex = 0;
    int endIndex = -1;
    for (int i = 0; i < source.length() - 1; i++) {
        if (source[i] == '(') {
            if (n == 0 && !parentheses) {
                startIndex = i;
                parentheses = true;
            }
            n++;
        } else if (source[i] == ')') {
            n--;
            if (n == 0 && parentheses) {
                if (i < source.length() - 1 && source[i + 1] == '*') {
                    if (startIndex - endIndex > 1) {
                        output += getSubstring(source, endIndex + 1, startIndex - 1);
                    }
                    endIndex = i + 1;
                    output += removeExtraParentheses(getSubstring(source, startIndex, endIndex));
                    i++;
                } else {
                    int m = 0;
                    bool alternativeExist = false;
                    for (int j = startIndex + 1; j < i; j++) {
                        if (source[j] == '(') {
                            m++;
                        } else if (source[j] == ')') {
                            m--;
                        } else if (m == 0 && source[j] == '|') {
                            alternativeExist = true;
                            break;
                        }
                    }
                    if (startIndex - endIndex > 1) {
                        output += getSubstring(source, endIndex + 1, startIndex - 1);
                    }
                    endIndex = i;
                    if (alternativeExist) {
                        output += "(" + removeExtraParentheses(getSubstring(source, startIndex + 1, endIndex - 1)) + ")";
                    } else {
                        if (endIndex - startIndex - 1 > 0) {
                            output += removeExtraParentheses(getSubstring(source, startIndex, endIndex));
                        }
                    }
                }
                parentheses = false;
            }
        }
    }

    if (endIndex == -1) {
        return source;
    } else if (endIndex < source.length() - 1) {
        output += getSubstring(source, endIndex + 1, source.length() - 1);
    }

    return output;
}

std::string regexp::convertLookaheadsToIntersections(std::string source, int start, int end) {

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
    bool existLookahead = false;
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
            if (existEndSymbol) {
                r2 = getSubstring(source, i, j - 3);
            } else {
                r2 = getSubstring(source, i, j - 2);
            }
            r2 = getSubstring(source, i, j - 2);
            existLookahead = true;
            i = j;
            break;
        }

        result += source[i];
        i++;
    }

    if (existLookahead) {
        result += "(" + r2;
        if (!existEndSymbol) {
            result += ".*";
        }
        result += "&" + convertLookaheadsToIntersections(source, i, end) + ")";
    }

    result += ")";

    return result;
}

std::string regexp::convertLookbehindsToIntersections(std::string source, int start, int end) {
    if (start > end) {
        return "";
    }

    int n = 0;
    
    for (int i = end; i >= start; i--) {
        if (source[i] == ')') {
            n++;
        } else if (source[i] == '(') {
            n--;
        } else if (n == 0 && source[i] == '|') {
            return "(" + convertLookbehindsToIntersections(source, start, i - 1) + "|" + convertLookbehindsToIntersections(source, i + 1, end) + ")";
        }
    }

    std::string result = ")";
    std::string r2 = "";
    int i = end;
    bool existStartSymbol = false;
    int index = -1;
    std::stack<int> stack;

    while (i >= start) {
        if (source[i] == ')') {
            stack.push(i);
        }
        if (source[i] == '(') {
            if (i <= end - 3 && source[i + 1] == '?' && source[i + 2] == '<' && source[i + 3] == '=' && stack.size() == 1) {
                if (i <= end - 4) {
                    if (source[i + 4] == '^') {
                        existStartSymbol = true;
                    }
                }
                index = stack.top();
                stack.pop();
                r2 = convertLookbehindsToIntersections(source, i + 4, index - 1);
                i--;
                break;
            }
            stack.pop();
        }
        i--;
    }

    if (index != -1) {
        result = getSubstring(source, index + 1, end) + result;
    } else {
        result = getSubstring(source, start, end) + result;
    }

    if (!r2.empty()) {
        result = r2 + ")" + result;
        if (!existStartSymbol) {
            result = ".*" + result;
        }

        result = "(" + convertLookbehindsToIntersections(source, start, i) + "&" + result;
    }

    result = "(" + result;

    return result;
}


std::string regexp::convertLookaheadsToIntersections(std::string source) {
    if (source[source.length() - 2] == ')') {
        bool containsLookaheads = false;
        int index = -1;
        int n = 1;
        for (int i = source.length() - 3; i >= 0; i--) {
            if (source[i] == ')') {
                n++;
            } else if (source[i] == '(') {
                n--;
            }
            if (n == 1 && source[i] == '|' && !containsLookaheads) {
                containsLookaheads = true;
            }
            if (n == 0) {
                index = i;
                break;
            }
        }
        if (containsLookaheads) {
            std::string part = convertLookaheadsToIntersections(source, index + 1, source.length() - 3);
            source = getSubstring(source, 0, index - 1) + "(" + part + ")$";
        }
    }

    return convertLookaheadsToIntersections(source, 0, source.length() - 1);
}

std::string regexp::convertLookbehindsToIntersections(std::string source) {
    if (source[1] == '(') {
        bool containsLookbehinds = false;
        int index = -1;
        int n = 1;
        for (int i = 2; i < source.length(); i++) {
            if (source[i] == '(') {
                n++;
            } else if (source[i] == ')') {
                n--;
            }
            if (n == 1 && source[i] == '|' && !containsLookbehinds) {
                containsLookbehinds = true;
            }
            if (n == 0) {
                index = i;
                break;
            }
        }
        if (containsLookbehinds) {
            std::string part = convertLookbehindsToIntersections(source, 2, index - 1);
            source = "^(" + part + ")" + getSubstring(source, index + 1, source.length() - 1);
        }
    }

    return convertLookbehindsToIntersections(source, 0, source.length() - 1);
}

std::string regexp::removeLookaheads(std::string source) {
    std::string output;
    int n = 0;

    int found = false;
    for (int i = 0; i < source.length(); i++) {
        if (source[i] == '(') {
            if (i < source.length() - 2) {
                if (source[i + 1] == '?' && source[i + 2] == '=') {
                    found = true;
                    n = 0;
                }
            }
            n++;
        }
        if (source[i] == ')') {
            n--;
            if (found && n == 0) {
                found = false;
                continue;
            }
        }
        if (!found) {
            output += source[i];
        }
    }

    return output;
}

std::string regexp::removeLookbehinds(std::string source) {
    std::string output;
    int n = 0;

    int found = false;
    for (int i = 0; i < source.length(); i++) {
        if (source[i] == '(') {
            if (i < source.length() - 3) {
                if (source[i + 1] == '?' && source[i + 2] == '<' && source[i + 3] == '=') {
                    found = true;
                    n = 0;
                }
            }
            n++;
        }
        if (source[i] == ')') {
            n--;
            if (found && n == 0) {
                found = false;
                continue;
            }
        }
        if (!found) {
            output += source[i];
        }
    }

    return output;
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

std::string regexp::convertToAcademicRegex(std::string source, Automaton* automaton) {
    if (source == "^$") {
        return "()";
    }
    source = removeExtraParentheses(source);
    std::set<char> alphabet = getAlphabetOf(source);
    
    std::string ex1 = convertLookaheadsToIntersections(removeLookbehinds(source));
    std::string ex2 = convertLookbehindsToIntersections(removeLookaheads(source));
    std::string expression = "(" + ex1 + "&" + ex2 + ")";

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

    academicRegex = removeExtraParentheses(academicRegex);

    if (academicRegex == "") {
        academicRegex = "()";
    }

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