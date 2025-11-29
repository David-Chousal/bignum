# BigNum Implementation

## How to Run

### Building the Program
```bash
make clean
make
```
This creates the `bignum` executable.

### Running the Main Program
#### Option 1: Interactive Mode
```bash
./bignum
```
Then type pairs of numbers (one per line) and press `q` to quit:
```
123 456
-100 100
q
```

#### Option 2: Run with Test File
```bash
./bignum < test_happy.txt
```

### Running the Test Orchestrator
The test orchestrator feeds test data to the program and verifies mathematical correctness:

```bash
python3 test_bignum.py test_happy.txt test_boundary.txt
```

Test all operations:
```bash
python3 test_bignum.py test_happy.txt test_boundary.txt test_copy.txt test_move.txt test_long.txt
```

### Input Format
- **Addition**: `123 456` (two numbers separated by space)
- **Copy constructor**: `copy 123`
- **Move operator**: `move 123`
- **Long constructor**: `long 12345`
- **Quit**: `q` or `Q`

---

## Requirements Met

### 1. Main reads from cin, performs additions, outputs to cout (no hardcoded data)
- **Reads from cin**: `bignummain.cxx` line 24
- **Performs additions**: `bignummain.cxx` line 77
- **Outputs to cout**: `bignummain.cxx` lines 79-82
- **No hardcoded test data**: `bignummain.cxx` line 18

### 2. Test orchestrator feeds data and verifies mathematical correctness
- **Feeds data**: `test_bignum.py` line 62
- **Computes expected results**: `test_bignum.py` lines 15-23
- **Verifies correctness**: `test_bignum.py` lines 95-101

### 3. At least 2 test files: happy path and boundary cases
- **Happy path**: `test_happy.txt` - Contains large number additions with significant digits
- **Boundary cases**: `test_boundary.txt` - Contains edge cases (zero, negatives, mixed signs, very large numbers, etc.)
Both files exist and are used by the test orchestrator.

### 4. operator+, copy constructor, and move operator are all tested
#### operator+ (Addition)
- **Implementation**: `bignumops.cxx` line 76
- **Tested in main**: `bignummain.cxx` line 77
- **Test files**: `test_happy.txt` and `test_boundary.txt` contain addition test cases
#### Copy Constructor
- **Implementation**: `bignumops.cxx` line 17
- **Tested in main**: `bignummain.cxx` line 45
- **Test file**: `test_copy.txt` contains copy constructor test cases
#### Move Assignment Operator
- **Implementation**: `bignumops.cxx` line 28
- **Tested in main**: `bignummain.cxx` line 56
- **Test file**: `test_move.txt` contains move operator test cases

---

## Test Files

- `test_happy.txt` - Happy path tests with large numbers (10 test cases)
- `test_boundary.txt` - Boundary case tests (30+ test cases with zero, negatives, mixed signs, etc.)
- `test_copy.txt` - Copy constructor tests
- `test_move.txt` - Move assignment operator tests
- `test_long.txt` - Constructor with long argument tests

---

## Code Coverage (gcov)
Code coverage was measured using `gcov`. Results are documented in:
- `gcov_raw_output.txt` - Unfiltered gcov output

