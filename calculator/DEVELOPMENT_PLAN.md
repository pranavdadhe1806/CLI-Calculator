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

**Token Struct**:

```cpp
struct Token {
    TokenType type;
    double    value;   // NUMBER, CONSTANT
    std::string name;  // OPERATOR name, FUNCTION name
    int       arity;   // FUNCTION only: 1 or 2 (set by Parser, not Tokenizer)
};
```

> **Note**: The `arity` field is always 0 when the Tokenizer produces a FUNCTION token.
> The Parser sets the correct value (1 or 2) by counting commas inside the function's parentheses.
> The Evaluator reads `token.arity` directly — it never guesses from stack size.

**Algorithms**:

- Sequential character-by-character parsing
- State machine for numbers (integer part → decimal → exponent)

### 2. Parser (`parser.h/cpp`)

**Purpose**: Convert infix to postfix using Shunting Yard Algorithm

**Key Design Decisions**:

- Right-associative operators: `^` (power)
- Left-associative operators: `+`, `-`, `*`, `/`, `%`
- Precedence levels:
  - Level 5: unary minus (`u-`)
  - Level 4: `^`
  - Level 3: `*`, `/`, `%`
  - Level 2: `+`, `-`
- Functions are pushed onto the operator stack; popped to output after their closing `)`.
- Arity is determined by counting COMMAs at the same nesting depth inside a function call.

**Arity Tracking**:

- Maintain a `std::stack<int> arityStack` that mirrors the operator stack's function frames.
- When a FUNCTION token is seen: push `1` onto `arityStack`.
- When a COMMA is seen at the top-level of the current function's parens: increment `arityStack.top()`.
- When `)` pops a function off the operator stack: pop `arityStack`, set `token.arity` on the emitted FUNCTION token.

**Shunting Yard Algorithm**:

1. For each token:

   - NUMBER / CONSTANT → output (arity unchanged)
   - FUNCTION → push to operator stack; push `1` to arityStack
   - OPERATOR → pop higher/equal precedence ops to output, then push
   - LEFT_PAREN → push to stack
   - RIGHT_PAREN → pop until LEFT_PAREN found; if top of op stack is a function, pop it to output with its arity
   - COMMA → pop until LEFT_PAREN; increment `arityStack.top()`

2. Pop remaining operators to output

**Special Cases**:

- Unary minus: detected when `-` follows `(`, `,`, start, or operator → pushed as `"u-"` with arity=1
- Constants: `pi`, `e` → emitted directly to output as CONSTANT tokens (numeric value already set by Tokenizer)

### 3. Evaluator (`evaluator.h/cpp`)

**Purpose**: Evaluate postfix (RPN) expression using stack

**Algorithm**:

1. For each RPN token:

   - NUMBER / CONSTANT → push `token.value` to stack
   - OPERATOR → pop 2 operands, compute, push result
   - FUNCTION → read `token.arity`; pop exactly that many args; call math function; push result

2. Final stack must contain exactly one number (the result)

**Error Handling**:

- Division by zero
- Insufficient operands for operator / function
- Invalid function arguments (e.g., log(0), sqrt(-1))
- Stack size != 1 at end

### 4. Math Functions (`math_functions.h/cpp`)

**Purpose**: Wrapper around `<cmath>` with degree/radian mode support

**Design**:

- Global mode flag managed by `utils::getAngleMode()` / `utils::setAngleMode()`
- Wrapper functions that convert degrees → radians for trig functions
- Domain validation (e.g., `asin` must be in [-1, 1])
- `call(name, a)` — 1-argument dispatch
- `call2(name, a, b)` — 2-argument dispatch (currently only `log(base, value)`)

**Functions**:

- Trig: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- Hyperbolic: `sinh`, `cosh`, `tanh`
- Log/Exp: `log(x)` (base 10), `ln(x)` (natural), `exp(x)`, `log(base, x)` (change of base)
- Roots: `sqrt`, `cbrt`
- Utility: `abs`, `floor`, `ceil`, `factorial`

**Constants**:

- `pi` = 3.14159265358979323846
- `e` = 2.71828182845904523536

### 5. Utils (`utils.h/cpp`)

**Purpose**: Shared types and error handling

**Components**:

