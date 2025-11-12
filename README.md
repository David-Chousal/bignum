# BigNum Implementation

A C++ implementation of arbitrary-precision integers (BigNum) that can handle numbers of virtually unlimited size.

## Overview

The BigNum class stores integers internally as a polynomial in base 256, where each coefficient (digit) is stored as an `unsigned char`. This allows for efficient storage and arithmetic operations on very large numbers.

### Internal Representation

Each BigNum is stored as:
```
digits[0] + digits[1]*256 + digits[2]*256² + digits[3]*256³ + ...
```

Where:
- `digits[i]` is an `unsigned char` (0-255)
- The base is 256 (2^8)
- Numbers are stored in little-endian format (least significant digit first)

## Implementation Details

### Files Structure

- **`bignum.h`** - Header file with class declaration
- **`bignum.cxx`** - Implementation of string constructor and helper functions
- **`bignumops.cxx`** - Implementation of operators and member functions
- **`bignummain.cxx`** - Main program for interactive testing
- **`Makefile`** - Build configuration
- **`test_bignum.py`** - Automated test script
- **`test_all.txt`** - Test data file

### Implemented Features

#### 1. Rule of 4 (Memory Management)

The "Rule of 4" consists of four essential member functions for proper resource management:

##### Destructor (`~BigNum()`)
- Frees the dynamically allocated `digits` array
- Automatically called when object goes out of scope

##### Copy Constructor (`BigNum(const BigNum &rhs)`)
- Creates a new BigNum as an exact copy of another
- Initializes members to safe defaults, then calls assignment operator
- Ensures deep copy of all resources

##### Copy Assignment Operator (`operator=(const BigNum &rhs)`)
- Handles self-assignment check
- Calls `deepCopy()` to perform deep copy of resources
- Returns reference to `*this` for chaining

##### Move Assignment Operator (`operator=(BigNum &&rhs)`)
- Efficiently moves resources from source object
- Transfers ownership of `digits` array
- Leaves source in valid but empty state (nullptr, zero values)
- Prevents resource leaks and unnecessary copying

#### 2. Constructor with `long` Argument

```cpp
BigNum(const long &n)
```

- Converts a standard `long` integer to BigNum's base-256 representation
- Handles positive and negative numbers
- Handles zero case
- Uses repeated division by 256 to extract digits

**Algorithm:**
1. Handle sign (store separately)
2. Repeatedly divide by 256, storing remainders as digits
3. Digits are stored from least significant to most significant

#### 3. Addition Operator (`operator+`)

```cpp
friend BigNum operator+(const BigNum &lhs, const BigNum &rhs)
```

Implemented as a **friend function** (non-member) following C++ best practices for binary operators.

**Algorithm:**

The addition follows polynomial arithmetic principles:

1. **Same Signs (Addition):**
   - Add corresponding coefficients (digits) of the same order
   - If sum exceeds base (256), carry to next higher order
   - Continue until all digits processed and no carry remains

2. **Different Signs (Subtraction):**
   - Determine which number has larger absolute value
   - Subtract smaller from larger
   - Result sign matches the larger number's sign
   - Handle borrow when digit subtraction underflows

3. **Normalization:**
   - Remove leading zero coefficients
   - Handle zero result (set sign to positive)

**Key Features:**
- Uses `buffer_t` (unsigned short) for intermediate calculations to prevent overflow
- Properly handles carry propagation
- Correctly handles borrow in subtraction
- Normalizes result to remove leading zeros

#### 4. Deep Copy Helper (`deepCopy()`)

```cpp
void deepCopy(const BigNum &rhs)
```

- Allocates new memory for `digits` array
- Copies all member variables (high, sign, capacity)
- Uses `memcpy` for efficient array copying
- Handles null/empty cases safely
- Throws `std::bad_alloc` if memory allocation fails

#### 5. String Constructor

Already implemented in `bignum.cxx`:
- Parses decimal string representation
- Handles optional sign character (+ or -)
- Converts to base-256 representation digit by digit
- Uses cascading carry algorithm to prevent overflow

#### 6. Decimal Output (`operator<<`)

Converts from internal base-256 representation back to decimal string:
- Repeatedly divides by 10, collecting remainders
- Builds decimal string from least to most significant digit
- Reverses string for correct output
- Handles zero and negative numbers correctly

### Exception Handling

The implementation properly handles exceptions:

- **`checkCapacity()`** throws `std::bad_alloc` when memory allocation fails
- Constructors propagate exceptions (object not constructed if exception occurs)
- `operator+` propagates exceptions (temporary result destroyed via RAII)
- Destructor safely handles `nullptr` (no-op for `delete[]`)

## Building the Project

### Prerequisites

- C++20 compatible compiler (g++, clang++)
- Python 3 (for running tests)
- Make utility

### Build Instructions

```bash
# Clean previous build
make clean

# Build the executable
make

# This creates the 'bignum' executable
```

The Makefile compiles with:
- C++20 standard (`-std=c++20`)
- Debug symbols (`-g`)

