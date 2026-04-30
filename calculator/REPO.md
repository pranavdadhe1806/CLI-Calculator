# CLI Scientific Calculator - Complete Project Documentation

## Table of Contents
1. [Project Overview](#1-project-overview)
2. [Architecture & Approach](#2-architecture--approach)
3. [Directory Structure](#3-directory-structure)
4. [Detailed Codebase Breakdown](#4-detailed-codebase-breakdown)
5. [Data Flow & Pipeline](#5-data-flow--pipeline)
6. [Building & Running](#6-building--running)
7. [Usage Guide](#7-usage-guide)
8. [Error Handling](#8-error-handling)
9. [Design Principles](#9-design-principles)
10. [Future Enhancements](#10-future-enhancements)

---

## 1. Project Overview

The CLI Scientific Calculator is a robust, production-quality command-line application built in modern C++ (C++17) that evaluates complex mathematical expressions. It takes user input as standard text strings in infix notation (the conventional way humans write math) and computes accurate results.

### Key Capabilities

- **Basic Arithmetic**: Addition (`+`), Subtraction (`-`), Multiplication (`*`), Division (`/`), Modulo (`%`)
- **Advanced Operators**: Right-associative exponentiation (`^`), unary minus
- **Trigonometric Functions**: `sin`, `cos`, `tan`, `asin`, `acos`, `atan` with degree/radian mode support
- **Hyperbolic Functions**: `sinh`, `cosh`, `tanh`
- **Logarithmic Functions**: `log` (base 10), `ln` (natural log), `log(base, x)` (custom base), `exp` (e^x)
- **Root & Utility Functions**: `sqrt`, `cbrt`, `abs`, `floor`, `ceil`, `factorial`
- **Mathematical Constants**: `pi` (3.14159265...), `e` (2.71828182...)
- **Scientific Notation**: Support for inputs like `1.5e-10` or `3E8`
- **Smart Formatting**: Near-zero snapping, integer detection, and trailing zero removal in output

---

## 2. Architecture & Approach

### Three-Stage Pipeline Architecture

The calculator is built around a classic **compiler-style three-stage pipeline** that transforms raw user input into computed results:

```
User Input (String)
       │
       ▼
┌─────────────────┐
│   Tokenizer     │  ← Lexical Analysis
│  (Lexical)      │     Converts raw string into discrete tokens
└────────┬────────┘
         │ Token Stream
         ▼
┌─────────────────┐
│    Parser       │  ← Syntax Analysis
│  (Shunting Yard)│     Converts infix to Reverse Polish Notation (RPN)
└────────┬────────┘
         │ RPN Token Queue
         ▼
┌─────────────────┐
│   Evaluator     │  ← Execution
│  (Stack-based)  │     Computes final result
└────────┬────────┘
         │
         ▼
    Double Result
```

### Why This Approach?

1. **Separation of Concerns**: Each stage handles one specific transformation, making debugging and extension straightforward
2. **Classic Compiler Theory**: Uses the well-proven Shunting Yard algorithm (by Edsger Dijkstra) for operator precedence handling
3. **Extensibility**: Adding new functions requires changes only in the Tokenizer (recognition) and MathFunctions (execution)
4. **Error Isolation**: Syntax errors are caught during parsing; domain errors during evaluation

---

## 3. Directory Structure

```
cli-calculator/
│
├── README.md                    # Project overview and quick start guide
├── REPO.md                      # Repository guide (original documentation)
├── REPO2.md                     # This file - comprehensive project documentation
├── DEVELOPMENT_PLAN.md          # Development roadmap and future plans
├── .gitignore                   # Git ignore rules
│
└── calculator/                  # Source code directory
    ├── CMakeLists.txt           # CMake build configuration
    │
    ├── tokenizer.h              # Tokenizer header (types, Token struct, Tokenizer class)
    ├── tokenizer.cpp            # Tokenizer implementation (lexical analysis)
    │
    ├── parser.h                 # Parser header (Parser class declaration)
    ├── parser.cpp               # Parser implementation (Shunting Yard algorithm)
    │
    ├── evaluator.h              # Evaluator header (Evaluator class declaration)
    ├── evaluator.cpp            # Evaluator implementation (RPN evaluation)
    │
    ├── math_functions.h         # Math functions header (function declarations)
    ├── math_functions.cpp       # Math functions implementation (cmath wrappers)
    │
    ├── utils.h                  # Utilities header (exceptions, angle mode, config)
    ├── utils.cpp                # Utilities implementation (global state management)
    │
    └── main.cpp                 # Application entry point (REPL, CLI interface)
```

---

## 4. Detailed Codebase Breakdown

### 4.1 Tokenizer Module (`tokenizer.h` / `tokenizer.cpp`)

**Purpose**: Lexical Analysis — converts raw input string into a stream of meaningful tokens.

#### Data Structures

**TokenType Enum**
```
NUMBER        - Numeric literals (e.g., 42, 3.14, 1.5e-10)
OPERATOR      - Mathematical operators (+, -, *, /, %, ^)
LEFT_PAREN    - Opening parenthesis (
RIGHT_PAREN   - Closing parenthesis )
FUNCTION      - Math function names (sin, cos, log, etc.)
CONSTANT      - Mathematical constants (pi, e)
COMMA         - Argument separator in function calls
END           - End of input marker
```

**Token Struct**
| Field   | Type        | Description                                    |
|---------|-------------|------------------------------------------------|
| type    | TokenType   | Classification of the token                    |
| value   | double      | Numeric value for NUMBER and CONSTANT tokens   |
| name    | std::string | String representation for OPERATOR and FUNCTION|
| arity   | int         | Number of arguments (set by Parser for functions) |

#### Key Functions

| Function         | Description                                           |
|------------------|-------------------------------------------------------|
| `next()`         | Returns the next token from input, advances position  |
| `reset()`        | Resets the tokenizer to the beginning of input        |
| `scanNumber()`   | Parses integers, decimals, and scientific notation    |
| `scanIdentifier()`| Parses function names and constants, validates against known sets |

#### Implementation Details

- **Recognized Functions**: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `log`, `ln`, `exp`, `sqrt`, `cbrt`, `abs`, `floor`, `ceil`, `factorial`
- **Recognized Constants**: `pi` (3.14159265358979323846), `e` (2.71828182845904523536)
- **Character Handling**: Case-insensitive identifier matching
- **Number Parsing**: Supports scientific notation with positive/negative exponents (e.g., `1.5e-10`, `3E8`)
- **Error Handling**: Throws `CalculatorException` for unknown identifiers and invalid number formats

---

### 4.2 Parser Module (`parser.h` / `parser.cpp`)

**Purpose**: Syntax Analysis — converts the sequential token stream into Reverse Polish Notation (postfix) using the Shunting Yard Algorithm.

#### Key Functions

| Function            | Description                                          |
|---------------------|------------------------------------------------------|
| `parse()`           | Main entry point; returns vector of RPN tokens       |
| `advance()`         | Fetches next token from tokenizer                    |
| `precedence(op)`    | Returns operator precedence level (higher = binds tighter) |
| `isRightAssoc(op)`  | Returns true for right-associative operators         |

#### Operator Precedence Table

| Operator | Precedence | Associativity | Description            |
|----------|------------|---------------|------------------------|
| `u-`     | 5          | Right         | Unary minus            |
| `^`      | 4          | Right         | Exponentiation         |
| `*` `/` `%` | 3       | Left          | Multiplicative         |
| `+` `-`  | 2          | Left          | Additive               |

#### Shunting Yard Implementation Details

**Internal Data Structures**:
- **Operator Stack**: Holds operator strings and function names during parsing, using `OpEntry` struct to distinguish between operators and functions
- **Arity Stack**: Parallel stack that tracks argument counts for active function calls

**Key Parsing Rules**:
1. **Numbers/Constants**: Directly appended to output queue
2. **Functions**: Pushed to operator stack with arity initialized to 1
3. **Unary Minus**: Detected when `-` appears where an operand is expected; treated as precedence-5 right-associative operator, emitted as `neg` function
4. **Unary Plus**: Consumed and discarded (no effect on expression)
5. **Operators**: Popped from stack to output based on precedence and associativity rules
6. **Left Parenthesis**: Pushed to stack as scope marker
7. **Right Parenthesis**: Pops all operators until matching left paren; emits any pending function
8. **Commas**: Increment arity counter for the innermost active function call

**Arity Tracking Mechanism**:
- When a function is encountered, arity is initialized to 1
- Each comma inside the function call increments the arity counter
- When the function is emitted, its arity is read from the stack top
- Validates arity is between 1 and 2; throws exception otherwise

---

### 4.3 Evaluator Module (`evaluator.h` / `evaluator.cpp`)

**Purpose**: Execution — evaluates the RPN token queue using a stack-based algorithm.

#### Key Functions

| Function             | Description                                       |
|----------------------|---------------------------------------------------|
| `evaluate(rpn)`      | Main entry point; processes RPN tokens, returns double result |

#### Evaluation Algorithm

```
For each token in RPN:
    If NUMBER or CONSTANT:
        Push value onto stack
    
    If OPERATOR:
        Pop 2 values (b, a) from stack
        Compute a OP b
        Push result onto stack
    
    If FUNCTION:
        Read arity from token
        Pop 'arity' values from stack
        Execute function with popped values
        Push result onto stack

Final result = stack top
```

#### Supported Operations

**Binary Operators**: `+`, `-`, `*`, `/`, `%` (fmod), `^` (pow)
**Unary Functions**: `neg` (unary minus, internally generated)
**Multi-argument Functions**: Dispatched to `math_functions::call()` (1 arg) or `math_functions::call2()` (2 args)

#### Error Conditions

- Insufficient operands for binary operators
- Insufficient arguments for functions
- Unsupported function arity
- Division by zero
- Modulo by zero
- Invalid token in RPN stream
- Stack not containing exactly 1 value at completion

---

### 4.4 Math Functions Module (`math_functions.h` / `math_functions.cpp`)

**Purpose**: Centralized mathematical computation logic — safe wrappers around `<cmath>` with domain validation and angle mode support.

#### Function Signatures

```cpp
double call(const std::string& name, double a);          // Single-argument functions
double call2(const std::string& name, double a, double b); // Two-argument functions
```

#### Single-Argument Functions

| Function    | Domain Check              | Computation                    | Notes                           |
|-------------|---------------------------|--------------------------------|---------------------------------|
| `neg`       | None                      | `-a`                           | Unary minus                     |
| `sin`       | None                      | `sin(deg ? toRad(a) : a)`      | Angle mode aware                |
| `cos`       | None                      | `cos(deg ? toRad(a) : a)`      | Angle mode aware                |
| `tan`       | None                      | `tan(deg ? toRad(a) : a)`      | Angle mode aware                |
| `asin`      | `[-1, 1]`                 | `asin(a)`, optionally to deg  | Domain validation               |
| `acos`      | `[-1, 1]`                 | `acos(a)`, optionally to deg  | Domain validation               |
| `atan`      | None                      | `atan(a)`, optionally to deg  |                                 |
| `sinh`      | None                      | `sinh(a)`                      |                                 |
| `cosh`      | None                      | `cosh(a)`                      |                                 |
| `tanh`      | None                      | `tanh(a)`                      |                                 |
| `log`       | `a > 0`                   | `log10(a)`                     | Base 10 logarithm               |
| `ln`        | `a > 0`                   | `log(a)`                       | Natural logarithm               |
| `exp`       | None                      | `exp(a)`                       | e^x                             |
| `sqrt`      | `a >= 0`                  | `sqrt(a)`                      | Square root                     |
| `cbrt`      | None                      | `cbrt(a)`                      | Cube root (handles negatives)   |
| `abs`       | None                      | `fabs(a)`                      | Absolute value                  |
| `floor`     | None                      | `floor(a)`                     | Floor function                  |
| `ceil`      | None                      | `ceil(a)`                      | Ceiling function                |
| `factorial` | `a >= 0`, `a` is integer, `a <= 170` | Iterative product | Throws for non-integers or overflow |

#### Two-Argument Functions

| Function | Domain Check                        | Computation           |
|----------|-------------------------------------|-----------------------|
| `log`    | `base > 0`, `base ≠ 1`, `x > 0`    | `log(x) / log(base)`  |

#### Angle Mode Integration

Trigonometric functions query the global `AngleMode` via `utils::getAngleMode()`:
- **Degree mode**: Input angles are converted to radians before computation; inverse trig functions return degrees
- **Radian mode**: Direct pass-through to `std::sin`, `std::cos`, etc.

---

### 4.5 Utils Module (`utils.h` / `utils.cpp`)

**Purpose**: Global configuration state and custom exception handling.

#### Components

**AngleMode Enum**
```cpp
enum class AngleMode { DEGREE, RADIAN };
```

**CalculatorException Class**
```cpp
class CalculatorException : public std::runtime_error {
public:
    explicit CalculatorException(const std::string& msg);
};
```

**Global State Management**
| Function                | Description                          |
|-------------------------|--------------------------------------|
| `utils::getAngleMode()` | Returns current angle mode (default: RADIAN) |
| `utils::setAngleMode()` | Sets angle mode for trigonometric functions   |

**Implementation**: Uses a file-scope static variable `g_angleMode` initialized to `AngleMode::RADIAN`, accessed through the `utils` namespace.

---

### 4.6 Main Module (`main.cpp`)

**Purpose**: Application entry point, REPL (Read-Eval-Print Loop) implementation, and CLI command handling.

#### Architecture

```
main()
  │
  ├── printBanner()          ← ASCII art startup display
  │
  └── REPL Loop
        │
        ├── Display prompt with angle mode indicator [RAD] or [DEG]
        │
        ├── Read line from stdin
        │
        ├── handleCommand()  ← Process CLI commands
        │     │
        │     ├── "quit"/"exit" → CommandResult::QUIT → break loop
        │     ├── "help"        → printHelp() → continue
        │     ├── "mode degree" → setAngleMode(DEGREE) → continue
        │     ├── "mode radian" → setAngleMode(RADIAN) → continue
        │     └── otherwise     → CommandResult::NOT_A_COMMAND
        │
        └── If not a command:
              │
              ├── Tokenizer tok(line)
              ├── Parser parser(tok)
              ├── rpn = parser.parse()
              ├── result = eval.evaluate(rpn)
              └── Print formatted result
```

#### Helper Functions

| Function              | Description                                        |
|-----------------------|----------------------------------------------------|
| `trim()`              | Removes leading/trailing whitespace                |
| `toLower()`           | Converts string to lowercase for command matching  |
| `formatResult()`      | Formats double for display with smart rules        |
| `modeTag()`           | Returns "DEG" or "RAD" based on current mode       |
| `printBanner()`       | Displays ASCII art calculator banner               |
| `printHelp()`         | Displays formatted help guide with box characters  |
| `handleCommand()`     | Parses and executes CLI commands                   |

#### Result Formatting Logic (`formatResult()`)

1. **Near-zero snapping**: Values with absolute magnitude < 1e-14 are snapped to 0.0
2. **Integer detection**: If value equals its floor and is < 1e15, display without decimals
3. **Float formatting**: Otherwise, use 12 significant digits, then strip trailing zeros

#### Platform Handling

- On Windows (`_WIN32`): Sets console output code page to UTF-8 (65001) for proper box-drawing character display

---

### 4.7 Build Configuration (`CMakeLists.txt`)

**Purpose**: CMake build script for cross-platform compilation.

**Key Settings**:
- Minimum CMake version: 3.10
- C++ Standard: C++17
- Target: Executable named `calculator`
- Sources: All `.cpp` files in the `calculator/` directory

---

## 5. Data Flow & Pipeline

### Expression Evaluation Example: `sin(pi/2) + 3^2`

#### Stage 1: Tokenization

Input: `"sin(pi/2) + 3^2"`

Output Token Stream:
```
[FUNCTION "sin"] [LEFT_PAREN] [CONSTANT "pi" val=3.14159...] [OPERATOR "/"] [NUMBER 2] [RIGHT_PAREN] [OPERATOR "+"] [NUMBER 3] [OPERATOR "^"] [NUMBER 2] [END]
```

#### Stage 2: Parsing (Shunting Yard)

Input Token Stream → RPN Conversion:

Processing:
1. `sin` → push to opStack, arity=1
2. `(` → push to opStack
3. `pi` → output queue
4. `/` → push to opStack
5. `2` → output queue
6. `)` → pop `/` to output, pop `(`, emit `sin` with arity=1
7. `+` → push to opStack
8. `3` → output queue
9. `^` → push to opStack (higher precedence than `+`)
10. `2` → output queue
11. End → pop `^`, then `+` to output

Final RPN Queue:
```
[CONSTANT "pi"] [NUMBER 2] [OPERATOR "/"] [FUNCTION "sin" arity=1] [NUMBER 3] [NUMBER 2] [OPERATOR "^"] [OPERATOR "+"]
```

#### Stage 3: Evaluation (Stack-based)

| Step | Token          | Stack After                    | Action                        |
|------|----------------|--------------------------------|-------------------------------|
| 1    | CONSTANT pi    | [3.14159...]                   | Push constant value           |
| 2    | NUMBER 2       | [3.14159..., 2]               | Push number                   |
| 3    | OPERATOR /     | [1.57079...]                   | Pop 2, divide, push result    |
| 4    | FUNCTION sin   | [1.0]                          | Pop 1, sin(), push result     |
| 5    | NUMBER 3       | [1.0, 3]                      | Push number                   |
| 6    | NUMBER 2       | [1.0, 3, 2]                   | Push number                   |
| 7    | OPERATOR ^     | [1.0, 9]                       | Pop 2, 3^2=9, push result     |
| 8    | OPERATOR +     | [10.0]                         | Pop 2, 1+9=10, push result    |

**Final Result**: `10`

---

## 6. Building & Running

### Prerequisites

- **CMake** 3.10 or higher
- **C++17 compatible compiler** (GCC, Clang, MSVC)

### CMake Build (Recommended)

```bash
# Navigate to project root
cd cli-calculator

# Create and enter build directory
mkdir build && cd build

# Configure CMake
cmake ../calculator

# Build
cmake --build .

# Run
./bin/calculator      # Linux/macOS
bin\calculator.exe    # Windows
```

### Direct Compilation (Alternative)

```bash
cd calculator
g++ -std=c++17 -Wall -Wextra -O3 *.cpp -o calculator
./calculator
```

### Windows PowerShell Quick Build

```powershell
Set-Location calculator
g++ -std=c++17 -Wall -Wextra -O2 *.cpp -o calculator.exe
.\calculator.exe
```

---

## 7. Usage Guide

### Interactive Mode (REPL)

The calculator runs in an interactive Read-Eval-Print Loop:

```
  ╔═══════════════════════════════════════════════════════╗
  ║          Scientific CLI Calculator  v1.0.0            ║
  ╚═══════════════════════════════════════════════════════╝

  [RAD] > 2 + 3 * 4
  = 14
  
  [RAD] > sin(pi/2)
  = 1
  
  [RAD] > mode degree
  >> Angle mode set to DEGREE
  
  [DEG] > sin(30)
  = 0.5
  
  [DEG] > quit
  Goodbye!
```

### Supported Expressions

| Category          | Examples                                    | Result      |
|-------------------|---------------------------------------------|-------------|
| Basic arithmetic  | `2 + 3 * 4`                                 | `14`        |
| Parentheses       | `(2 + 3) * 4`                               | `20`        |
| Exponentiation    | `3^3^2` (right-assoc: `3^(3^2)`)           | `19683`     |
| Trig (radians)    | `sin(pi/2)`                                 | `1`         |
| Trig (degrees)    | `sin(30)` (in degree mode)                  | `0.5`       |
| Logarithms        | `log(100)`                                  | `2`         |
| Custom base log   | `log(2, 8)`                                 | `3`         |
| Factorial         | `factorial(5)`                              | `120`       |
| Square root       | `sqrt(16) + abs(-5)`                        | `9`         |
| Scientific notation| `3E8 * 1.5e-10`                            | `0.045`     |
| Near-zero         | `sin(pi)`                                   | `0` (snapped)|

### CLI Commands

| Command           | Description                                  |
|-------------------|----------------------------------------------|
| `help`            | Display full usage guide with examples       |
| `mode degree`     | Switch trigonometric functions to degrees    |
| `mode radian`     | Switch trigonometric functions to radians    |
| `quit` / `exit`   | Exit the calculator                          |

---

## 8. Error Handling

The calculator provides comprehensive error handling through the `CalculatorException` class, catching and displaying user-friendly messages for various error conditions.

### Error Categories

| Error Type          | Example Input            | Error Message                              |
|---------------------|--------------------------|--------------------------------------------|
| **Syntax Errors**   |                          |                                            |
| Unknown identifier  | `foo(5)`                 | `Unknown identifier: foo`                  |
| Invalid number      | `1.2.3`                  | `Invalid number: 1.2.3`                    |
| Mismatched parens   | `(2 + 3`                 | `Mismatched parentheses: unclosed '('`     |
| Mismatched parens   | `2 + 3)`                 | `Missing expression before ')'`            |
| Missing args        | `sin()`                  | `Missing argument before ','` or similar   |
| **Domain Errors**   |                          |                                            |
| Sqrt negative       | `sqrt(-1)`               | `sqrt: argument must be non-negative`      |
| Log non-positive    | `log(0)`                 | `log: argument must be positive`           |
| Inverse trig range  | `asin(2)`                | `asin: argument must be in [-1, 1]`        |
| Factorial invalid   | `factorial(-3)`          | `factorial: argument must be non-negative integer` |
| Factorial overflow  | `factorial(171)`         | `factorial: argument too large (max 170)`  |
| **Runtime Errors**  |                          |                                            |
| Division by zero    | `5 / 0`                  | `Division by zero`                         |
| Modulo by zero      | `5 % 0`                  | `Modulo by zero`                           |
| Insufficient args   | `+ 3`                    | `Insufficient operands for '+'`            |
| Function arity      | `log(5)` (expects 1 or 2)| Handled by parser arity validation         |

---

## 9. Design Principles

1. **Separation of Concerns**: Each module (Tokenizer, Parser, Evaluator, MathFunctions, Utils) has a single, well-defined responsibility
2. **Pipeline Architecture**: Data flows linearly through tokenization → parsing → evaluation, making the system predictable and debuggable
3. **Stack-based Evaluation**: Uses the standard RPN evaluation pattern, which is efficient and well-understood
4. **Error Propagation**: Exceptions bubble up from any stage with meaningful, user-friendly error messages
5. **Extensibility**: Adding a new math function requires:
   - Adding the function name to the `FUNCTIONS` set in `tokenizer.cpp`
   - Implementing the computation in `math_functions.cpp`
   - The Parser and Evaluator automatically handle the new function through dynamic arity tracking
6. **Modern C++ Practices**: Uses C++17 features, RAII, namespaces, enums, and structured error handling
7. **Precision Handling**: Uses `double` precision with near-zero snapping and intelligent output formatting

---

## 10. Future Enhancements

### Planned for Version 2

| Feature              | Description                                      | Complexity |
|----------------------|--------------------------------------------------|------------|
| **Variables**        | Store and recall values (`x = 5`, `x * 2`)       | Medium     |
| **Expression History** | View, search, and reuse previous expressions   | Medium     |
| **Additional Functions** | Gamma function, Bessel functions, statistical functions | Low  |
| **Web API**          | REST API endpoint for web-based calculator access| High       |
| **Expression Files** | Load and evaluate expressions from files         | Low        |
| **Complex Numbers**  | Support for complex number arithmetic            | High       |
| **Matrix Operations**| Basic matrix math (determinant, inverse, etc.)   | High       |
| **Unit Conversion**  | Built-in unit conversion capabilities            | Medium     |
| **Custom Functions** | User-defined function definitions                | High       |

### Implementation Notes for Variables

To add variable support, the following changes would be needed:
- **Tokenizer**: Recognize identifiers that aren't functions/constants as potential variable names
- **Parser**: Handle assignment operator (`=`) as a special case with lowest precedence
- **Evaluator**: Add a symbol table (map of string → double) to store and retrieve variable values
- **Utils**: Add symbol table management to the global configuration

---

## Appendix A: Token Type Flow Diagram

```
Input String
     │
     ▼
┌─────────────────────────────────┐
│          TOKENIZER              │
│                                 │
│  Numbers → NUMBER               │
│  + - * / % ^ → OPERATOR         │
│  ( ) → LEFT_PAREN, RIGHT_PAREN  │
│  sin, cos, ... → FUNCTION       │
│  pi, e → CONSTANT               │
│  , → COMMA                      │
│  End of string → END            │
│  Unknown → CalculatorException  │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│          PARSER                 │
│    (Shunting Yard Algorithm)    │
│                                 │
│  Handles:                       │
│  • Operator precedence          │
│  • Right-associativity (^)      │
│  • Unary minus (→ neg function) │
│  • Function arity tracking      │
│  • Parentheses balancing        │
│  • Comma counting for functions │
│                                 │
│  Output: RPN Token Vector       │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│         EVALUATOR               │
│    (Stack-based Execution)      │
│                                 │
│  NUMBER/CONSTANT → push to stack│
│  OPERATOR → pop 2, compute, push│
│  FUNCTION → pop N, call, push   │
│                                 │
│  Final: stack top = result      │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│      MATH_FUNCTIONS             │
│    (cmath wrappers + domain     │
│     validation + angle mode)    │
└─────────────────────────────────┘
```

---

## Appendix B: Complete Function Reference

### Operators

| Symbol | Precedence | Associativity | Description           |
|--------|------------|---------------|-----------------------|
| `^`    | 4          | Right         | Exponentiation        |
| `*`    | 3          | Left          | Multiplication        |
| `/`    | 3          | Left          | Division              |
| `%`    | 3          | Left          | Modulo (fmod)         |
| `+`    | 2          | Left          | Addition              |
| `-`    | 2          | Left          | Subtraction           |
| `-` (unary) | 5     | Right         | Unary negation        |

### Functions

| Name        | Arity | Description                        |
|-------------|-------|------------------------------------|
| `sin`       | 1     | Sine (angle mode aware)            |
| `cos`       | 1     | Cosine (angle mode aware)          |
| `tan`       | 1     | Tangent (angle mode aware)         |
| `asin`      | 1     | Arc sine, returns angle            |
| `acos`      | 1     | Arc cosine, returns angle          |
| `atan`      | 1     | Arc tangent, returns angle         |
| `sinh`      | 1     | Hyperbolic sine                    |
| `cosh`      | 1     | Hyperbolic cosine                  |
| `tanh`      | 1     | Hyperbolic tangent                 |
| `log`       | 1 or 2| Base-10 log (1 arg) or custom base |
| `ln`        | 1     | Natural logarithm                  |
| `exp`       | 1     | e raised to power x                |
| `sqrt`      | 1     | Square root                        |
| `cbrt`      | 1     | Cube root                          |
| `abs`       | 1     | Absolute value                     |
| `floor`     | 1     | Floor function                     |
| `ceil`      | 1     | Ceiling function                   |
| `factorial` | 1     | Factorial (non-negative integers)  |

### Constants

| Name | Value                          |
|------|--------------------------------|
| `pi` | 3.14159265358979323846         |
| `e`  | 2.71828182845904523536         |

---

*This document provides a comprehensive overview of the CLI Scientific Calculator project. For quick reference, see README.md. For repository-specific guidance, see REPO.md.*