- `CalculatorException` — custom exception inheriting `std::runtime_error`
- `AngleMode` enum: `DEGREE`, `RADIAN`
- `utils::getAngleMode()` / `utils::setAngleMode()` — global mode state

### 6. Main (`main.cpp`)

**Purpose**: CLI interface

**Features**:

- REPL loop: read expression → tokenize → parse → evaluate → print result
- Commands: `mode degree`, `mode radian`, `help`, `quit` / `exit`
- All exceptions caught and displayed with a clear `Error: ...` prefix

## Implementation Order

### Phase 1: Foundation (Core Pipeline)

1. Create project structure (`calculator/` subdirectory with all source files + CMakeLists.txt)
2. Implement `Token` struct (with `arity` field) and `TokenType` enum in `tokenizer.h`
3. Implement `Tokenizer` — numbers, operators, parentheses, comma, identifiers
4. Implement `Parser` — Shunting Yard for basic arithmetic (no functions yet)
5. Implement `Evaluator` — basic arithmetic (+, -, *, /, %, ^)
6. Wire together in `main.cpp`; verify `2 + 3 * 4 = 14`, `3^3^2 = 19683`

### Phase 2: Functions & Constants

7. Extend Tokenizer to recognize function names and constants (`pi`, `e`)
8. Extend Parser with arity tracking for function calls
9. Implement `MathFunctions` — all 1-arg and 2-arg functions
10. Implement unary minus (`u-`) support in Parser and Evaluator
11. Verify: `sin(pi/2) = 1`, `log(100) = 2`, `log(2, 8) = 3`, `factorial(5) = 120`

### Phase 3: Polish & Error Handling

12. Robust error handling (mismatched parens, domain errors, division by zero)
13. Degree/radian mode support in `utils` + math functions
14. CLI commands (`mode`, `help`, `quit`) in `main.cpp`
15. Edge cases: `-(2+3)`, `--5`, nested functions `sqrt(sin(pi/6)^2 + cos(pi/6)^2)`

### Phase 4: Testing & Documentation

16. Manual test suite — table of expressions with expected results
17. Verify all test cases pass
18. Update README

## File Structure

```
cli-calculator/
├── calculator/
│   ├── main.cpp             # CLI entry point
│   ├── tokenizer.h/cpp      # Lexical analyzer
│   ├── parser.h/cpp         # Shunting Yard parser (sets token.arity)
│   ├── evaluator.h/cpp      # RPN evaluator (reads token.arity)
│   ├── math_functions.h/cpp # Math function wrappers
│   ├── utils.h/cpp          # CalculatorException, AngleMode
│   └── CMakeLists.txt       # Build configuration
├── build/                   # Out-of-source build directory (git-ignored)
├── DEVELOPMENT_PLAN.md
└── README.md
```

## Key Design Patterns

1. **Separation of Concerns**: Each module has single responsibility
2. **Pipeline Architecture**: Data flows linearly through stages
3. **Arity in Token**: Parser is the single source of truth for function argument count; Evaluator never guesses from stack size
4. **Stack-based Evaluation**: Standard RPN evaluation pattern
5. **Error Propagation**: Exceptions bubble up to `main` for user-friendly messages

## Common Pitfalls to Avoid

1. **Operator Precedence**: `^` is right-associative (`3^3^2 = 3^(3^2) = 19683`)
2. **Unary Minus**: Distinguish from binary subtraction; treat as a 1-arg function `neg` in RPN
3. **Function Arity**: Always set `token.arity` in the Parser; never infer it from stack size in the Evaluator
4. **Angle Mode**: All trig functions must respect DEGREE/RADIAN mode
5. **Domain Errors**: Validate inputs (`sqrt(-1)`, `log(0)`, `asin(2)`)
6. **Parentheses Matching**: Detect mismatches in the Parser, not silently in the Evaluator
7. **Floating Point Precision**: Use `double`; results like `sin(pi)` will be ~1e-16, not exactly 0

## Future: Version 2 (Web API)

- Keep calculator engine (`tokenizer`, `parser`, `evaluator`, `math_functions`) completely decoupled from I/O
- Add `api_server.cpp` using the same pipeline
- REST endpoint: `POST /calculate` with JSON `{"expression": "sin(30)"}`
- Return JSON: `{"result": 0.5, "error": null}`
- Zero changes needed to core calculator modules
