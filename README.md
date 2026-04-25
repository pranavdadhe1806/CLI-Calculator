# Scientific Calculator

A production-quality CLI-based scientific calculator built in C++ with clean, modular architecture. This calculator supports basic arithmetic, trigonometric functions, logarithms, and advanced mathematical operations.

## Features

### Basic Operations
- Arithmetic: `+`, `-`, `*`, `/`, `%` (modulo), `^` (power)
- Operator precedence and associativity
- Parentheses grouping: `(2 + 3) * 4`

### Scientific Functions
- **Trigonometric**: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
  - Supports both degree and radian modes
- **Hyperbolic**: `sinh`, `cosh`, `tanh`
- **Logarithmic**: 
  - `log(x)` - base 10 logarithm
  - `ln(x)` - natural logarithm
  - `log(a, b)` - logarithm base a of b
  - `exp(x)` - e raised to the power x
- **Roots**: `sqrt(x)`, `cbrt(x)`
- **Utility**: `abs(x)`, `floor(x)`, `ceil(x)`, `factorial(x)`

### Constants
- `pi` - π (3.141592653589793...)
- `e` - Euler's number (2.718281828459045...)

### Advanced Features
- Unary minus support: `-5`, `-(2 + 3)`
- Right-associative power operator: `3^3^2 = 3^(3^2)`
- Multi-argument functions: `log(10, 100)`
- Comprehensive error handling with user-friendly messages

## Architecture

The calculator uses a three-stage pipeline architecture:

```
User Input → Tokenizer → Parser → Evaluator → Result
```

### Modules

1. **Tokenizer** (`tokenizer.h/cpp`)
   - Lexical analysis: converts input string into tokens
   - Handles numbers (including scientific notation), operators, functions, constants

2. **Parser** (`parser.h/cpp`)
   - Implements Shunting Yard Algorithm
   - Converts infix notation to postfix (RPN)
   - Handles operator precedence, associativity, and function calls

3. **Evaluator** (`evaluator.h/cpp`)
   - Stack-based RPN evaluation
   - Executes operations and function calls
   - Error detection and reporting

4. **Math Functions** (`math_functions.h/cpp`)
   - Wrapper around `<cmath>` with angle mode support
   - Domain validation and error handling

5. **Utils** (`utils.h/cpp`)
   - Custom exception class (`CalculatorException`)
   - Angle mode management (degree/radian)

6. **Main** (`main.cpp`)
   - CLI interface with REPL loop
   - Command handling and user interaction

## Building

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ../calculator

# Build
cmake --build .

# Run
./bin/calculator    # Linux/macOS
# or
bin\calculator.exe  # Windows
```

### Alternative: Direct Compilation

```bash
cd calculator
g++ -std=c++17 -Wall -Wextra -O3 *.cpp -o calculator
./calculator
```

## Usage

### Running the Calculator

Start the calculator and enter expressions:

```
> 2 + 3 * 4
14

> sin(pi/2)
1

> log(100) + sqrt(16)
8

> 3^3^2
19683
```

### Commands

- `mode degree` - Switch to degree mode for trigonometric functions
- `mode radian` - Switch to radian mode (default)
- `help` - Show help message
- `quit` or `exit` - Exit the calculator

### Examples

```
> mode degree
Angle mode set to DEGREE

> sin(30)
0.5

> mode radian
Angle mode set to RADIAN

> sin(pi/2)
1

> log(100)
2

> log(2, 8)
3

> factorial(5)
120

> sqrt(16) + abs(-5)
9
```

## Manual Testing (Phase 4)

- Full manual suite: [MANUAL_TEST_SUITE.md](MANUAL_TEST_SUITE.md)
- Validation scope: arithmetic, precedence/associativity, functions/constants, angle mode, parser errors, evaluator/runtime errors
- Latest verification run: **2026-04-25**
- Result: **26 / 26 test cases passed**

### Quick Validation Command (Windows PowerShell)

```powershell
Set-Location calculator
g++ -std=c++17 -Wall -Wextra -O2 *.cpp -o calculator.exe
```

Then run the expressions listed in [MANUAL_TEST_SUITE.md](MANUAL_TEST_SUITE.md) and compare outputs.

## Design Principles

1. **Separation of Concerns**: Each module has a single, well-defined responsibility
2. **Pipeline Architecture**: Data flows linearly through tokenization, parsing, and evaluation
3. **Stack-based Evaluation**: Standard RPN evaluation pattern for postfix expressions
4. **Error Propagation**: Exceptions bubble up with meaningful error messages
5. **Extensibility**: Easy to add new functions or operators

## Implementation Details

### Shunting Yard Algorithm
The parser uses the classic Shunting Yard algorithm by Edsger Dijkstra to convert infix expressions to postfix notation. This handles:
- Operator precedence (power > multiply/divide > add/subtract)
- Right-associativity for power operator (`^`)
- Left-associativity for other operators
- Function calls with variable arguments
- Parentheses grouping

### Error Handling
- **Syntax Errors**: Invalid expressions, mismatched parentheses
- **Domain Errors**: sqrt(-1), log(0), asin(2)
- **Runtime Errors**: Division by zero, insufficient operands
- All errors are caught and displayed with helpful messages

## Future Enhancements (Version 2)

- **Web API**: REST API endpoint for web-based calculator
- **Additional Functions**: Gamma function, statistical functions
- **Variables**: Store and use variables (e.g., `x = 5`, `x * 2`)
- **Expression History**: View and reuse previous expressions

## License

This project is for educational and portfolio purposes.

## Contributing

This is a learning project. Suggestions and improvements are welcome!
