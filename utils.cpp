#include "utils.h"

namespace {
    AngleMode g_angleMode = AngleMode::RADIAN;
}

namespace utils {
    AngleMode getAngleMode() { return g_angleMode; }
    void setAngleMode(AngleMode mode) { g_angleMode = mode; }
}
