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
    Token      current_;

    void advance();
    int  precedence(const std::string& op) const;
    bool isRightAssoc(const std::string& op) const;
};

#endif // PARSER_H
