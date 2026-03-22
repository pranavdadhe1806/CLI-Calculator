#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include <vector>

class Parser {
public:
    explicit Parser(Tokenizer& tokenizer);
    std::vector<Token> parse();

private:
    Tokenizer& tokenizer_;
    Token lookahead_;
    bool hasLookahead_;

    void advance();
    const Token& current() const;
    bool isUnaryContext() const;

    int precedence(const std::string& op) const;
    bool isRightAssoc(const std::string& op) const;
};

#endif // PARSER_H
