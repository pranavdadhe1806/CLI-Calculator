#include "tokenizer.h"
#include "utils.h"
#include <cctype>
#include <cmath>
#include <unordered_set>

namespace {
    const std::unordered_set<std::string> FUNCTIONS = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh",
        "log", "ln", "exp", "sqrt", "cbrt",
        "abs", "floor", "ceil", "factorial"
    };
    const std::unordered_set<std::string> CONSTANTS = { "pi", "e" };
}

Tokenizer::Tokenizer(const std::string& input) : input_(input), pos_(0) {}

void Tokenizer::skipWhitespace() {
    while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_])))
        ++pos_;
}

bool Tokenizer::isAtEnd() const { return pos_ >= input_.size(); }

char Tokenizer::peek() const {
    return isAtEnd() ? '\0' : input_[pos_];
}

char Tokenizer::advance() {
    if (isAtEnd()) return '\0';
    return input_[pos_++];
}

bool Tokenizer::match(char c) {
    if (peek() == c) { advance(); return true; }
    return false;
}

Token Tokenizer::scanNumber() {
    size_t start = pos_ - 1;
    while (std::isdigit(static_cast<unsigned char>(peek()))) advance();

    if (match('.')) {
        while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
    }

    if (match('e') || match('E')) {
        match('+');
        match('-');
        if (!std::isdigit(static_cast<unsigned char>(peek())))
            throw CalculatorException("Invalid scientific notation");
        while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
    }

    std::string raw(input_.begin() + start, input_.begin() + pos_);
    double v = 0;
    try {
        v = std::stod(raw);
    } catch (...) {
        throw CalculatorException("Invalid number: " + raw);
    }
    return Token{TokenType::NUMBER, v, ""};
}

Token Tokenizer::scanOperator() {
    char c = input_[pos_ - 1];
    std::string op;
    op += c;
    return Token{TokenType::OPERATOR, 0, op};
}

Token Tokenizer::scanIdentifier() {
    size_t start = pos_ - 1;
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
        advance();
    std::string id(input_.begin() + start, input_.begin() + pos_);

    for (char& ch : id) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    if (CONSTANTS.count(id)) {
        double val = (id == "pi") ? 3.14159265358979323846 : 2.71828182845904523536;
        return Token{TokenType::CONSTANT, val, id};
    }
    if (FUNCTIONS.count(id))
        return Token{TokenType::FUNCTION, 0, id};

    throw CalculatorException("Unknown identifier: " + id);
}

Token Tokenizer::next() {
    skipWhitespace();
    if (isAtEnd()) return Token{TokenType::END, 0, ""};

    char c = advance();
    if (std::isdigit(static_cast<unsigned char>(c)))
        return scanNumber();
    if (c == '+' || c == '*' || c == '/' || c == '%' || c == '^')
        return Token{TokenType::OPERATOR, 0, std::string(1, c)};
    if (c == '-')
        return Token{TokenType::OPERATOR, 0, "-"};
    if (c == '(')
        return Token{TokenType::LEFT_PAREN, 0, "("};
    if (c == ')')
        return Token{TokenType::RIGHT_PAREN, 0, ")"};
    if (c == ',')
        return Token{TokenType::COMMA, 0, ","};
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
        return scanIdentifier();

    throw CalculatorException(std::string("Unexpected character: ") + c);
}

void Tokenizer::reset() { pos_ = 0; }
