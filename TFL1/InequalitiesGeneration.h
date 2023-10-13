#ifndef FLT1_INEQUALITIESGENERATION_H
#define FLT1_INEQUALITIESGENERATION_H

std::map<std::pair<int, bool>, std::string> extractCoefficients(Node* node, int degree_w = 0, bool is_x = false, bool is_w = false) {
    std::map<std::pair<int, bool>, std::string> coefficients;

    if (node == nullptr) {
        return coefficients;
    }

    auto* ordinalNode = dynamic_cast<OrdinalNode*>(node);
    if (ordinalNode != nullptr) {
        if (ordinalNode->ordinal.value == "w") {
            if (is_w) {

                coefficients[{degree_w, is_x}] = ordinalNode->ordinal.coefficient;
            } else {
                coefficients[{ordinalNode->ordinal.degree, is_x}] = ordinalNode->ordinal.coefficient;
            }
        } else if (ordinalNode->ordinal.value != "x") {
            coefficients[{degree_w, is_x}] = ordinalNode->ordinal.value;
        }
    }

    is_w = false;
    auto* operationNode = dynamic_cast<OperationNode*>(node);
    if (operationNode != nullptr) {
        if (operationNode->operation == "*") {
            auto *leftOrdinalNode = dynamic_cast<OrdinalNode *>(operationNode->left);
            if (leftOrdinalNode != nullptr && leftOrdinalNode->ordinal.value == "w") {
                degree_w += leftOrdinalNode->ordinal.degree;
                is_w = true;
            }
            auto *rightOrdinalNode = dynamic_cast<OrdinalNode *>(operationNode->right);
            if (rightOrdinalNode != nullptr && rightOrdinalNode->ordinal.value == "x") {
                is_x = true;
            }
        }

        auto leftCoefficients = extractCoefficients(operationNode->left, degree_w, is_x, is_w);
        auto rightCoefficients = extractCoefficients(operationNode->right, degree_w, is_x, is_w);

        for (const auto& leftPair : leftCoefficients) {
            std::pair<int, int> degrees = leftPair.first;
            std::string leftCoefficient = leftPair.second;

            if (coefficients.count(degrees) > 0) {
                std::string& mergedCoefficient = coefficients[degrees];

                mergedCoefficient += "+" + leftCoefficient;

                coefficients[degrees] = mergedCoefficient;
            } else {
                coefficients[degrees] = leftCoefficient;
            }
        }

        for (const auto& rightPair : rightCoefficients) {
            std::pair<int, int> degrees = rightPair.first;
            std::string rightCoefficient = rightPair.second;

            if (coefficients.count(degrees) > 0) {
                std::string& mergedCoefficient = coefficients[degrees];

                mergedCoefficient += "+" + rightCoefficient;

                coefficients[degrees] = mergedCoefficient;
            } else {
                coefficients[degrees] = rightCoefficient;
            }
        }
    }

    return coefficients;
}

std::vector<std::string> splitByAddition(const std::string& expression) {
    std::vector<std::string> terms;
    std::string term;

    for (char c : expression) {
        if (c == '+') {
            terms.push_back(term);
            term.clear();
        } else {
            term += c;
        }
    }

    terms.push_back(term);

    return terms;
}

std::vector<std::string> splitByMultiplication(const std::string& expression) {
    std::vector<std::string> terms;
    std::string term;

    for (char c : expression) {
        if (c == '*') {
            terms.push_back(term);
            term.clear();
        } else {
            term += c;
        }
    }

    terms.push_back(term);

    return terms;
}

