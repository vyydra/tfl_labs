#include <slr1.h>
#include <string>
#include <iostream>
#include <queue>
#include <map>

using namespace slr1;

ProductionRule::ProductionRule(std::string leftPart, std::string rightPart) {
    this->leftPart = leftPart;
    this->rightPart = rightPart;
}

std::string ProductionRule::getLeftPart() {
    return leftPart;
}

std::string ProductionRule::getRightPart() {
    return rightPart;
}

std::string ProductionRule::toString() {
    return leftPart + " -> " + rightPart;
}

bool ProductionRule::equals(ProductionRule rule) {
    return (leftPart == rule.getLeftPart() && rightPart == rule.getRightPart());
}

std::set<char> Grammar::getTerminals() {
    return terminals;
}

std::set<char> Grammar::getNonterminals() {
    return nonterminals;
}

char Grammar::getStartSymbol() {
    return startSymbol;
}

std::vector<ProductionRule> Grammar::getProductionRules() {
    return productionRules;
}

void Grammar::setTerminals(std::set<char> terminals) {
    this->terminals = terminals;
}

void Grammar::setNonterminals(std::set<char> nonterminals) {
    this->nonterminals = nonterminals;
}

void Grammar::setStartSymbol(char startSymbol) {
    this->startSymbol = startSymbol;
}

void Grammar::setProductionRules(std::vector<ProductionRule> productionRules) {
    this->productionRules = productionRules;
}

void Automaton::add(std::vector<ProductionRule> state) {
    states.push_back(state);
    statesNumber++;
    Matrix<char> matrix(statesNumber, statesNumber);
    matrix.setAll('0');
    for (int i = 0; i < statesNumber - 1; i++) {
        for (int j = 0; j < statesNumber - 1; j++) {
            matrix.set(i, j, transitionMatrix.get(i, j));
        }
    }
    transitionMatrix = matrix;
}

void Automaton::setInitialState(int state) {
    initialState = state;
}

void Automaton::addFinalState(int state) {
    finalStates.insert(state);
}

void Automaton::removeFinalState(int state) {
    finalStates.erase(state);
}


std::vector<std::vector<ProductionRule>>* Automaton::getStates() {
    return &states;
}

Matrix<char>* Automaton::getTransitionMatrix() {
    return &transitionMatrix;
}

std::set<int> Automaton::getFinalStates() {
    return finalStates;
}

std::string Automaton::toString() {
    std::stringstream ss;
    ss << "digraph automaton {\n";
    ss << "\trankdir=LR\n";
    ss << "\tnode [shape = doublecircle]";

    bool exist = false;
    for (auto stateNumber : finalStates) {
        if (!exist) {
            ss << "; " << stateNumber;
            exist = true;
        } else {
            ss << " " << stateNumber;
        }
    }
    ss << ";\n";

    ss << "\tnode [shape = circle];\n";

    for (int i = 0; i < transitionMatrix.getN(); i++) {
        for (int j = 0; j < transitionMatrix.getM(); j++) {
            if (transitionMatrix.get(i, j) != '0') {
                std::stringstream stream1;
                std::stringstream stream2;

                stream1 << "\"" << i << "\\n";
                stream2 << "\"" << j << "\\n";

                for (int k = 0; k < states[i].size(); k++) {
                    stream1 << states[i][k].toString() << "\\n";
                }
                for (int k = 0; k < states[j].size(); k++) {
                    stream2 << states[j][k].toString() << "\\n";
                }

                stream1 << "\"";
                stream2 << "\"";

                ss << "\t" << stream1.str() << " -> " << stream2.str() << " [label = \"" << transitionMatrix.get(i, j) << "\"];\n";
            }
        }
    }

    ss << "}\n";

    return ss.str();
}

