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

std::vector<std::string> generateInequalities(const std::map<std::pair<int, bool>, std::string>& lhs, const std::map<std::pair<int, bool>, std::string>& rhs) {
    std::vector<std::string> result = {};
    for (const auto & lhsPair : lhs) {
        std::pair<int, bool> degrees = lhsPair.first;
        std::string lhsCoefficient = lhsPair.second;

        if (rhs.count(degrees) > 0) {
            const std::string& rhsCoefficient = rhs.at(degrees);
            std::string inequality;

            if (!degrees.second) {
                inequality += "(>= ";
            } else {
                inequality += "(> ";
            }


            std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);
            std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);

            int addHelper = 0;
            for (const auto& term : lhsTerms) {
                if (addHelper != lhsTerms.size() - 1) {
                    inequality += "(+ ";
                    addHelper++;
                } else {
                    inequality += " ";
                }
                std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                if (lhsMultiplicationTerms.size() > 1) {
                    int multHelper = 0;
                    for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                        if (multHelper != lhsMultiplicationTerms.size() - 1) {
                            inequality += "(* " + multiplicationTerm + " ";
                            multHelper++;
                        } else {
                            inequality += multiplicationTerm;
                            for (int i = 0; i < multHelper; i++) {
                                inequality += ")";
                            }
                        }
                    }
                } else {
                    inequality += lhsMultiplicationTerms[0];
                }
            }

            if (addHelper == lhsTerms.size() - 1) {
                for (int i = 0; i < addHelper; i++) {
                    inequality += ")";
                }
            }

            inequality += " ";


            addHelper = 0;
            for (const auto& term : rhsTerms) {
                if (addHelper != rhsTerms.size() - 1) {
                    inequality += "(+ ";
                    addHelper++;
                } else {
                    inequality += " ";
                }
                std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                if (rhsMultiplicationTerms.size() > 1) {
                    int helper = 0;
                    for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                        if (helper != rhsMultiplicationTerms.size() - 1) {
                            inequality += "(* " + multiplicationTerm + " ";
                            helper++;
                        } else {
                            inequality += multiplicationTerm;
                            if (helper != 0) {
                                for (int i = 0; i < helper; i++) {
                                    inequality += ")";
                                }
                            }
                        }
                    }
                } else {
                    inequality += rhsMultiplicationTerms[0];
                }
            }

            if (addHelper == rhsTerms.size() - 1) {
                for (int i = 0; i < addHelper; i++) {
                    inequality += ")";
                }
            }

            inequality += ")";
            result.emplace_back(inequality);
        } else {
            std::string inequality;

            if (!degrees.second) {
                inequality += "(>= ";
            } else {
                inequality += "(> ";
            }


            std::vector<std::string> lhsTerms = splitByAddition(lhsCoefficient);

            int addHelper = 0;
            for (const auto& term : lhsTerms) {
                if (addHelper != lhsTerms.size() - 1) {
                    inequality += "(+ ";
                    addHelper++;
                } else {
                    inequality += " ";
                }
                std::vector<std::string> lhsMultiplicationTerms = splitByMultiplication(term);
                if (lhsMultiplicationTerms.size() > 1) {
                    int multHelper = 0;
                    for (const auto& multiplicationTerm : lhsMultiplicationTerms) {
                        if (multHelper != lhsMultiplicationTerms.size() - 1) {
                            inequality += "(* " + multiplicationTerm + " ";
                            multHelper++;
                        } else {
                            inequality += multiplicationTerm;
                            for (int i = 0; i < multHelper; i++) {
                                inequality += ")";
                            }
                        }
                    }
                } else {
                    inequality += lhsMultiplicationTerms[0] + " ";
                }

            }

            if (addHelper == lhsTerms.size() - 1) {
                for (int i = 0; i < addHelper; i++) {
                    inequality += ")";
                }
            }

            inequality += " 0)";
            result.emplace_back(inequality);
        }
    }

    for (const auto & rhsPair : rhs) {
        std::pair<int, bool> degrees = rhsPair.first;
        std::string rhsCoefficient = rhsPair.second;

        if (lhs.count(degrees) == 0) {
            std::string inequality;

            if (!degrees.second) {
                inequality += "(<= ";
            } else {
                inequality += "(< ";
            }


            std::vector<std::string> rhsTerms = splitByAddition(rhsCoefficient);

            int addHelper = 0;
            for (const auto& term : rhsTerms) {
                if (addHelper != rhsTerms.size() - 1) {
                    inequality += "(+ ";
                    addHelper++;
                } else {
                    inequality += " ";
                }
                std::vector<std::string> rhsMultiplicationTerms = splitByMultiplication(term);
                if (rhsMultiplicationTerms.size() > 1) {
                    int multHelper = 0;
                    for (const auto& multiplicationTerm : rhsMultiplicationTerms) {
                        if (multHelper != rhsMultiplicationTerms.size() - 1) {
                            inequality += "(* " + multiplicationTerm + " ";
                            multHelper++;
                        } else {
                            inequality += multiplicationTerm;
                            for (int i = 0; i < multHelper; i++) {
                                inequality += ")";
                            }
                        }
                    }
                } else {
                    inequality += rhsMultiplicationTerms[0] + " ";
                }

            }

            if (addHelper == rhsTerms.size() - 1) {
                for (int i = 0; i < addHelper; i++) {
                    inequality += ")";
                }
            }

            inequality += " 0)";
            result.emplace_back(inequality);
        }
    }

    return result;
}

#endif //FLT1_INEQUALITIESGENERATION_H
