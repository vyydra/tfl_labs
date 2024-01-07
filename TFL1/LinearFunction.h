#ifndef FLT1_LINEARFUNCTION_H
#define FLT1_LINEARFUNCTION_H

class Ordinal {
public:
    bool isLimit;
    std::string value;
    int degree;
    std::string coefficient;

    Ordinal() : isLimit(false), value("0"), degree(1) {}
    explicit Ordinal(std::string value) : isLimit(false), value(std::move(value)), degree(1) {}
    explicit Ordinal(bool isLimit, std::string value, int degree = 1, std::string coefficient = "1") : isLimit(isLimit), value(std::move(value)), degree(degree), coefficient(std::move(coefficient)) {}

    Ordinal add(const Ordinal& other) const {
        if (isLimit && other.isLimit && value == other.value && degree == other.degree) {
            return Ordinal(true, value, degree, coefficient + "+" + other.coefficient);
        } else if (isLimit && other.isLimit) {
            return Ordinal(true, value + " + " + other.value);
        } else if (other.isLimit) {
            return Ordinal(true, other.value);
        } else {
            return Ordinal(value + " + " + other.value);
        }
    }

    Ordinal multiply(const Ordinal& other) const {
        if (isLimit && other.isLimit && value == other.value) {
            if (coefficient != "1") {
                return Ordinal(true, value, degree + other.degree, other.coefficient);
            } else {
                return Ordinal(true, value, degree + other.degree);
            }
        } else if (isLimit) {
            return Ordinal(true, value, degree, coefficient + "*" + other.value);
        } else if (other.isLimit) {
            return Ordinal(true, other.value);
        } else {
            return Ordinal(value + " * " + other.value);
        }
    }
};

class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
    virtual Node* simplify() const = 0;
    virtual Node* clone() const = 0;
};

class OrdinalNode : public Node {
public:
    Ordinal ordinal;

    explicit OrdinalNode(Ordinal ordinal) : ordinal(std::move(ordinal)) {}

    std::string to_string() const override {
        std::ostringstream oss;
        if (ordinal.degree > 1) {
            oss << ordinal.value << "^" << ordinal.degree;
        } else {
            oss << ordinal.value;
        }
        if (!ordinal.coefficient.empty()) {
            oss << " * " << "(" + ordinal.coefficient + ")";
        }

        return oss.str();
    }

    Node* simplify() const override {
        return new OrdinalNode(ordinal);
    }

    Node* clone() const override {
        return new OrdinalNode(ordinal);
    }
};

class OperationNode : public Node {
public:
    std::string operation;
    Node* left;
    Node* right;
    OperationNode(std::string operation, Node* left, Node* right)
            : operation(std::move(operation)),
              left(left),
              right(right) {}
    ~OperationNode() override {
        if (left) {
            delete left;
            left = nullptr;
        }
        if (right) {
            delete right;
            right = nullptr;
        }
    }