void slr1::parse(Grammar grammar, std::string word) {

    bool slrProperty;

    std::vector<std::map<char, std::pair<std::string, int>>> table = buildSLR1Table(grammar, &slrProperty);

    if (!slrProperty) {
        std::cout << "BAD";
    }

    std::cout << "SLR(1) TABLE: " << std::endl;

    for (int i = 0; i < table.size(); i++) {
        std::cout << i << ":" << std::endl;
        std::pair<std::string, int> pair("", 0);
        for (auto terminal : grammar.getTerminals()) {
            if (table[i][terminal].first != "") {
                std::cout << terminal << " [" << table[i][terminal].first << ", " << table[i][terminal].second << "]" << std::endl;
            }
        }

        if (table[i]['~'].first != "") {
            std::cout << '~' << " [" << table[i]['~'].first << ", " << table[i]['~'].second << "]" << std::endl;
        }

        for (auto nonterminal : grammar.getNonterminals()) {
            if (table[i][nonterminal].first != "") {
                std::cout << nonterminal << " [" << table[i][nonterminal].first << ", " << table[i][nonterminal].second << "]" << std::endl;
            }
        }

        std::cout << std::endl;
    }
}

std::vector<std::map<char, std::pair<std::string, int>>> slr1::buildSLR1Table(Grammar grammar, bool* slrProperty) {

    std::vector<ProductionRule> g;
    g.push_back(ProductionRule("s0", ".S")); // s0 = S₀ 's' must be small for this algorithm
    for (int i = 0; i < grammar.getProductionRules().size(); i++) {
        g.push_back(ProductionRule(grammar.getProductionRules()[i].getLeftPart(), "." + grammar.getProductionRules()[i].getRightPart()));
    }

    Automaton automaton = buildLR0Automaton(grammar, g);

    std::cout << automaton.toString();

    std::map<char, std::set<char>> first;
    std::map<char, std::set<char>> follow;

    for (auto nonterminal : grammar.getNonterminals()) {
        first.insert({nonterminal, std::set<char>()});
        follow.insert({nonterminal, std::set<char>()});
    }

    constructFirst(grammar, &first);

    std::cout << std::endl;

    std::cout << "FIRST: " << std::endl;

    for (auto nonterminal : grammar.getNonterminals()) {
        std::cout << nonterminal << ": " << std::endl;
        for (auto terminal : first.at(nonterminal)) {
            std::cout << terminal << " ";
        }
        std::cout << std::endl;
    }

    constructFollow(grammar, &first, &follow);

    std::cout << std::endl;

    std::cout << "FOLLOW: " << std::endl;

    for (auto nonterminal : grammar.getNonterminals()) {
        std::cout << nonterminal << ": " << std::endl;
        for (auto terminal : follow.at(nonterminal)) {
            std::cout << terminal << " ";
        }
        std::cout << std::endl;
    }

    std::vector<ProductionRule> rules;
    rules.push_back(ProductionRule("s0", "S")); // s0 = S₀
    for (int i = 0; i < grammar.getProductionRules().size(); i++) {
        rules.push_back(ProductionRule(grammar.getProductionRules()[i].getLeftPart(), grammar.getProductionRules()[i].getRightPart()));
    }

    std::cout << std::endl;

    for (int i = 0; i < rules.size(); i++) {
        std::cout << "(" << i << ") " << rules[i].toString() << std::endl;
    }

    std::vector<std::map<char, std::pair<std::string, int>>> table;

    for (int i = 0; i < automaton.getStates()->size(); i++) {
        std::map<char, std::pair<std::string, int>> map;
        std::pair<std::string, int> pair("", 0);
        for (auto terminal : grammar.getTerminals()) {
            map.insert({terminal, pair});
        }
        for (auto nonterminal : grammar.getNonterminals()) {
            map.insert({nonterminal, pair});
        }
        map.insert({'~', pair});
        table.push_back(map);
    }

    *slrProperty = true;

    for (int i = 0; i < automaton.getStates()->size(); i++) {
        for (int j = 0; j < automaton.getStates()->at(i).size(); j++) {
            ProductionRule item = automaton.getStates()->at(i)[j];
            if (item.getLeftPart() == "s0" && item.getRightPart() == "S.") {
                std::pair<std::string, int> pair("accept", 0);
                if (table[i]['~'].first != "" && (table[i]['~'].first != pair.first || table[i]['~'].second != pair.second)) {
                    *slrProperty = false;
                }
                table[i]['~'] = pair;
            } else if (item.getRightPart()[item.getRightPart().length() - 1] == '.') {
                for (auto a : follow[item.getLeftPart()[0]]) {
                    std::string rightPart = item.getRightPart().substr(0, item.getRightPart().length() - 1);
                    int index = -1;
                    for (int k = 0; k < rules.size(); k++) {
                        if (rules[k].getLeftPart() == item.getLeftPart() && rules[k].getRightPart() == rightPart) {
                            index = k;
                            break;
                        }
                    }
                    std::pair<std::string, int> pair("reduce", index);
                    if (table[i][a].first != "" && (table[i][a].first != pair.first || table[i][a].second != pair.second)) {
                        *slrProperty = false;
                    }
                    table[i][a] = pair;
                }
            } else if (isNonTerminal(grammar, item.getRightPart()[item.getRightPart().find('.') + 1])) {
                char nonterminal = item.getRightPart()[item.getRightPart().find('.') + 1];
                int index = -1;
                for (int k = 0; k < automaton.getStates()->size(); k++) {
                    if (automaton.getTransitionMatrix()->get(i, k) == nonterminal) {
                        index = k;
                        break;
                    }
                }
                std::pair<std::string, int> pair("goto", index);
                if (table[i][nonterminal].first != "" && (table[i][nonterminal].first != pair.first || table[i][nonterminal].second != pair.second)) {
                    *slrProperty = false;
                }
                table[i][nonterminal] = pair;
            } else {
                char terminal = item.getRightPart()[item.getRightPart().find('.') + 1];
                int index = -1;
                for (int k = 0; k < automaton.getStates()->size(); k++) {
                    if (automaton.getTransitionMatrix()->get(i, k) == terminal) {
                        index = k;
                        break;
                    }
                }
                std::pair<std::string, int> pair("shift", index);
                if (table[i][terminal].first != "" && (table[i][terminal].first != pair.first || table[i][terminal].second != pair.second)) {
                    *slrProperty = false;
                }
                table[i][terminal] = pair;
            }
        }
    }

    for (int i = 0; i < automaton.getStates()->size(); i++) {
        std::cout << "PRIORITY" << i << ": " << rules[getPriorityRuleIndex(grammar, automaton, i, true)].toString() << std::endl;
    }

    return table;
}

