#include "parser.h"
#include "utils.h"
#include <stack>
#include <unordered_set>

namespace {
    const std::unordered_set<std::string> FUNC_NAMES = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "log", "ln", "exp", "sqrt", "cbrt",
        "abs", "floor", "ceil", "factorial", "neg"
    };
}

Parser::Parser(Tokenizer& tokenizer) : tokenizer_(tokenizer), hasLookahead_(false) {
    advance();
}

void Parser::advance() {
    lookahead_ = tokenizer_.next();
    hasLookahead_ = true;
}

const Token& Parser::current() const { return lookahead_; }

bool Parser::isUnaryContext() const {
    if (!hasLookahead_) return true;
    if (lookahead_.type == TokenType::LEFT_PAREN || lookahead_.type == TokenType::COMMA)
        return true;
    if (lookahead_.type == TokenType::OPERATOR) return true;
    return false;
}

int Parser::precedence(const std::string& op) const {
    if (op == "u-") return 5;
    if (op == "^") return 4;
    if (op == "*" || op == "/" || op == "%") return 3;
    if (op == "+" || op == "-") return 2;
    return 0;
}

bool Parser::isRightAssoc(const std::string& op) const {
    return op == "^" || op == "u-";
}

static bool isLeftAssoc(const std::string& op) {
    return op != "^" && op != "u-";
}

std::vector<Token> Parser::parse() {
    std::vector<Token> output;
    std::stack<std::string> opStack;
    bool expectOperand = true;

    auto popWhile = [&](int currentPrec, bool currentIsRight) {
        while (!opStack.empty() && opStack.top() != "(") {
            const std::string op = opStack.top();
            if (op == "u-") {
                opStack.pop();
                output.push_back(Token{TokenType::FUNCTION, 0, "neg"});
                continue;
            }
            int p = precedence(op);
            if (p < currentPrec) break;
            if (p == currentPrec && currentIsRight) break;  // don't pop same prec for right-assoc
            if (p == currentPrec && isLeftAssoc(op)) { /* fall through to pop */ }
            opStack.pop();
            output.push_back(Token{TokenType::OPERATOR, 0, op});
        }
    };

    while (current().type != TokenType::END) {
        const Token& t = current();

        if (t.type == TokenType::NUMBER) {
            output.push_back(t);
            advance();
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::CONSTANT) {
            output.push_back(t);
            advance();
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::FUNCTION) {
            opStack.push(t.name);
            advance();
            expectOperand = true;
            if (current().type != TokenType::LEFT_PAREN)
                throw CalculatorException("Function must be followed by '('");
            continue;
        }

        if (t.type == TokenType::OPERATOR) {
            if (t.name == "-" && expectOperand) {
                opStack.push("u-");
                advance();
                expectOperand = true;
                continue;
            }
            if (t.name == "+" && expectOperand) {
                advance();
                expectOperand = true;
                continue;
            }
            int p = precedence(t.name);
            popWhile(p, isRightAssoc(t.name));
            opStack.push(t.name);
            advance();
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::LEFT_PAREN) {
            opStack.push("(");
            advance();
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::RIGHT_PAREN) {
            while (!opStack.empty() && opStack.top() != "(") {
                std::string op = opStack.top();
                opStack.pop();
                if (op == "u-")
                    output.push_back(Token{TokenType::FUNCTION, 0, "neg"});
                else
                    output.push_back(Token{TokenType::OPERATOR, 0, op});
            }
            if (opStack.empty())
                throw CalculatorException("Mismatched parentheses");
            opStack.pop();  // discard "("
            advance();
            expectOperand = false;
            if (!opStack.empty() && opStack.top() != "(" &&
                FUNC_NAMES.count(opStack.top())) {
                std::string fn = opStack.top();
                opStack.pop();
                output.push_back(Token{TokenType::FUNCTION, 0, fn});
            }
            continue;
        }

        if (t.type == TokenType::COMMA) {
            while (!opStack.empty() && opStack.top() != "(") {
                std::string op = opStack.top();
                opStack.pop();
                if (op == "u-")
                    output.push_back(Token{TokenType::FUNCTION, 0, "neg"});
                else
                    output.push_back(Token{TokenType::OPERATOR, 0, op});
            }
            advance();
            expectOperand = true;
            continue;
        }

        throw CalculatorException("Unexpected token");
    }

    while (!opStack.empty()) {
        std::string op = opStack.top();
        opStack.pop();
        if (op == "(")
            throw CalculatorException("Mismatched parentheses");
        if (op == "u-")
            output.push_back(Token{TokenType::FUNCTION, 0, "neg"});
        else
            output.push_back(Token{TokenType::OPERATOR, 0, op});
    }

    return output;
}
