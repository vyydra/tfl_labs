#pragma once

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <map>

namespace slr1 {

    template<typename T>
    class Matrix {
    private:
        std::vector<std::vector<T>> data;
        int n, m;

    public:
        Matrix() {
            n = 1;
            m = 1;
            data.push_back(std::vector<T>(1));
        }

        Matrix(int n, int m) {
            this->n = n;
            this->m = m;
            for (int i = 0; i < n; i++) {
                data.push_back(std::vector<T>(m));
            }
        }

        T get(int x, int y) {
            return data.at(x).at(y);
        }

        void set(int x, int y, T value) {
            data.at(x).at(y) = value;
        }

        int getN() {
            return n;
        }

        int getM() {
            return m;
        }

        void setAll(T value) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    set(i, j, value);
                }
            }
        }

        std::vector<std::vector<T>>* getData() {
            return &data;
        }

        std::string toString() {
            std::stringstream ss;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    ss << get(i, j) << " ";
                }
                ss << std::endl;
            }
            return ss.str();
        }
    };

    class ProductionRule {
        private:
        std::string leftPart;
        std::string rightPart;
        public:
        ProductionRule(std::string leftPart, std::string rightPart);
        std::string getLeftPart();
        std::string getRightPart();
        std::string toString();
        bool equals(ProductionRule rule);
    };

    /**
     * https://neerc.ifmo.ru/wiki/index.php?title=Формальные_грамматики
    */
    class Grammar {
        private:
        std::set<char> terminals;
        std::set<char> nonterminals;
        char startSymbol;
        std::vector<ProductionRule> productionRules;
        public:
        std::set<char> getTerminals();
        std::set<char> getNonterminals();
        char getStartSymbol();
        std::vector<ProductionRule> getProductionRules();
        void setTerminals(std::set<char> terminals);
        void setNonterminals(std::set<char> nonterminals);
        void setStartSymbol(char startSymbol);
        void setProductionRules(std::vector<ProductionRule> productionRules);
    };

    class Automaton {
        private:
        std::vector<std::vector<ProductionRule>> states;
        int initialState = 0;
        std::set<int> finalStates;
        Matrix<char> transitionMatrix;
        int statesNumber = 0;
        public:
        void add(std::vector<ProductionRule> state);
        void setInitialState(int state);
        int getInitialState();
        void addFinalState(int state);
        void removeFinalState(int state);
        std::vector<std::vector<ProductionRule>>* getStates();
        Matrix<char>* getTransitionMatrix();
        std::set<int> getFinalStates();
        std::string toString();
    };

    /**
     * https://neerc.ifmo.ru/wiki/index.php?title=LR(0)-разбор
    */
    void parse(Grammar grammar, std::string word, bool keySeniority);

    std::vector<ProductionRule> closure(std::vector<ProductionRule> items, std::vector<ProductionRule> rules, Grammar grammar);

    bool isNonTerminal(Grammar grammar, char symbol);

    bool isContained(std::vector<ProductionRule> rules, ProductionRule rule);

    bool dfs(Automaton* automaton, Matrix<bool>* visitMatrix, int sVertex, std::vector<ProductionRule>* rules);

    Automaton buildLR0Automaton(Grammar grammar, std::vector<ProductionRule> g);

    /**
     * https://neerc.ifmo.ru/wiki/index.php?title=Построение_FIRST_и_FOLLOW
     * https://ru.wikipedia.org/wiki/SLR(1)
    */
    void constructFirst(Grammar grammar, std::map<char, std::set<char>>* first);

    /**
     * https://neerc.ifmo.ru/wiki/index.php?title=Построение_FIRST_и_FOLLOW
     * https://ru.wikipedia.org/wiki/SLR(1)
    */
    void constructFollow(Grammar grammar, std::map<char, std::set<char>>* first, std::map<char, std::set<char>>* follow);

    /**
     * http://gas-teach.narod.ru/au/tfl/tfl13.pdf
    */
    std::vector<std::map<char, std::pair<std::string, int>>> buildSLR1Table(Grammar grammar, bool* slrProperty);

    class NonterminalNode {
        private:
        std::vector<NonterminalNode*> nodes;
        std::set<char> parents;
        char nonterminal;
        public:
        NonterminalNode(char nonterminal) {
            this->nonterminal = nonterminal;
        }
        ~NonterminalNode() {
            for (auto node : nodes) {
                delete node;
            }
        }
        void add(char nonterminal) {
            if (parents.count(nonterminal) == 0) {
                NonterminalNode* node = new NonterminalNode(nonterminal);
                std::set<char> p = parents;
                p.insert(nonterminal);
                node->setParents(p);
                nodes.push_back(node);
            }
        }

        std::vector<NonterminalNode*>* getNodes() {
            return &nodes;
        }

        std::set<char>* getParents() {
            return &parents;
        }

        void setParents(std::set<char> parents) {
            this->parents = parents;
        }

        char getNonterminal() {
            return nonterminal;
        }

        void setNonterminal(char nonterminal) {
            this->nonterminal = nonterminal;
        }
    };

    /**
     * @param grammar непополненная грамматика
     * @param automaton
     * @param state
     * @param flag true, если приоритет от старшему к младшему, false - наоборот
    */
    int getPriorityRuleIndex(Grammar grammar, Automaton automaton, int state, bool flag);

    int getMaxLengthPhiRuleIndex(std::vector<ProductionRule> rules, char nonterminal);

    int getMinLengthPhiRuleIndex(std::vector<ProductionRule> rules, char nonterminal);

    void buildNonterminalTree(NonterminalNode* node, Grammar grammar, std::vector<ProductionRule> rules);

    bool dfs2(NonterminalNode* node, char destinationNonterminal, char exclusiveNonterminal);

    NonterminalNode* dfs3(NonterminalNode* root, char startNonterminal, char destinationNonterminal);
}