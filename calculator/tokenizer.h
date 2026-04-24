#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

enum class TokenType {
    NUMBER,
    OPERATOR,
    LEFT_PAREN,
    RIGHT_PAREN,
    FUNCTION,
    CONSTANT,
    COMMA,
    END
};

struct Token {
    TokenType   type;
    double      value;  // NUMBER, CONSTANT
    std::string name;   // OPERATOR (e.g. "+"), FUNCTION (e.g. "sin")
    int         arity;  // FUNCTION only: set by Parser (1 or 2); 0 elsewhere
};

class Tokenizer {
public:
    explicit Tokenizer(const std::string& input);
    Token next();
    void  reset();

private:
    std::string input_;
    size_t      pos_;

    void skipWhitespace();
    bool isAtEnd() const;
    char peek()    const;
    char advance();

    Token scanNumber();
    Token scanIdentifier();
};

#endif // TOKENIZER_H
