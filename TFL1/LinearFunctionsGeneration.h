#ifndef FLT1_LINEARFUNCTIONSGENERATION_H
#define FLT1_LINEARFUNCTIONSGENERATION_H

std::pair<Node*, Node*>* generateLinearFunctions(const std::string& lhs, const std::string& rhs) {
    std::vector<Node*> linear_functions_lhs = {};
    std::vector<Node*> linear_functions_rhs = {};
    int helper = 0;

    for (auto symbol = lhs.crbegin(); symbol != lhs.crend(); ++symbol) {
        std::string s(1, *symbol);
        if (linear_functions_lhs.empty()) {
            LinearFunction func(new OperationNode("+",
                                                  new OperationNode("+",
                                                                    new OperationNode("*",
                                                                                      new OperationNode("+",
                                                                                                        new OperationNode("*",
                                                                                                                          new OrdinalNode(Ordinal(true, "w")),
                                                                                                                          new OrdinalNode(Ordinal("a_" + s))),
                                                                                                        new OrdinalNode(Ordinal("b_" + s))),
                                                                                      new OrdinalNode(Ordinal("x"))),
                                                                    new OperationNode("*",
                                                                                      new OrdinalNode(Ordinal(true, "w")),
                                                                                      new OrdinalNode(Ordinal("c_" + s)))),
                                                  new OrdinalNode(Ordinal("d_" + s))));

            linear_functions_lhs.emplace_back(func.root->clone());
        } else {
            LinearFunction func(new OperationNode("+",
                                                  new OperationNode("+",
                                                                    new OperationNode("*",
                                                                                      new OperationNode("+",
                                                                                                        new OperationNode("*",
                                                                                                                          new OrdinalNode(Ordinal(true, "w")),
                                                                                                                          new OrdinalNode(Ordinal("a_" + s))),
                                                                                                        new OrdinalNode(Ordinal("b_" + s))),
                                                                                      linear_functions_lhs[helper]->clone()),
                                                                    new OperationNode("*",
                                                                                      new OrdinalNode(Ordinal(true, "w")),
                                                                                      new OrdinalNode(Ordinal("c_" + s)))),
                                                  new OrdinalNode(Ordinal("d_" + s))));
            helper++;
            auto f = func.simplify();
            auto str = f->to_string();
            linear_functions_lhs.emplace_back(f->root->clone());
        }
    }

    helper = 0;

    for (auto symbol = rhs.crbegin(); symbol != rhs.crend(); ++symbol) {
        std::string s(1, *symbol);
        if (linear_functions_rhs.empty()) {
            LinearFunction func(new OperationNode("+",
                                                  new OperationNode("+",
                                                                    new OperationNode("*",
                                                                                      new OperationNode("+",
                                                                                                        new OperationNode("*",
                                                                                                                          new OrdinalNode(Ordinal(true, "w")),
                                                                                                                          new OrdinalNode(Ordinal("a_" + s))),
                                                                                                        new OrdinalNode(Ordinal("b_" + s))),
                                                                                      new OrdinalNode(Ordinal("x"))),
                                                                    new OperationNode("*",
                                                                                      new OrdinalNode(Ordinal(true, "w")),
                                                                                      new OrdinalNode(Ordinal("c_" + s)))),
                                                  new OrdinalNode(Ordinal("d_" + s))));

            linear_functions_rhs.emplace_back(func.root->clone());
        } else {
            LinearFunction func(new OperationNode("+",
                                                  new OperationNode("+",
                                                                    new OperationNode("*",
                                                                                      new OperationNode("+",
                                                                                                        new OperationNode("*",
                                                                                                                          new OrdinalNode(Ordinal(true, "w")),
                                                                                                                          new OrdinalNode(Ordinal("a_" + s))),
                                                                                                        new OrdinalNode(Ordinal("b_" + s))),
                                                                                      linear_functions_rhs[helper]->clone()),
                                                                    new OperationNode("*",
                                                                                      new OrdinalNode(Ordinal(true, "w")),
                                                                                      new OrdinalNode(Ordinal("c_" + s)))),
                                                  new OrdinalNode(Ordinal("d_" + s))));
            helper++;
            auto f = func.simplify();
            auto str = f->to_string();
            linear_functions_rhs.emplace_back(f->root->clone());
        }
    }

    return new std::pair<Node*, Node*>(linear_functions_lhs[lhs.length() - 1], linear_functions_rhs[rhs.length() - 1]);
}

#endif //FLT1_LINEARFUNCTIONSGENERATION_H
