#include "evaluator.h"
#include "math_functions.h"
#include "utils.h"
#include <cmath>
#include <stack>
#include <unordered_set>

namespace {
    bool isBinaryLog(const std::string& name, size_t stackSize) {
        return name == "log" && stackSize >= 2;
    }
}

double Evaluator::evaluate(const std::vector<Token>& rpn) {
    std::stack<double> st;

    for (const Token& t : rpn) {
        switch (t.type) {
            case TokenType::NUMBER:
            case TokenType::CONSTANT:
                st.push(t.value);
                break;

            case TokenType::OPERATOR: {
                if (st.size() < 2)
                    throw CalculatorException("Insufficient operands for operator " + t.name);
                double b = st.top(); st.pop();
                double a = st.top(); st.pop();
                if (t.name == "+") st.push(a + b);
                else if (t.name == "-") st.push(a - b);
                else if (t.name == "*") st.push(a * b);
                else if (t.name == "/") {
                    if (b == 0) throw CalculatorException("Division by zero");
                    st.push(a / b);
                }
                else if (t.name == "%") {
                    if (b == 0) throw CalculatorException("Modulo by zero");
                    st.push(std::fmod(a, b));
                }
                else if (t.name == "^") st.push(std::pow(a, b));
                else throw CalculatorException("Unknown operator: " + t.name);
                break;
            }

            case TokenType::FUNCTION: {
                if (isBinaryLog(t.name, st.size())) {
                    double b = st.top(); st.pop();
                    double a = st.top(); st.pop();
                    st.push(math_functions::call2(t.name, a, b));
                } else {
                    if (st.size() < 1)
                        throw CalculatorException("Insufficient arguments for " + t.name);
                    double a = st.top(); st.pop();
                    st.push(math_functions::call(t.name, a));
                }
                break;
            }

            default:
                throw CalculatorException("Unexpected token in RPN");
        }
    }

    if (st.size() != 1)
        throw CalculatorException("Invalid expression: stack has " + std::to_string(st.size()) + " values");
    return st.top();
}
