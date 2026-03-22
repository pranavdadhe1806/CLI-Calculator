#include "math_functions.h"
#include "utils.h"
#include <cmath>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

namespace math_functions {

static double toRad(double deg) {
    return deg * M_PI / 180.0;
}

double call(const std::string& name, double a) {
    bool useDeg = (utils::getAngleMode() == AngleMode::DEGREE);

    if (name == "neg") return -a;
    if (name == "sin") return std::sin(useDeg ? toRad(a) : a);
    if (name == "cos") return std::cos(useDeg ? toRad(a) : a);
    if (name == "tan") return std::tan(useDeg ? toRad(a) : a);
    if (name == "asin") {
        if (a < -1 || a > 1) throw CalculatorException("asin: argument must be in [-1, 1]");
        double r = std::asin(a);
        return useDeg ? r * 180.0 / M_PI : r;
    }
    if (name == "acos") {
        if (a < -1 || a > 1) throw CalculatorException("acos: argument must be in [-1, 1]");
        double r = std::acos(a);
        return useDeg ? r * 180.0 / M_PI : r;
    }
    if (name == "atan") {
        double r = std::atan(a);
        return useDeg ? r * 180.0 / M_PI : r;
    }
    if (name == "sinh") return std::sinh(a);
    if (name == "cosh") return std::cosh(a);
    if (name == "tanh") return std::tanh(a);
    if (name == "log") {
        if (a <= 0) throw CalculatorException("log: argument must be positive");
        return std::log10(a);
    }
    if (name == "ln") {
        if (a <= 0) throw CalculatorException("ln: argument must be positive");
        return std::log(a);
    }
    if (name == "exp") return std::exp(a);
    if (name == "sqrt") {
        if (a < 0) throw CalculatorException("sqrt: argument must be non-negative");
        return std::sqrt(a);
    }
    if (name == "cbrt") return std::cbrt(a);
    if (name == "abs") return std::fabs(a);
    if (name == "floor") return std::floor(a);
    if (name == "ceil") return std::ceil(a);
    if (name == "factorial") {
        if (a < 0 || a != std::floor(a))
            throw CalculatorException("factorial: argument must be non-negative integer");
        if (a > 170) throw CalculatorException("factorial: argument too large");
        double r = 1;
        for (int n = 1; n <= static_cast<int>(a); ++n) r *= n;
        return r;
    }
    throw CalculatorException("Unknown function: " + name);
}

double call2(const std::string& name, double a, double b) {
    if (name == "log") {
        if (a <= 0 || a == 1) throw CalculatorException("log(a,b): base must be positive and not 1");
        if (b <= 0) throw CalculatorException("log(a,b): argument must be positive");
        return std::log(b) / std::log(a);
    }
    throw CalculatorException("Unknown two-argument function: " + name);
}

} // namespace math_functions