Automaton slr1::buildLR0Automaton(Grammar grammar, std::vector<ProductionRule> g) {
    Automaton automaton;
    std::vector<ProductionRule> state0;
    state0.push_back(g[0]);
    automaton.add(state0);

    std::queue<int> queue;
    queue.push(0);

    while (!queue.empty()) {
        int stateNumber = queue.front();
        queue.pop();
        int count = 0;
        std::vector<ProductionRule> items = closure(automaton.getStates()->at(stateNumber), g, grammar);
        count += items.size();
        for (auto item : items) {
            bool exist = false;
            int index = -1;
            for (int i = 0; i < automaton.getStates()->size(); i++) {
                if (automaton.getStates()->at(i)[0].equals(item)) {
                    exist = true;
                    index = i;
                    break;
                }
            }

            if (!exist) {
                std::vector<ProductionRule> state;
                state.push_back(item);
                automaton.add(state);
                automaton.getTransitionMatrix()->set(stateNumber, automaton.getStates()->size() - 1, '1');
                queue.push(automaton.getStates()->size() - 1);
            } else {
                automaton.getTransitionMatrix()->set(stateNumber, index, '1');
            }
        }

        int index = 0;
        for (int i = 0; i < automaton.getStates()->at(stateNumber)[0].getRightPart().size(); i++) {
            if (automaton.getStates()->at(stateNumber)[0].getRightPart()[i] == '.') {
                index = i;
                break;
            }
        }

        if (index < automaton.getStates()->at(stateNumber)[0].getRightPart().size() - 1) {
            std::string leftPart = automaton.getStates()->at(stateNumber)[0].getLeftPart();
            std::string rightPart = automaton.getStates()->at(stateNumber)[0].getRightPart();
            rightPart.erase(index, 1);
            char symbol = rightPart[index];
            rightPart.insert(index + 1, ".");
            ProductionRule productionRule(leftPart, rightPart);
            bool exist = false;
            int index2 = -1;
            for (int i = 0; i < automaton.getStates()->size(); i++) {
                if (automaton.getStates()->at(i)[0].equals(productionRule)) {
                    exist = true;
                    index2 = i;
                    break;
                }
            }

            if (!exist) {
                std::vector<ProductionRule> state;
                state.push_back(productionRule);
                automaton.add(state);
                automaton.getTransitionMatrix()->set(stateNumber, automaton.getStates()->size() - 1, symbol);
                queue.push(automaton.getStates()->size() - 1);
            } else {
                automaton.getTransitionMatrix()->set(stateNumber, index2, symbol);
            }

            count++;
        }

        if (count == 0) {
            automaton.addFinalState(stateNumber);
        }
    }

    Automaton a;
    for (auto state : automaton.getFinalStates()) {
        a.addFinalState(state);
    }
    
    for (int i = 0; i < automaton.getStates()->size(); i++) {
        Matrix<bool> visitMatrix(automaton.getStates()->size(), 1);
        visitMatrix.setAll(false);
        std::vector<ProductionRule> rules;
        dfs(&automaton, &visitMatrix, i, &rules);
        a.add(rules);
    }

    *a.getTransitionMatrix() = *automaton.getTransitionMatrix();

    for (int i = 0; i < a.getStates()->size(); i++) {
        bool exist = false;
        for (int j = 0; j < a.getStates()->size(); j++) {
            if (a.getTransitionMatrix()->get(j, i) == '1' && i != j) {
                exist = true;
                break;
            }
        }

        if (!exist) {
            continue;
        }

        for (int j = 0; j < a.getStates()->size(); j++) {
            if (j == i) {
                continue;
            }
            for (int k = 0; k < a.getStates()->size(); k++) {
                if (i == k) {
                    continue;
                }
                if (a.getTransitionMatrix()->get(i, k) != '0') {
                    if (a.getTransitionMatrix()->get(j, i) != '0') {
                        a.getTransitionMatrix()->set(j, k, a.getTransitionMatrix()->get(i, k));
                    }
                }
            }
        }

        for (int j = 0; j < a.getStates()->size(); j++) {
            a.getTransitionMatrix()->set(j, i, '0');
            a.getTransitionMatrix()->set(i, j, '0');
        }
    }

    std::vector<int> toRemoveStates;

    for (int i = 0; i < a.getStates()->size(); i++) {
        std::vector<std::pair<char, std::vector<int>>> pairs;
        for (int j = 0; j < a.getStates()->size(); j++) {
            char symbol = a.getTransitionMatrix()->get(i, j);
            if (symbol != '0') {
                int index = -1;
                for (int k = 0; k < pairs.size(); k++) {
                    if (pairs[k].first == symbol) {
                        index = k;
                        break;
                    }
                }
                
                if (index == -1) {
                    std::pair<char, std::vector<int>> pair;
                    pair.first = symbol;
                    pair.second = std::vector<int>();
                    pair.second.push_back(j);
                    pairs.push_back(pair);
                } else {
                    pairs[index].second.push_back(j);
                }
            }
        }

        for (int j = 0; j < pairs.size(); j++) {
            if (pairs[j].second.size() > 1) {
                std::vector<ProductionRule> rules;
                bool isFinalState = false;
                for (int k = 0; k < pairs[j].second.size(); k++) {
                    toRemoveStates.push_back(pairs[j].second[k]);
                    if (a.getFinalStates().count(pairs[j].second[k])) {
                        isFinalState = true;
                    }
                    for (auto rule : a.getStates()->at(pairs[j].second[k])) {
                        rules.push_back(rule);
                    }
                }

                a.add(rules);
                if (isFinalState) {
                    a.addFinalState(a.getStates()->size() - 1);
                }
                a.getTransitionMatrix()->set(i, a.getStates()->size() - 1, pairs[j].first);

                for (int k = 0; k < pairs[j].second.size(); k++) {
                    for (int w = 0; w < a.getStates()->size(); w++) {
                        char symbol1 = a.getTransitionMatrix()->get(w, pairs[j].second[k]);
                        char symbol2 = a.getTransitionMatrix()->get(pairs[j].second[k], w);
                        if (symbol1 != '0' && symbol1 != pairs[j].first) {
                            a.getTransitionMatrix()->set(w, a.getStates()->size() - 1, symbol1);
                        }
                        if (symbol2 != '0') {
                            a.getTransitionMatrix()->set(a.getStates()->size() - 1, w, symbol2);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < toRemoveStates.size(); i++) { 
        for (int j = 0; j < a.getStates()->size(); j++) {
            a.getTransitionMatrix()->set(j, toRemoveStates[i], '0');
            a.getTransitionMatrix()->set(toRemoveStates[i], j, '0');
        }

        if (a.getFinalStates().find(toRemoveStates[i]) != a.getFinalStates().end()) {
            a.removeFinalState(toRemoveStates[i]);
        }
    }

    for (int i = 0; i < a.getStates()->size(); i++) {
        bool emptyState = true;
        for (int j = 0; j < a.getStates()->size(); j++) {
            if (a.getTransitionMatrix()->get(i, j) != '0') {
                emptyState = false;
                break;
            }
        }
        if (emptyState) {
            for (int j = 0; j < a.getStates()->size(); j++) {
                if (a.getTransitionMatrix()->get(j, i) != '0') {
                    emptyState = false;
                    break;
                }
            }

            if (emptyState) {
                std::vector<ProductionRule> empty;
                a.getStates()->at(i) = empty;
            }
        }
    }

    Automaton result;

    std::map<int, int> map;
    int index = 0;
    for (int i = 0; i < a.getStates()->size(); i++) {
        if (!a.getStates()->at(i).empty()) {
            map.insert({i, index});
            if (a.getFinalStates().count(i) > 0) {
                result.addFinalState(index);
            }
            result.add(a.getStates()->at(i));
            index++;
        }
    }

    for (int i = 0; i < a.getStates()->size(); i++) {
        if (!a.getStates()->at(i).empty()) {
            if (i == 1) {
                std::cout << a.getStates()->at(i)[0].toString();
            }
            for (int j = 0; j < a.getStates()->size(); j++) {
                if (a.getTransitionMatrix()->get(i, j) != '0') {
                    result.getTransitionMatrix()->set(map[i], map[j], a.getTransitionMatrix()->get(i, j));
                }
            }
        }
    }


    return result;
}

void slr1::constructFirst(Grammar grammar, std::map<char, std::set<char>>* first) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto rule : grammar.getProductionRules()) {
            char nonterminal = rule.getLeftPart()[0];
            char symbol = rule.getRightPart()[0];
            int previousSize = first->at(nonterminal).size();
            if (!isNonTerminal(grammar, symbol)) {
                first->at(nonterminal).insert(symbol);
            } else {
                for (auto terminal : first->at(symbol)) {
                    first->at(nonterminal).insert(terminal);
                }
            }
            if (first->at(nonterminal).size() != previousSize) {
                changed = true;
            }
        }
    }
}

void slr1::constructFollow(Grammar grammar, std::map<char, std::set<char>>* first, std::map<char, std::set<char>>* follow) {
    follow->at('S').insert('~');
    int j = 0;
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto rule : grammar.getProductionRules()) {
            char nonterminal = rule.getLeftPart()[0];
            for (int i = 0; i < rule.getRightPart().size(); i++) {
                char symbol = rule.getRightPart()[i];

                if (isNonTerminal(grammar, symbol)) {
                    int previousSize = follow->at(symbol).size();
                    if (i < rule.getRightPart().size() - 1) {
                        char gamma = rule.getRightPart()[i + 1];
                        if (!isNonTerminal(grammar, gamma)) {
                            follow->at(symbol).insert(gamma);
                        } else {
                            for (auto terminal : first->at(gamma)) {
                                if (terminal != 'E') {
                                    follow->at(symbol).insert(terminal);
                                }
                            }
                            if (first->at(gamma).count('E')) {
                                for (auto terminal : follow->at(nonterminal)) {
                                    follow->at(symbol).insert(terminal);
                                }
                            }
                        }
                    } else {
                        for (auto terminal : follow->at(nonterminal)) {
                            follow->at(symbol).insert(terminal);
                        }
                    }

                    if (follow->at(symbol).size() != previousSize) {
                        changed = true;
                    }
                }
            }
        }
    }
}

