# Scientific Calculator Development Plan

## Architecture Overview

The calculator uses a three-stage pipeline:

1. **Tokenizer** (Lexical Analysis) → breaks input string into tokens
2. **Parser** (Syntax Analysis) → converts infix to postfix (RPN) using Shunting Yard
3. **Evaluator** (Execution) → evaluates postfix expression using stack

### Data Flow

```
User Input → Tokenizer → Parser → Evaluator → Result
              ↓           ↓          ↓
         [Tokens]   [RPN Tokens]  [Math Functions]
```

## Module Breakdown

### 1. Tokenizer (`tokenizer.h/cpp`)

**Purpose**: Lexical analyzer that converts input string into tokens

**Key Design Decisions**:

- Case-insensitive function/constant matching (convert to lowercase)
- Support scientific notation (1.5e-10)
- Distinguish unary vs binary `-` (handled in parser context)

**Token Types**:

- `NUMBER` (int/float)
- `OPERATOR` (+, -, *, /, %, ^)
- `LEFT_PAREN`, `RIGHT_PAREN`
- `FUNCTION` (sin, cos, log, etc.)
- `CONSTANT` (pi, e)
- `COMMA` (for multi-arg functions: log(a, b))
- `END` (end of input marker)

**Algorithms**:

- Sequential character-by-character parsing
- State machine for numbers (integer part → decimal → exponent)

### 2. Parser (`parser.h/cpp`)

**Purpose**: Convert infix to postfix using Shunting Yard Algorithm

**Key Design Decisions**:

- Right-associative operators: `^` (power)
- Left-associative operators: `+`, `-`, `*`, `/`, `%`
- Precedence levels:
  - Level 4: `^`
  - Level 3: `*`, `/`, `%`
  - Level 2: `+`, `-`
- Functions have implicit precedence (higher than operators)
- Handle multi-argument functions (log base conversion)

**Shunting Yard Algorithm**:

1. For each token:

   - NUMBER → output
   - FUNCTION → push to operator stack
   - OPERATOR → pop higher/equal precedence ops to output, then push
   - LEFT_PAREN → push to stack
   - RIGHT_PAREN → pop until LEFT_PAREN found
   - COMMA → pop until LEFT_PAREN (handles function args)

2. Pop remaining operators to output

**Special Cases**:

- Unary minus: detected when `-` follows `(`, `,`, start, or operator
- Function calls: `sin(30)` → function marked for special handling
- Constants: `pi`, `e` → converted to numbers in output

### 3. Evaluator (`evaluator.h/cpp`)

**Purpose**: Evaluate postfix (RPN) expression using stack

**Algorithm**:

1. For each RPN token:

   - NUMBER → push to stack
   - OPERATOR → pop 2 operands, compute, push result
   - FUNCTION → pop required args (1 or 2), call math function, push result
   - CONSTANT → push numeric value to stack

2. Final stack should contain one number (the result)

**Error Handling**:

- Division by zero
- Insufficient operands for operator
- Invalid function arguments (e.g., log(0), sqrt(-1) if domain error)
- Stack underflow/overflow

### 4. Math Functions (`math_functions.h/cpp`)

**Purpose**: Wrapper around `<cmath>` with degree/radian mode support

**Design**:

- Global mode flag: `ANGLE_MODE` (DEGREE or RADIAN)
- Wrapper functions that convert degrees → radians for trig functions
- Domain validation (e.g., `asin` must be in [-1, 1])

**Functions**:

- Trig: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- Hyperbolic: `sinh`, `cosh`, `tanh` (bonus)
- Log/Exp: `log` (base 10), `ln` (natural), `exp`, `log(a, b)` (log base a of b)
- Roots: `sqrt`, `cbrt`
- Utility: `abs`, `floor`, `ceil`
- Advanced: `factorial(n)` using iterative or gamma function

**Constants**:

- `pi` = 3.14159265358979323846
- `e` = 2.71828182845904523536

### 5. Utils (`utils.h/cpp`)

**Purpose**: Helper functions and error handling

**Components**:

- `CalculatorException` custom exception class
- `AngleMode` enum (DEGREE, RADIAN)
- Mode getter/setter functions
- Error message formatting

### 6. Main (`main.cpp`)

**Purpose**: CLI interface

**Features**:

- REPL loop: read expression → evaluate → print result
- Command handling: `mode degree`, `mode radian`, `quit`, `help`
- Error display with clear messages
- Optional: history of expressions

## Implementation Order

### Phase 1: Foundation (Core Pipeline)

1. Create project structure (headers + empty implementations)
2. Implement `Token` and `TokenType` enum
3. Implement `Tokenizer` (basic numbers, operators, parentheses)
4. Implement `Parser` skeleton (Shunting Yard for basic arithmetic)
5. Implement `Evaluator` (basic arithmetic operations)
6. Test: `2 + 3 * 4` should yield `14`

### Phase 2: Advanced Features

7. Add function tokenization (`sin`, `cos`, etc.)
8. Extend parser for functions and constants
9. Implement `MathFunctions` module
10. Add constants (`pi`, `e`)
11. Test: `sin(pi/2)` in radian mode should yield `1`

### Phase 3: Polish & Error Handling

12. Robust error handling (exceptions, try-catch)
13. Degree/radian mode support
14. Multi-argument functions (`log(a, b)`)
15. Edge cases: unary minus, nested parentheses, complex expressions
16. CLI commands (`mode`, `help`, `quit`)

### Phase 4: Testing & Documentation

17. Test suite with known expressions
18. README with architecture docs
19. Build system (CMakeLists.txt)

## File Structure

```
calculator/
├── main.cpp                 # CLI entry point
├── tokenizer.h/cpp          # Lexical analyzer
├── parser.h/cpp             # Shunting Yard parser
├── evaluator.h/cpp          # RPN evaluator
├── math_functions.h/cpp     # Math function wrappers
├── utils.h/cpp              # Utilities and error handling
├── CMakeLists.txt           # Build configuration
└── README.md                # Documentation
```

## Key Design Patterns

1. **Separation of Concerns**: Each module has single responsibility
2. **Pipeline Architecture**: Data flows linearly through stages
3. **Stack-based Evaluation**: Standard RPN evaluation pattern
4. **Error Propagation**: Exceptions bubble up to main for user-friendly messages

## Common Pitfalls to Avoid

1. **Operator Precedence**: `^` is right-associative (3^3^2 = 3^(3^2) = 19683, not (3^3)^2)
2. **Unary Minus**: Distinguish from binary subtraction (`-5` vs `5-3`)
3. **Function Arguments**: Handle both 1-arg (`sin(x)`) and 2-arg (`log(a,b)`) functions
4. **Angle Mode**: All trig functions should respect DEGREE/RADIAN mode
5. **Domain Errors**: Validate inputs (sqrt(-1), log(0), asin(2))
6. **Parentheses Matching**: Ensure balanced parentheses (detect in parser)
7. **Floating Point Precision**: Use `double`, be aware of precision limits

## Future: Version 2 (Web API)

- Keep calculator engine completely separate from I/O
- Create new `api_server.cpp` that uses same `Evaluator` class
- REST endpoint: `POST /calculate` with JSON `{"expression": "sin(30)"}`
- Return JSON: `{"result": 0.5, "error": null}`
- No changes needed to core calculator modules