### Manual Compilation

If you prefer to compile manually:

```bash
c++ -std=c++20 -g -c bignum.cxx
c++ -std=c++20 -g -c bignummain.cxx
c++ -std=c++20 -g -c bignumops.cxx
c++ -std=c++20 -g -o bignum bignum.o bignummain.o bignumops.o
```

## Running the Program

### Interactive Mode

Run the executable directly:

```bash
./bignum
```

The program will prompt for numbers:
- Enter a decimal number (positive or negative)
- Enter 'q' or 'Q' to quit
- Output shows original input and BigNum representation

Example:
```
#Enter number of 'q': 12345678901234567890

orig=12345678901234567890
bn=12345678901234567890
print('yes' if orig==bn else 'no')
```

### Automated Testing

Use the Python test script to run automated tests:

```bash
# Test a single file
python3 test_bignum.py test_all.txt

# Test multiple files
python3 test_bignum.py test_all.txt test_file2.txt

# Test all test files matching a pattern
python3 test_bignum.py test*.txt
```

**Test Script Features:**
- Reads test data from external files
- Feeds data to executable automatically
- Validates output by comparing original input with BigNum output
- Outputs "pass" or "fail" for each test file
- Handles timeouts and errors gracefully
- Normalizes numbers (removes leading zeros) for comparison

**Test File Format:**
- One number per line
- Supports positive and negative numbers
- Lines starting with '#' are treated as comments and ignored
- Empty lines are ignored

Example test file:
```
0
1
-1
255
256
12345678901234567890
```

## Test Data

The `test_all.txt` file contains comprehensive test cases including:
- Basic numbers (0, 1, -1, small numbers)
- Boundary conditions (powers of 256, edge cases)
- Large numbers (hundreds of digits)

You can create additional test files following the same format.

## Design Decisions

### Why Base 256?

- Each digit fits in one byte (`unsigned char`)
- Efficient memory usage
- Natural for binary computers
- Easy to convert to/from binary representations

### Why Friend Function for `operator+`?

- Follows C++ best practices for binary operators
- Allows symmetric behavior: `a + b` and `b + a` work the same
- Doesn't require modifying the left operand
- Returns a new object (doesn't modify operands)

### Why Polynomial Representation?

- Makes addition straightforward (add coefficients of same order)
- Natural carry propagation
- Efficient for large numbers
- Mathematically elegant

## Code Quality

### Exception Safety

- All constructors are exception-safe
- RAII principles followed (destructor cleans up)
- No memory leaks on exceptions

### Memory Management

- Dynamic allocation only when needed
- Automatic cleanup via destructor
- Proper handling of null pointers
- Capacity grows incrementally as needed

### Code Style

- Clear, descriptive variable names
- Comprehensive comments explaining algorithms
- Consistent formatting
- No magic numbers (uses named constants)

## Limitations and Future Enhancements

### Current Limitations

- Only addition operator implemented
- No subtraction, multiplication, or division
- No comparison operators
- No input validation for invalid strings

### Potential Enhancements

- Additional arithmetic operators (-, *, /, %)
- Comparison operators (<, >, ==, !=)
- Bitwise operations
- Input validation and error messages
- Performance optimizations
- Support for different number bases

## Troubleshooting

### Compilation Errors

- Ensure C++20 support: `c++ --version` should show C++20 support
- Check all source files are present
- Verify Makefile is in the same directory

### Test Failures

- Ensure executable is built: `make`
- Check test file format (one number per line)
- Verify executable has execute permissions: `chmod +x bignum`

### Runtime Errors

- Memory allocation failures are caught and reported
- Invalid input may cause undefined behavior (no validation currently)
- Very large numbers may take significant time to process

## Author Notes

This implementation demonstrates:
- Proper C++ memory management (Rule of 4)
- Exception safety
- Efficient algorithms for big number arithmetic
- Clean, maintainable code structure
- Comprehensive testing framework

The code follows best practices for:
- Resource management
- Operator overloading
- Exception handling
- Code organization

### Alternative `store_t` Type Choices

The current implementation uses `unsigned char` (1 byte, base 256). Alternatives:

- **`unsigned short`** (2 bytes, base 65,536): Fewer digits/operations, but 2x memory per digit. Would need `buffer_t = unsigned int`.
- **`unsigned int`** (4 bytes, base 2³²): Very few digits, but 4x memory and needs `buffer_t = unsigned long long`.
- **`unsigned long long`** (8 bytes, base 2⁶⁴): Minimal digits, but 8x memory and needs 128-bit `buffer_t` (not standard).

**Key Constraint:** `buffer_t` must be at least 2x `store_t` to safely handle `store_t + store_t + carry` without overflow.

**Trade-off:** Smaller `store_t` = more memory efficient but more operations. Larger `store_t` = fewer operations but more memory per digit.

**Why `unsigned char`?** Good balance: memory efficient, simple overflow handling (unsigned short easily holds 2×256), works on all platforms.