bool slr1::dfs(Automaton* automaton, Matrix<bool>* visitMatrix, int sVertex, std::vector<ProductionRule>* rules) {
	if (visitMatrix->get(sVertex, 0))
        return false;

	visitMatrix->set(sVertex, 0, true);
    for (int i = 0; i < automaton->getStates()->at(sVertex).size(); i++) {
        rules->push_back(automaton->getStates()->at(sVertex)[i]);
    }

	for(int i = 0; i < visitMatrix->getN(); i++) {
        if (automaton->getTransitionMatrix()->get(sVertex, i) != '1') {
            continue;
        }
		if(!visitMatrix->get(i, 0)) {
			bool reached = dfs(automaton, visitMatrix, i, rules);
			if (reached) {
                return true;
            }
		}
	}
	return false;
}

std::vector<ProductionRule> slr1::closure(std::vector<ProductionRule> items, std::vector<ProductionRule> rules, Grammar grammar) {
    std::vector<ProductionRule> result;
    for(auto item : items) {
        int index = 0;
        for (int i = 0; i < item.getRightPart().size(); i++) {
            if (item.getRightPart()[i] == '.') {
                index = i;
                break;
            }
        }

        if (index == item.getRightPart().size() - 1 || !isNonTerminal(grammar, item.getRightPart()[index + 1])) {
            continue;
        }

        for (auto rule : rules) {
            if (rule.getLeftPart()[0] == item.getRightPart()[index + 1]) {
                if (!isContained(result, rule)) {
                    result.push_back(rule);
                }
            }
        }
    }
    return result;
}

