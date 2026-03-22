#ifndef UTILS_H
#define UTILS_H

#include <stdexcept>
#include <string>

enum class AngleMode { DEGREE, RADIAN };

class CalculatorException : public std::runtime_error {
public:
    explicit CalculatorException(const std::string& msg) : std::runtime_error(msg) {}
};

namespace utils {
    AngleMode getAngleMode();
    void setAngleMode(AngleMode mode);
}

#endif // UTILS_H
