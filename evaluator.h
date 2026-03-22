#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "tokenizer.h"
#include <vector>

class Evaluator {
public:
    double evaluate(const std::vector<Token>& rpn);
};

#endif // EVALUATOR_H
