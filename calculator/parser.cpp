#include "parser.h"
#include "utils.h"
#include <stack>

Parser::Parser(Tokenizer& tok) : tokenizer_(tok) {
    advance();
}

void Parser::advance() {
    current_ = tokenizer_.next();
}

int Parser::precedence(const std::string& op) const {
    if (op == "u-") return 5;
    if (op == "^")  return 4;
    if (op == "*" || op == "/" || op == "%") return 3;
    if (op == "+" || op == "-") return 2;
    return 0;
}

bool Parser::isRightAssoc(const std::string& op) const {
    return op == "^" || op == "u-";
}

std::vector<Token> Parser::parse() {
    std::vector<Token> output;

    // Operator stack holds: operator strings OR function names.
    // We use a separate struct to distinguish them.
    struct OpEntry {
        std::string name;
        bool        isFunc;
    };

    std::stack<OpEntry> opStack;

    // arityStack mirrors function frames on opStack.
    // Top = arg count for the innermost active function call.
    std::stack<int> arityStack;

    bool expectOperand = true;

    auto emitOperator = [&](const std::string& name) {
        if (name == "u-")
            output.push_back(Token{TokenType::FUNCTION, 0, "neg", 1});
        else
            output.push_back(Token{TokenType::OPERATOR, 0, name, 0});
    };

    auto emitFunction = [&](const std::string& name) {
        if (arityStack.empty())
            throw CalculatorException("Internal parser error: function arity underflow");

        int arity = arityStack.top();
        arityStack.pop();

        if (arity < 1 || arity > 2)
            throw CalculatorException("Function '" + name + "' expects 1 or 2 arguments");

        output.push_back(Token{TokenType::FUNCTION, 0, name, arity});
    };

    auto flushOps = [&](int prec, bool rightAssoc) {
        while (!opStack.empty() && !opStack.top().isFunc && opStack.top().name != "(") {
            const std::string& top = opStack.top().name;
            int topPrec = precedence(top);
            if (topPrec < prec) break;
            if (topPrec == prec && rightAssoc) break;
            opStack.pop();
            emitOperator(top);
        }
    };

    while (current_.type != TokenType::END) {
        const Token t = current_;

        if (t.type == TokenType::NUMBER || t.type == TokenType::CONSTANT) {
            output.push_back(t);
            advance();
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::FUNCTION) {
            opStack.push({t.name, true});
            arityStack.push(1);
            advance();
            if (current_.type != TokenType::LEFT_PAREN)
                throw CalculatorException("Expected '(' after function '" + t.name + "'");
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::OPERATOR) {
            if (t.name == "-" && expectOperand) {
                opStack.push({"u-", false});
                advance();
                expectOperand = true;
                continue;
            }
            if (t.name == "+" && expectOperand) {
                advance();
                expectOperand = true;
                continue;
            }
            flushOps(precedence(t.name), isRightAssoc(t.name));
            opStack.push({t.name, false});
            advance();
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::LEFT_PAREN) {
            opStack.push({"(", false});
            advance();
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::RIGHT_PAREN) {
            if (expectOperand)
                throw CalculatorException("Missing expression before ')'");

            while (!opStack.empty() && opStack.top().name != "(") {
                OpEntry e = opStack.top(); opStack.pop();
                if (e.isFunc)
                    emitFunction(e.name);
                else
                    emitOperator(e.name);
            }
            if (opStack.empty())
                throw CalculatorException("Mismatched parentheses: unexpected ')'");

            opStack.pop(); // discard "("

            // If the top of opStack is a function, pop it now.
            if (!opStack.empty() && opStack.top().isFunc) {
                emitFunction(opStack.top().name);
                opStack.pop();
            }

            advance();
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::COMMA) {
            if (expectOperand)
                throw CalculatorException("Missing argument before ','");

            while (!opStack.empty() && opStack.top().name != "(") {
                OpEntry e = opStack.top(); opStack.pop();
                if (e.isFunc)
                    emitFunction(e.name);
                else
                    emitOperator(e.name);
            }
            if (opStack.empty())
                throw CalculatorException("Unexpected ','");

            // Commas are valid only inside function argument lists.
            opStack.pop();
            bool inFunctionCall = !opStack.empty() && opStack.top().isFunc;
            opStack.push({"(", false});

            if (!inFunctionCall || arityStack.empty())
                throw CalculatorException("Unexpected ','");

            arityStack.top()++;
            advance();
            expectOperand = true;
            continue;
        }

        throw CalculatorException("Unexpected token");
    }

    while (!opStack.empty()) {
        OpEntry e = opStack.top(); opStack.pop();
        if (e.name == "(")
            throw CalculatorException("Mismatched parentheses: unclosed '('");
        if (e.isFunc) {
            emitFunction(e.name);
        } else {
            emitOperator(e.name);
        }
    }

    return output;
}
