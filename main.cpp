#include "evaluator.h"
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"
#include <iostream>
#include <string>

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static bool handleCommand(const std::string& line) {
    std::string t = trim(line);
    if (t.empty()) return true;

    if (t == "quit" || t == "exit") return false;

    if (t == "help") {
        std::cout << "Scientific Calculator\n"
                     "  Enter expressions: 2+3*4, sin(pi/2), log(100)\n"
                     "  Commands: mode degree | mode radian | help | quit\n";
        return true;
    }

    if (t == "mode degree") {
        utils::setAngleMode(AngleMode::DEGREE);
        std::cout << "Angle mode set to DEGREE\n";
        return true;
    }
    if (t == "mode radian") {
        utils::setAngleMode(AngleMode::RADIAN);
        std::cout << "Angle mode set to RADIAN\n";
        return true;
    }

    return false;
}

int main() {
    Evaluator eval;
    std::cout << "Scientific Calculator. Type 'help' for commands, 'quit' to exit.\n";

    while (true) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;

        if (handleCommand(line)) continue;

        try {
            Tokenizer tok(line);
            Parser parser(tok);
            std::vector<Token> rpn = parser.parse();
            double result = eval.evaluate(rpn);
            std::cout << result << "\n";
        } catch (const CalculatorException& e) {
            std::cerr << "Error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