bool slr1::isNonTerminal(Grammar grammar, char symbol) {
    return grammar.getNonterminals().count(symbol);
}

bool slr1::isContained(std::vector<ProductionRule> rules, ProductionRule rule) {
    bool contained = false;
    for (auto r : rules) {
        if (r.equals(rule)) {
            contained = true;
            break;
        }
    }

    return contained;
}

int slr1::getPriorityRuleIndex(Grammar grammar, Automaton automaton, int state, bool flag) {
    std::vector<ProductionRule> rules;
    for (int i = 0; i < automaton.getStates()->at(state).size(); i++) {
        std::string rightPart = automaton.getStates()->at(state)[i].getRightPart();
        rightPart.erase(rightPart.find('.'), 1);
        rules.push_back(ProductionRule(automaton.getStates()->at(state)[i].getLeftPart(), rightPart));
    }

    if (rules.size() == 1) {
        // S₀ -> ...
        if (!isNonTerminal(grammar, rules[0].getLeftPart()[0])) {
            return 0;
        } else {
            for (int i = 0; i < grammar.getProductionRules().size(); i++) {
                if (grammar.getProductionRules()[i].getLeftPart() == rules[0].getLeftPart() && grammar.getProductionRules()[i].getRightPart() == rules[0].getRightPart()) {
                    return i + 1;
                }
            }
        }
    }

    if (flag) {
        for (auto rule : rules) {
            // S₀ -> ...
            if (!isNonTerminal(grammar, rule.getLeftPart()[0])) {
                return 0;
            }
        }

        std::set<char> nonterminals;

        for (auto rule : rules) {
            nonterminals.insert(rule.getLeftPart()[0]);
        }

        if (nonterminals.size() == 1) {
            int index = getMaxLengthPhiRuleIndex(automaton.getStates()->at(state), rules[0].getLeftPart()[0]);
            for (int i = 0; i < grammar.getProductionRules().size(); i++) {
                if (grammar.getProductionRules()[i].getLeftPart() == rules[index].getLeftPart() && grammar.getProductionRules()[i].getRightPart() == rules[index].getRightPart()) {
                    return i + 1;
                }
            }
        }

        NonterminalNode root(grammar.getStartSymbol());
        buildNonterminalTree(&root, grammar, rules);

        for (auto ni : nonterminals) {
            bool accept = true;

            for (auto nj : nonterminals) {
                if (ni == nj) {
                    continue;
                }
                if (!dfs2(dfs3(&root, ni, nj), nj, '0')) {
                    accept = false;
                    break;
                }
                if (dfs2(&root, nj, ni)) {
                    accept = false;
                    break;
                }
            }

            if (accept) {
                int index = getMaxLengthPhiRuleIndex(automaton.getStates()->at(state), ni);
                for (int i = 0; i < grammar.getProductionRules().size(); i++) {
                    if (grammar.getProductionRules()[i].getLeftPart() == rules[index].getLeftPart() && grammar.getProductionRules()[i].getRightPart() == rules[index].getRightPart()) {
                        return i + 1;
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < rules.size(); i++) {
            // S₀ -> ...
            if (!isNonTerminal(grammar, rules[i].getLeftPart()[0])) {
                rules.erase(rules.cbegin() + i);
                break;
            }
        }

        std::set<char> nonterminals;

        for (auto rule : rules) {
            nonterminals.insert(rule.getLeftPart()[0]);
        }

        if (nonterminals.size() == 1) {
            int index = getMinLengthPhiRuleIndex(automaton.getStates()->at(state), rules[0].getLeftPart()[0]);
            for (int i = 0; i < grammar.getProductionRules().size(); i++) {
                if (grammar.getProductionRules()[i].getLeftPart() == rules[index].getLeftPart() && grammar.getProductionRules()[i].getRightPart() == rules[index].getRightPart()) {
                    return i + 1;
                }
            }
        }

        NonterminalNode root(grammar.getStartSymbol());
        buildNonterminalTree(&root, grammar, rules);

        for (auto ni : nonterminals) {
            bool accept = true;

            for (auto nj : nonterminals) {
                if (!dfs2(dfs3(&root, ni, nj), nj, '0')) {
                    accept = false;
                    break;
                }
                if (dfs2(&root, nj, ni)) {
                    accept = false;
                    break;
                }
            }

            if (accept) {
                int index = getMinLengthPhiRuleIndex(automaton.getStates()->at(state), ni);
                for (int i = 0; i < grammar.getProductionRules().size(); i++) {
                    if (grammar.getProductionRules()[i].getLeftPart() == rules[index].getLeftPart() && grammar.getProductionRules()[i].getRightPart() == rules[index].getRightPart()) {
                        return i + 1;
                    }
                }
            }
        }

    }

    return -1;
}

int slr1::getMaxLengthPhiRuleIndex(std::vector<ProductionRule> rules, char nonterminal) {
    int index = -1;
    int max = -1;
    for (int i = 0; i < rules.size(); i++) {
        // Ni -> Ф.Y
        // length = |Ф|
        if (rules[i].getLeftPart()[0] == nonterminal) {
            int length = rules[i].getRightPart().find('.');
            if (max < length) {
                max = length;
                index = i;
            }
        }
    }

    return index;
}

int slr1::getMinLengthPhiRuleIndex(std::vector<ProductionRule> rules, char nonterminal) {
    int index = -1;
    int min = 2147483647;
    for (int i = 0; i < rules.size(); i++) {
        // Ni -> Ф.Y
        // length = |Ф|
        if (rules[i].getLeftPart()[0] == nonterminal) {
            int length = rules[i].getRightPart().find('.');
            if (min > length) {
                min = length;
                index = i;
            }
        }
    }

    return index;
}

void slr1::buildNonterminalTree(NonterminalNode* node, Grammar grammar, std::vector<ProductionRule> rules) {
    std::set<char> nonterminals;
    for (auto rule : rules) {
        if (rule.getLeftPart()[0] == node->getNonterminal()) {
            for (auto symbol : rule.getRightPart()) {
                if (isNonTerminal(grammar, symbol)) {
                    nonterminals.insert(symbol);
                }
            }
        }
    }

    for (auto nonterminal : nonterminals) {
        node->add(nonterminal);
    }

    for (int i = 0; i < node->getNodes()->size(); i++) {
        buildNonterminalTree(node->getNodes()->at(i), grammar, rules);
    }
}

bool slr1::dfs2(NonterminalNode* node, char destinationNonterminal, char exclusiveNonterminal) {
    if (node->getNonterminal() == exclusiveNonterminal) {
        return false;
    }

    if (node->getNonterminal() == destinationNonterminal) {
        return true;
    }

	for(int i = 0; i < node->getNodes()->size(); i++) {
        bool reached = dfs2(node->getNodes()->at(i), destinationNonterminal, exclusiveNonterminal);
        if (reached) {
            return true;
        }
	}

	return false;
}

NonterminalNode* slr1::dfs3(NonterminalNode* node, char startNonterminal, char destinationNonterminal) {
    if (node->getNonterminal() == destinationNonterminal) {
        return node;
    }

	for(int i = 0; i < node->getNodes()->size(); i++) {
        NonterminalNode* n = dfs3(node->getNodes()->at(i), startNonterminal, destinationNonterminal);
        if (n != nullptr) {
            return n;
        }
	}

	return nullptr;
}