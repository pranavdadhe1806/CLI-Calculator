#include "evaluator.h"
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// ─── Helpers ────────────────────────────────────────────────────────────────

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::string toLower(std::string s) {
    for (char& ch : s)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

static std::string formatResult(double v) {
    // Snap near-zero results (e.g. sin(pi) = 6.12e-17 -> 0)
    if (std::fabs(v) < 1e-14) v = 0.0;

    // If the value is an integer that fits nicely, print without decimals
    if (v == std::floor(v) && std::fabs(v) < 1e15) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(0) << v;
        return os.str();
    }

    // Otherwise use up to 12 significant digits, trim trailing zeros
    std::ostringstream os;
    os << std::setprecision(12) << v;
    std::string s = os.str();
    if (s.find('.') != std::string::npos) {
        size_t last = s.find_last_not_of('0');
        if (last != std::string::npos && s[last] == '.') --last;
        s.erase(last + 1);
    }
    return s;
}

static const char* modeTag() {
    return (utils::getAngleMode() == AngleMode::DEGREE) ? "DEG" : "RAD";
}

// ─── TUI elements ──────────────────────────────────────────────────────────

static void printBanner() {
    std::cout <<
        "\n"
        "  ╔═══════════════════════════════════════════════════════╗\n"
        "  ║                                                       ║\n"
        "  ║    ____      _       ____      _                      ║\n"
        "  ║   / ___|__ _| | ___ / ___|__ _| | ___                 ║\n"
        "  ║  | |   / _` | |/ __| |   / _` | |/ __|                ║\n"
        "  ║  | |__| (_| | | (__| |__| (_| | | (__                  ║\n"
        "  ║   \\____\\__,_|_|\\___|\\____\\__,_|_|\\___|             ║\n"
        "  ║                                                       ║\n"
        "  ║            Scientific CLI Calculator                  ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║  Type 'help' for usage guide   |   'quit' to exit    ║\n"
        "  ╚═══════════════════════════════════════════════════════╝\n"
        "\n";
}

static void printHelp() {
    std::cout <<
        "\n"
        "  ╔═══════════════════════════════════════════════════════╗\n"
        "  ║                    HELP  GUIDE                        ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  USAGE                                                ║\n"
        "  ║    Type any expression and press Enter.               ║\n"
        "  ║    Scientific notation works: 1.5e-10, 3E8            ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  OPERATORS              (by precedence, high to low)  ║\n"
        "  ║    ^                    power (right-associative)     ║\n"
        "  ║    *   /   %            multiply, divide, modulo      ║\n"
        "  ║    +   -                add, subtract                 ║\n"
        "  ║    ( )                  grouping                      ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  CONSTANTS                                            ║\n"
        "  ║    pi                   3.14159265...                 ║\n"
        "  ║    e                    2.71828182...                 ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  FUNCTIONS (1 arg)                                    ║\n"
        "  ║    Trig      : sin  cos  tan  asin  acos  atan       ║\n"
        "  ║    Hyperbolic: sinh  cosh  tanh                      ║\n"
        "  ║    Log / Exp : log(x)  ln(x)  exp(x)                ║\n"
        "  ║    Roots     : sqrt(x)  cbrt(x)                     ║\n"
        "  ║    Utility   : abs  floor  ceil  factorial            ║\n"
        "  ║                                                       ║\n"
        "  ║  FUNCTIONS (2 args)                                   ║\n"
        "  ║    log(base, x)         log base 'base' of x        ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  ANGLE MODE             affects trig functions        ║\n"
        "  ║    mode radian          (default)                    ║\n"
        "  ║    mode degree          switch to degrees            ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  COMMANDS                                             ║\n"
        "  ║    help                 show this guide               ║\n"
        "  ║    mode degree          set trig to degrees           ║\n"
        "  ║    mode radian          set trig to radians           ║\n"
        "  ║    quit / exit          close calculator               ║\n"
        "  ║                                                       ║\n"
        "  ╠═══════════════════════════════════════════════════════╣\n"
        "  ║                                                       ║\n"
        "  ║  EXAMPLES                                             ║\n"
        "  ║    (2 + 3) * 4          => 20                        ║\n"
        "  ║    3^3^2                => 19683                     ║\n"
        "  ║    sin(pi/2)            => 1                          ║\n"
        "  ║    log(100)             => 2                          ║\n"
        "  ║    log(2, 8)            => 3                          ║\n"
        "  ║    factorial(5)         => 120                        ║\n"
        "  ║    sqrt(2)              => 1.41421356237              ║\n"
        "  ║                                                       ║\n"
        "  ╚═══════════════════════════════════════════════════════╝\n"
        "\n";
}

// ─── Command handling ───────────────────────────────────────────────────────

enum class CommandResult {
    NOT_A_COMMAND,
    HANDLED,
    QUIT
};

static CommandResult handleCommand(const std::string& line) {
    const std::string t = toLower(trim(line));
    if (t.empty()) return CommandResult::HANDLED;

    if (t == "quit" || t == "exit") return CommandResult::QUIT;

    if (t == "help") {
        printHelp();
        return CommandResult::HANDLED;
    }

    if (t == "mode degree") {
        utils::setAngleMode(AngleMode::DEGREE);
        std::cout << "  >> Angle mode set to DEGREE\n";
        return CommandResult::HANDLED;
    }
    if (t == "mode radian") {
        utils::setAngleMode(AngleMode::RADIAN);
        std::cout << "  >> Angle mode set to RADIAN\n";
        return CommandResult::HANDLED;
    }

    return CommandResult::NOT_A_COMMAND;
}

// ─── Entry point ────────────────────────────────────────────────────────────

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    Evaluator eval;
    printBanner();

    while (true) {
        std::cout << "  [" << modeTag() << "] > ";
        std::string line;
        if (!std::getline(std::cin, line)) break;

        CommandResult cmd = handleCommand(line);
        if (cmd == CommandResult::QUIT) {
            std::cout << "\n  Goodbye!\n\n";
            break;
        }
        if (cmd == CommandResult::HANDLED) continue;

        try {
            Tokenizer tok(line);
            Parser    parser(tok);
            std::vector<Token> rpn = parser.parse();
            double result = eval.evaluate(rpn);
            std::cout << "  = " << formatResult(result) << "\n";
        } catch (const CalculatorException& e) {
            std::cerr << "  Error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "  Error: " << e.what() << "\n";
        }
    }

    return 0;
}