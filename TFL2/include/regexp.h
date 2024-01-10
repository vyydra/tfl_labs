#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>

namespace regexp {

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

    class Automaton {
        private:
        Matrix<int> initialStateMatrix;
        Matrix<std::string> transitionMatrix;
        Matrix<int> finalStateMatrix;
        int statesNumber;

        public:
        Automaton(int statesNumber);

        Matrix<int>* getInitialStateMatrix();

        Matrix<std::string>* getTransitionMatrix();

        Matrix<int>* getFinalStateMatrix();

        int getStatesNumber();

        std::string toString();
        
        static Automaton getUnion(Automaton& a1, Automaton& a2);
        
        static Automaton getConcatenation(Automaton& a1, Automaton& a2);

        static Automaton getKleeneClosure(Automaton& a);

        static Automaton getIntersection(Automaton& a1, Automaton& a2);

        //std::vector<std::string> findPaths(Automaton& automaton, std::stack<int>& dfsStack);

        /*void writeWordsToFile(const std::string& filename);*/
    };

    /**
     * The lexer that tokenizes some expression.
    */
    class Lexer {
    public:
        class Token;
        std::vector<Token> tokenize(std::string source, std::set<char> alphabet);
    };

    class Lexer::Token {
    private:
        std::string data;
        int type;

    public:
        static const int kNone = 0;
        static const int kLetter = 1;
        static const int kOpenParenthesis = 2;
        static const int kClosedParenthesis = 4;
        static const int kKleeneStar = 8;
        static const int kConcatenation = 16;
        static const int kIntersection = 32;
        static const int kAlternative = 64;
        
        Token(std::string data, int type);

        std::string getData();

        int getType();

        bool isOperator();

        bool hasHigherPriority(Token token);
    };

    /**
     * Replaces all strings with.
     * 
     * @param source the source string.
     * @param toReplace the string to be replaced.
     * @param replaceWith the string to replace with.
    */
    void replaceAll(std::string& source, std::string const& toReplace, std::string const& replaceWith);

    /**
     * Replaces all dots in a regular expression with constructs consisting of alternatives.
     * 
     * @param source the source regular expression.
     * @param alphabet the alphabet of the regular expression.
    */
    void replaceDots(std::string& source, std::set<char> alphabet);

    /**
     * Gets the alphabet of a regular expression.
     * 
     * @param source the source regular expression.
     * @return the alphabet of the regular expression.
    */
    std::set<char> getAlphabetOf(std::string source);

    /**
     * Converts a regular expression containing lookaheads to a regular expression containing intersections
     * in the range from `start` position to `end` position.
     * 
     * @param source the source regular expression.
     * @param start the start position.
     * @param end the end position.
     * @return the result regular expression.
    */
    std::string convertLookaheadsToIntersections(std::string& source, int start, int end);

    /**
     * Converts a regular expression containing lookaheads to a regular expression containing intersections.
     * 
     * @param source the source regular expression.
     * @return the result regular expression.
    */
    std::string convertLookaheadsToIntersections(std::string& source);

    /**
     * Removes the specified string from a source string.
     * 
     * @param source the source string.
     * @param to_remove the specified string to be removed from the source string.
    */
    void removeFrom(std::string& source, std::string const& toRemove);

    /**
     * Converts a vector of tokens in infix form to a vector of tokens in posfix form.
     * 
     * @param tokens the source vector of tokens in infix form.
     * @return the result vector in postfix form.
    */
    std::vector<Lexer::Token> convertToPostfixForm(std::vector<Lexer::Token>& tokens);

    /**
     * Builds a Glushkov automaton based on a `symbol`.
     * 
     * @param symbol the symbol.
     * @return the Glushkov automaton.
    */
    Automaton buildGlushkovAutomaton(std::string symbol);

    /**
     * Converts an automaton to an academic regular expression.
     * 
     * @param automaton the automaton to be converted.
     * @return the academic regular expression.
    */
    std::string convertAutomatonToRegex(Automaton automaton);

    /**
     * Converts a regular expression containing lookaheads to an academic regular expression.
     * 
     * @param source the source regular expression to be converted.
     * @return the academic regular expression.
    */
    std::string convertToAcademicRegex(std::string& source, Automaton* automaton);

    /**
     * Performs depth-first search from a start vertex to destination vertices.
     * 
     * @param transitionMatrix the transition matrix.
     * @param visitMatrix the visit matrix.
     * @param sVertex the start vertex.
     * @param dVertices the destination vertices.
     * @return whether at least one destination vertex has been visited.
    */
    bool dfs(Matrix<std::string>* transitionMatrix, Matrix<bool>* visitMatrix, int sVertex, std::vector<int>* dVertices);

    /**
    * Generates the given amount of words accepted by the given automaton.
    * 
    * @param automaton the automaton accepting generated words.
    * @param quantity the number of words to be generated.
    */
    std::vector<std::string> generateWords(Automaton& automaton, int quantity);

    /**
     * Generates random regular expression.
     * 
     * @param depth the depth of regular expression.
     * @param currentStarHeight the current star height of regular expression.
     * @param currentNumLookaheads the current lookaheads amount in regular expression.
     * @param nestedLookahead the presence of nested lookaheads in regular expression.
     * @param lookaheadInAlternative the presence of lookaheads inside alternative operation.
     * @param currentNumLookbehinds the current lookbehinds amount in regular expression.
     * @param prefixBinary the presence of concatenation prefix before binary operation in regular expression.
     * @param unaryBinaryInLookbehindthe presence of unary or binary operations in lookbehind.
    */
    std::string generateRandomRegex(int depth, int& currentStarHeight, int& currentNumLookaheads, bool nestedLookahead, bool& lookaheadInAlternative, int& currentNumLookbehinds, bool& prefixBinary, bool& unaryBinaryInLookbehind);

    /**
     * Generates random regular expression.
    */
    std::string generateRandomRegex();

    /**
     * Tests regular expressions (generated random one and academic version of it) and words for matches.
     * 
     * @param outputFileName the name of the output file.
     * @param numExpressions the amount of regular expressions to be tested.
     * @param numWords the amount of words to be tested for each regular expressions pair.
    */
    void testRegularExpressions(const std::string& outputFileName, int numExpressions, int numWords);
}