std::string generateLogicalExpression(const std::vector<std::pair<int, std::string>>& lhs, const std::vector<std::pair<int, std::string>>& rhs, const std::string& comparator = "") {
    std::string expression;
    int helper = 0;

    if (!comparator.empty()) {
        for (auto lhsIt = lhs.begin(), rhsIt = rhs.begin(); lhsIt != lhs.end() && rhsIt != rhs.end(); ) {
            int degree = lhsIt->first;
            std::string lhsCoefficient = lhsIt->second;

            if (rhsIt->first == degree) {
                std::string rhsCoefficient = rhsIt->second;
                std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);
                std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);
                std::string lhsNewCoefficient;
                std::string rhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : lhsTerms) {
                    if (addHelper != lhsTerms.size() - 1) {
                        lhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (lhsTerms.size() != 1) {
                        lhsNewCoefficient += " ";
                    }
                    std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                    if (lhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                            if (multHelper != lhsMultiplicationTerms.size() - 1) {
                                lhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                lhsNewCoefficient += multiplicationTerm;
                                for (int i = 0; i < multHelper; i++) {
                                    lhsNewCoefficient += ")";
                                }
                            }
                        }
                    } else {
                        lhsNewCoefficient += lhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == lhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        lhsNewCoefficient += ")";
                    }
                }


                addHelper = 0;
                for (const auto& term : rhsTerms) {
                    if (addHelper != rhsTerms.size() - 1) {
                        rhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (lhsTerms.size() != 1) {
                        rhsNewCoefficient += " ";
                    }
                    std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                    if (rhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                            if (multHelper != rhsMultiplicationTerms.size() - 1) {
                                rhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                rhsNewCoefficient += multiplicationTerm;
                                if (multHelper != 0) {
                                    for (int i = 0; i < multHelper; i++) {
                                        rhsNewCoefficient += ")";
                                    }
                                }
                            }
                        }
                    } else {
                        rhsNewCoefficient += rhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == rhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        rhsNewCoefficient += ")";
                    }
                }

                if (lhs.size() > 1 && lhsIt != lhs.end() - 1) {
                    expression += "(or (" + comparator + " " + lhsNewCoefficient + " " + rhsNewCoefficient + ") (and (= " + lhsNewCoefficient + " " + rhsNewCoefficient + ")";
                    helper += 2;
                } else {
                    expression += "(" + comparator + " " + lhsNewCoefficient + " " + rhsNewCoefficient + ")";
                }

                ++lhsIt;
                ++rhsIt;
            } else if (rhsIt->first < degree) {
                std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);
                std::string lhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : lhsTerms) {
                    if (addHelper != lhsTerms.size() - 1) {
                        lhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (lhsTerms.size() != 1) {
                        lhsNewCoefficient += " ";
                    }
                    std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                    if (lhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                            if (multHelper != lhsMultiplicationTerms.size() - 1) {
                                lhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                lhsNewCoefficient += multiplicationTerm;
                                for (int i = 0; i < multHelper; i++) {
                                    lhsNewCoefficient += ")";
                                }
                            }
                        }
                    } else {
                        lhsNewCoefficient += lhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == lhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        lhsNewCoefficient += ")";
                    }
                }

                if (lhs.size() > 1 && lhsIt != lhs.end() - 1) {
                    expression += "(or (" + comparator + " " + lhsNewCoefficient + " 0) (and (" + comparator + " " + lhsNewCoefficient + " 0) ";
                    helper += 2;
                    ++lhsIt;
                } else {
                    expression += "(" + comparator + " " + lhsNewCoefficient + " 0) ";
                    ++lhsIt;
                }
            } else {
                std::string rhsCoefficient = rhsIt->second;
                std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);
                std::string rhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : rhsTerms) {
                    if (addHelper != rhsTerms.size() - 1) {
                        rhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (rhsTerms.size() != 1) {
                        rhsNewCoefficient += " ";
                    }
                    std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                    if (rhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                            if (multHelper != rhsMultiplicationTerms.size() - 1) {
                                rhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                rhsNewCoefficient += multiplicationTerm;
                                if (multHelper != 0) {
                                    for (int i = 0; i < multHelper; i++) {
                                        rhsNewCoefficient += ")";
                                    }
                                }
                            }
                        }
                    } else {
                        rhsNewCoefficient += rhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == rhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        rhsNewCoefficient += ")";
                    }
                }

                if (rhs.size() > 1 && rhsIt != rhs.end() - 1) {
                    expression += "(or (" + comparator + " 0 " + rhsNewCoefficient + ") (and (= 0 " + rhsNewCoefficient + ") ";
                    helper += 2;
                    ++rhsIt;
                } else {
                    expression += "(" + comparator + " 0 " + rhsNewCoefficient + ")";
                    ++rhsIt;
                }
            }
        }

        for (int i = 0; i < helper; i++) {
            expression += ")";
        }
    } else {
        if (!lhs.empty() || !rhs.empty()) {
            expression += "(and ";
        }
        for (auto lhsIt = lhs.begin(), rhsIt = rhs.begin(); lhsIt != lhs.end() && rhsIt != rhs.end(); ) {
            int degree = lhsIt->first;
            std::string lhsCoefficient = lhsIt->second;

            if (rhsIt->first == degree) {
                std::string rhsCoefficient = rhsIt->second;
                std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);
                std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);
                std::string lhsNewCoefficient;
                std::string rhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : lhsTerms) {
                    if (addHelper != lhsTerms.size() - 1) {
                        lhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (lhsTerms.size() != 1) {
                        lhsNewCoefficient += " ";
                    }
                    std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                    if (lhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                            if (multHelper != lhsMultiplicationTerms.size() - 1) {
                                lhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                lhsNewCoefficient += multiplicationTerm;
                                for (int i = 0; i < multHelper; i++) {
                                    lhsNewCoefficient += ")";
                                }
                            }
                        }
                    } else {
                        lhsNewCoefficient += lhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == lhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        lhsNewCoefficient += ")";
                    }
                }


                addHelper = 0;
                for (const auto& term : rhsTerms) {
                    if (addHelper != rhsTerms.size() - 1) {
                        rhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (rhsTerms.size() != 1) {
                        rhsNewCoefficient += " ";
                    }
                    std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                    if (rhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                            if (multHelper != rhsMultiplicationTerms.size() - 1) {
                                rhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                rhsNewCoefficient += multiplicationTerm;
                                if (multHelper != 0) {
                                    for (int i = 0; i < multHelper; i++) {
                                        rhsNewCoefficient += ")";
                                    }
                                }
                            }
                        }
                    } else {
                        rhsNewCoefficient += rhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == rhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        rhsNewCoefficient += ")";
                    }
                }

                expression += "(= " + lhsNewCoefficient + " " + rhsNewCoefficient + ")";

                lhsIt++;
                rhsIt++;
            } else if (rhsIt->first < degree) {
                std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);
                std::string lhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : lhsTerms) {
                    if (addHelper != lhsTerms.size() - 1) {
                        lhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (lhsTerms.size() != 1) {
                        lhsNewCoefficient += " ";
                    }
                    std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                    if (lhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                            if (multHelper != lhsMultiplicationTerms.size() - 1) {
                                lhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                lhsNewCoefficient += multiplicationTerm;
                                for (int i = 0; i < multHelper; i++) {
                                    lhsNewCoefficient += ")";
                                }
                            }
                        }
                    } else {
                        lhsNewCoefficient += lhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == lhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        lhsNewCoefficient += ")";
                    }
                }
                expression += "(= " + lhsNewCoefficient + " 0)";
                ++lhsIt;
            } else {
                std::string rhsCoefficient = rhsIt->second;
                std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);
                std::string rhsNewCoefficient;


                int addHelper = 0;
                for (const auto& term : rhsTerms) {
                    if (addHelper != rhsTerms.size() - 1) {
                        rhsNewCoefficient += "(+ ";
                        addHelper++;
                    } else if (rhsTerms.size() != 1) {
                        rhsNewCoefficient += " ";
                    }
                    std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                    if (rhsMultiplicationTerms.size() > 1) {
                        int multHelper = 0;
                        for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                            if (multHelper != rhsMultiplicationTerms.size() - 1) {
                                rhsNewCoefficient += "(* " + multiplicationTerm + " ";
                                multHelper++;
                            } else {
                                rhsNewCoefficient += multiplicationTerm;
                                if (multHelper != 0) {
                                    for (int i = 0; i < multHelper; i++) {
                                        rhsNewCoefficient += ")";
                                    }
                                }
                            }
                        }
                    } else {
                        rhsNewCoefficient += rhsMultiplicationTerms[0];
                    }
                }

                if (addHelper == rhsTerms.size() - 1) {
                    for (int i = 0; i < addHelper; i++) {
                        rhsNewCoefficient += ")";
                    }
                }

                expression += "(= 0 " + rhsNewCoefficient + ")";
                ++rhsIt;
            }
        }

        expression += ")";
    }

    return expression;
}