    std::string to_string() const override {
        return "(" + left->to_string() + " " + operation + " " + right->to_string() + ")";
    }
    Node* simplify() const override {
        Node* simplifiedLeft = left->simplify();
        Node* simplifiedRight = right->simplify();

        auto operationLeft = dynamic_cast<OperationNode*>(simplifiedLeft);
        auto operationRight = dynamic_cast<OperationNode*>(simplifiedRight);
        if (operation == "*" && operationLeft && operationLeft->operation == "+" &&
            operationRight && operationRight->operation == "+") {

            auto item1 = (new OperationNode("*", operationLeft->left, operationRight->left))->simplify();
            auto item2 = (new OperationNode("*", operationLeft->left, operationRight->right))->simplify();
            auto h = operationRight->clone();
            while (!dynamic_cast<OrdinalNode*>(dynamic_cast<OperationNode*>(h)->left)) {
                h = dynamic_cast<OperationNode*>(h)->left;
            }
            if (dynamic_cast<OrdinalNode*>(operationLeft->right) && !dynamic_cast<OrdinalNode*>(operationLeft->right)->ordinal.isLimit
                && dynamic_cast<OperationNode*>(h)->left && dynamic_cast<OrdinalNode*>(dynamic_cast<OperationNode*>(h)->left)->ordinal.isLimit) {
                auto item3 = (new OperationNode("+", dynamic_cast<OperationNode*>(dynamic_cast<OperationNode*>(item1)->left)->left, new OrdinalNode(Ordinal(dynamic_cast<OrdinalNode*>(operationLeft->right)->ordinal.value))))->simplify();
                dynamic_cast<OperationNode*>(dynamic_cast<OperationNode*>(item1)->left)->left = item3;
                return (new OperationNode("+", item1, item2))->simplify();
            } else {
                auto item3 = (new OperationNode("*", operationLeft->right, operationRight->left))->simplify();
                auto item4 = (new OperationNode("*", operationLeft->right, operationRight->right))->simplify();
                auto item5 = (new OperationNode("+", item1, item2))->simplify();
                auto item6 = (new OperationNode("+", item3, item4))->simplify();
                return (new OperationNode("+", item5, item6))->simplify();
            }
        }

        if (operation == "+" && operationLeft && operationLeft->operation == "+" &&
            dynamic_cast<OrdinalNode*>(operationLeft->right) && !dynamic_cast<OrdinalNode*>(operationLeft->right)->ordinal.isLimit &&
            ((operationRight && dynamic_cast<OrdinalNode*>(operationRight->left) && dynamic_cast<OrdinalNode*>(operationRight->left)->ordinal.isLimit) ||
             (dynamic_cast<OrdinalNode*>(simplifiedRight) && dynamic_cast<OrdinalNode*>(simplifiedRight)->ordinal.isLimit))) {
            if (operationRight) {
                return (new OperationNode("+", operationLeft->left, new OperationNode(operationRight->operation, operationRight->left, operationRight->right)))->simplify();
            } else {
                return (new OperationNode("+", operationLeft->left, new OrdinalNode(dynamic_cast<OrdinalNode*>(simplifiedRight)->ordinal)))->simplify();
            }
        }

        if (operation == "*" && operationRight && operationRight->operation == "*"
            && dynamic_cast<OperationNode*>(operationRight->left)
            && dynamic_cast<OperationNode*>(operationRight->left)->operation == "+") {
            Node* b = dynamic_cast<OperationNode*>(operationRight->left);
            return new OperationNode("*", (new OperationNode("*", simplifiedLeft, b))->simplify(), operationRight->right);
        }

        if (operation == "*" && dynamic_cast<OperationNode*>(simplifiedRight)
            && dynamic_cast<OperationNode*>(simplifiedRight)->operation == "+") {
            return (new OperationNode("+", (new OperationNode("*", simplifiedLeft, dynamic_cast<OperationNode*>(simplifiedRight)->left))->simplify(), (new OperationNode("*", simplifiedLeft, dynamic_cast<OperationNode*>(simplifiedRight)->right))->simplify()))->simplify();
        }

        auto ordinalLeft = dynamic_cast<OrdinalNode*>(simplifiedLeft);
        auto ordinalRight = dynamic_cast<OrdinalNode*>(simplifiedRight);
        if (ordinalLeft && ordinalRight) {
            if (operation == "+") {
                if ((ordinalLeft->ordinal.isLimit && !ordinalRight->ordinal.isLimit)
                    || (ordinalLeft->ordinal.isLimit && ordinalRight->ordinal.isLimit
                        && ordinalLeft->ordinal.degree != ordinalRight->ordinal.degree)) {
                    return new OperationNode("+", simplifiedLeft, simplifiedRight);
                }
                return new OrdinalNode(ordinalLeft->ordinal.add(ordinalRight->ordinal));
            } else if (operation == "*") {
                return new OrdinalNode(ordinalLeft->ordinal.multiply(ordinalRight->ordinal));
            }
        }

        return new OperationNode(operation, simplifiedLeft, simplifiedRight);
    }

    Node* clone() const override {
        return new OperationNode(operation, left->clone(), right->clone());
    }

};

class LinearFunction {
public:
    Node* root;

    explicit LinearFunction(Node* root) : root(root) {}
    ~LinearFunction() { delete root; }

    std::string to_string() const {
        return root->to_string();
    }

    LinearFunction* simplify() const {
        return new LinearFunction(root->simplify());
    }
};

#endif //FLT1_LINEARFUNCTION_H
