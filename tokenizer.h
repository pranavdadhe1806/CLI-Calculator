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
    TokenType type;
    double value;       // for NUMBER, CONSTANT
    std::string name;   // for OPERATOR (e.g. "+"), FUNCTION (e.g. "sin")
};

class Tokenizer {
public:
    explicit Tokenizer(const std::string& input);
    Token next();
    void reset();

private:
    std::string input_;
    size_t pos_;

    void skipWhitespace();
    bool isAtEnd() const;
    char peek() const;
    char advance();
    bool match(char c);

    Token scanNumber();
    Token scanOperator();
    Token scanIdentifier();
};

#endif // TOKENIZER_H