std::string generateInequalities(const std::map<std::pair<int, bool>, std::string>& lhs, const std::map<std::pair<int, bool>, std::string>& rhs) {
    std::vector<std::pair<int, std::string>> lhsWithX, lhsWithoutX, rhsWithX, rhsWithoutX;

    for (const auto& pair : lhs) {
        if (pair.first.second) {
            lhsWithX.emplace_back(pair.first.first, pair.second);
        } else {
            lhsWithoutX.emplace_back(pair.first.first, pair.second);
        }
    }

    for (const auto& pair : rhs) {
        if (pair.first.second) {
            rhsWithX.emplace_back(pair.first.first, pair.second);
        } else {
            rhsWithoutX.emplace_back(pair.first.first, pair.second);
        }
    }

    std::sort(lhsWithX.rbegin(), lhsWithX.rend());
    std::sort(rhsWithX.rbegin(), rhsWithX.rend());
    std::sort(lhsWithoutX.rbegin(), lhsWithoutX.rend());
    std::sort(rhsWithoutX.rbegin(), rhsWithoutX.rend());

    std::string expression = "(or (and " + generateLogicalExpression(lhsWithX, rhsWithX, ">") + generateLogicalExpression(lhsWithoutX, rhsWithoutX, ">=") + ") " +
                             "(and " + generateLogicalExpression(lhsWithX, rhsWithX) + generateLogicalExpression(lhsWithoutX, rhsWithoutX, ">") + "))";

    return expression;
}

#endif //FLT1_INEQUALITIESGENERATION_H
