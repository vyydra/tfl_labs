#include <string>
#include <vector>
#include <stdexcept>

enum class TokenType { SYMBOL, BINARY_OP, UNARY_OP, PAREN_OPEN, PAREN_CLOSE, LOOKAHEAD, DOT, END_OF_LINE };

struct Token {
    TokenType type;
    char value;
};

std::vector<Token> tokenise(const std::string& input) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        switch (c) {
            case '|':
                tokens.push_back({TokenType::BINARY_OP, c});
                break;
            case '*':
                tokens.push_back({TokenType::UNARY_OP, c});
                break;
            case '(':
                tokens.push_back({TokenType::PAREN_OPEN, c});
                break;
            case ')':
                tokens.push_back({TokenType::PAREN_CLOSE, c});
                break;
            case '^':
                continue;
            case '$':
                tokens.push_back({TokenType::END_OF_LINE, c});
                break;
            case '?':
                if (i < input.size() - 1 && input[i + 1] == '=') {
                    tokens.push_back({TokenType::LOOKAHEAD, c});
                    ++i;
                }
                break;
            case '.':
                tokens.push_back({TokenType::DOT, c});
                break;
            default:
                tokens.push_back({TokenType::SYMBOL, c});
                break;
        }
    }
    return tokens;
}

struct Node {
    Token token;
    Node* left;
    Node* right;
};

Node* parseRegex(const std::vector<Token>& tokens, size_t& index);
Node* parseParenthesizedRegex(const std::vector<Token>& tokens, size_t& index);

Node* parseSymbol(const std::vector<Token>& tokens, size_t& index) {
    Node* node = new Node();
    node->token = tokens[index++];
    return node;
}

Node* parseDot(const std::vector<Token>& tokens, size_t& index) {
    Node* node = new Node();
    node->token = tokens[index++];
    return node;
}

Node* parseUnary(const std::vector<Token>& tokens, size_t& index) {
    Node* node;
    if (tokens[index].type == TokenType::DOT) {
        node = parseDot(tokens, index);
    }
    if (tokens[index].type == TokenType::SYMBOL) {
        node = parseSymbol(tokens, index);
    }
    if (tokens[index].type == TokenType::PAREN_OPEN) {
        node = parseParenthesizedRegex(tokens, index);
    }
    if (index < tokens.size() && tokens[index].type == TokenType::UNARY_OP) {
        node->token = tokens[index++];
        node->right = parseUnary(tokens, index);
    }
    return node;
}

Node* parseLookahead(const std::vector<Token>& tokens, size_t& index) {
    Node* node = parseUnary(tokens, index);
    if (node == nullptr) {
        node= new Node();
    }
    if (index < tokens.size() && tokens[index].type == TokenType::LOOKAHEAD) {
        node->token = tokens[index++];
        node->right = parseRegex(tokens, index);
        if (index < tokens.size() && tokens[index].type == TokenType::END_OF_LINE) {
            index++;
        } else {
            throw std::runtime_error("Lookahead expression must end in '$'");
        }
    }
    return node;
}

Node* parseParenthesizedRegex(const std::vector<Token>& tokens, size_t& index) {
    if (tokens[index].type == TokenType::PAREN_OPEN) {
        index++;
        Node* node = parseRegex(tokens, index);
        if (index < tokens.size() && tokens[index].type == TokenType::PAREN_CLOSE) {
            index++;
            return node;
        } else {
            throw std::runtime_error("Unbalanced parentheses");
        }
    } else {
        throw std::runtime_error("Expected '('");
    }
}

Node* parseBinary(const std::vector<Token>& tokens, size_t& index) {
    Node* node = parseLookahead(tokens, index);
    if (index < tokens.size() && tokens[index].type == TokenType::BINARY_OP) {
        node->token = tokens[index++];
        node->left = parseBinary(tokens, index);
    }
    return node;
}

Node* parseRegex(const std::vector<Token>& tokens, size_t& index) {
    Node* node = parseBinary(tokens, index);
    if (index < tokens.size()) {
        TokenType nextType = tokens[index].type;
        if (nextType != TokenType::PAREN_CLOSE && nextType != TokenType::END_OF_LINE) {
            node->right = parseRegex(tokens, index);
        }
    }
    return node;
}

Node* parse(const std::string& input) {
    std::vector<Token> tokens = tokenise(input);
    size_t index = 0;
    Node* rootNode = parseRegex(tokens, index);
    if (index < tokens.size() && tokens[index].type != TokenType::END_OF_LINE) {
        throw std::runtime_error("Unexpected tokens after end of line");
    }
    return rootNode;
}

int main() {
    Node* node = parse("^ab((?=.*(aa|b)$)(a|ab)*|(?=.*(ba|aa)$)(b|ba)*)$");
    return 0;
}
