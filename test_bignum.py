#!/usr/bin/env python3
import subprocess
import sys
import os

def normalize(s):
    """Normalize number string: remove leading zeros, handle zero."""
    s = s.strip()
    if s in ('0', '-0'):
        return '0'
    if s.startswith('-'):
        return '-' + s[1:].lstrip('0') or '0'
    return s.lstrip('0') or '0'

def compute_expected_sum(a_str, b_str):
    """Compute the expected sum using Python's arbitrary precision integers."""
    try:
        a_int = int(a_str)
        b_int = int(b_str)
        result = a_int + b_int
        return str(result)
    except ValueError:
        return None

def test_file(test_file_path, executable="./bignum"):
    """Test a single test data file. Returns True if all tests pass.
    
    Test file format: each line contains two numbers to add, separated by whitespace.
    The test orchestrator computes the expected result using Python's arbitrary
    precision integers and verifies the C++ implementation is mathematically correct.
    """
    if not os.path.exists(test_file_path) or not os.path.exists(executable):
        return False
    
    # Read test cases from file
    test_cases = []
    with open(test_file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split()
            if len(parts) >= 1:
                if parts[0] == 'copy' and len(parts) >= 2:
                    # Format: copy <num>
                    test_cases.append({
                        'op': 'copy',
                        'num': parts[1]
                    })
                elif parts[0] == 'move' and len(parts) >= 2:
                    # Format: move <num>
                    test_cases.append({
                        'op': 'move',
                        'num': parts[1]
                    })
                elif parts[0] == 'long' and len(parts) >= 2:
                    # Format: long <num>
                    test_cases.append({
                        'op': 'long',
                        'num': parts[1]
                    })
                elif len(parts) >= 2:
                    # Format: <num1> <num2> - test addition (operator+)
                    num1 = parts[0]
                    num2 = parts[1]
                    # Compute expected result using Python's arbitrary precision
                    expected = compute_expected_sum(num1, num2)
                    test_cases.append({
                        'op': 'add',
                        'a': num1,
                        'b': num2,
                        'expected': expected
                    })
    
    if not test_cases:
        return True
    
    # Prepare input for executable
    input_lines = []
    for tc in test_cases:
        if tc['op'] == 'add':
            input_lines.append(f"{tc['a']} {tc['b']}")
        elif tc['op'] == 'copy':
            input_lines.append(f"copy {tc['num']}")
        elif tc['op'] == 'move':
            input_lines.append(f"move {tc['num']}")
        elif tc['op'] == 'long':
            input_lines.append(f"long {tc['num']}")
    
    try:
        result = subprocess.run([executable], input='\n'.join(input_lines) + '\nq\n',
                               text=True, capture_output=True, timeout=30)
        
        if result.returncode != 0:
            return False
        
        output_lines = result.stdout.split('\n')
        all_passed = True
        test_count = 0
        
        # Parse output
        i = 0
        test_idx = 0
        while i < len(output_lines) and test_idx < len(test_cases):
            tc = test_cases[test_idx]
            
            # Test addition (operator+): op=add, a=, b=, sum=
            if tc['op'] == 'add':
                if (i + 3 < len(output_lines) and 
                    output_lines[i].startswith('op=add') and
                    output_lines[i+1].startswith('a=') and 
                    output_lines[i+2].startswith('b=') and 
                    output_lines[i+3].startswith('sum=')):
                    
                    a_val = normalize(output_lines[i+1][2:])
                    b_val = normalize(output_lines[i+2][2:])
                    sum_val = normalize(output_lines[i+3][4:])
                    
                    a_expected = normalize(tc['a'])
                    b_expected = normalize(tc['b'])
                    
                    # Verify the inputs match
                    if a_val != a_expected or b_val != b_expected:
                        all_passed = False
                    
                    # Verify the sum is mathematically correct
                    if tc['expected']:
                        expected_sum = normalize(tc['expected'])
                        if sum_val != expected_sum:
                            all_passed = False
                            print(f"  FAIL: {a_expected} + {b_expected} = {sum_val} (expected {expected_sum})", file=sys.stderr)
                    
                    test_count += 1
                    test_idx += 1
                    i += 4
                else:
                    i += 1
            
            # Test copy constructor: op=copy, orig=, copied=, original_after_copy=
            elif tc['op'] == 'copy':
                if (i + 3 < len(output_lines) and 
                    output_lines[i].startswith('op=copy') and
                    output_lines[i+1].startswith('orig=') and 
                    output_lines[i+2].startswith('copied=') and 
                    output_lines[i+3].startswith('original_after_copy=')):
                    
                    orig_val = normalize(output_lines[i+1][5:])
                    copied_val = normalize(output_lines[i+2][7:])
                    original_after = normalize(output_lines[i+3][20:])
                    
                    expected = normalize(tc['num'])
                    
                    # Verify: copied should equal original, and original should remain unchanged
                    if copied_val != expected or orig_val != expected or original_after != expected:
                        all_passed = False
                        print(f"  FAIL: copy test - orig={orig_val}, copied={copied_val}, original_after={original_after} (expected {expected})", file=sys.stderr)
                    
                    test_count += 1
                    test_idx += 1
                    i += 4
                else:
                    i += 1
            
            # Test move assignment operator: op=move, orig=, moved=, original_after_move=
            elif tc['op'] == 'move':
                if (i + 3 < len(output_lines) and 
                    output_lines[i].startswith('op=move') and
                    output_lines[i+1].startswith('orig=') and 
                    output_lines[i+2].startswith('moved=') and 
                    output_lines[i+3].startswith('original_after_move=')):
                    
                    orig_val = normalize(output_lines[i+1][5:])
                    moved_val = normalize(output_lines[i+2][6:])
                    original_after = normalize(output_lines[i+3][20:])
                    
                    expected = normalize(tc['num'])
                    
                    # Verify: moved should have the original value
                    if moved_val != expected:
                        all_passed = False
                        print(f"  FAIL: move test - moved={moved_val} (expected {expected})", file=sys.stderr)
                    
                    # Verify: original after move should be 0 (moved from)
                    if original_after != '0':
                        all_passed = False
                        print(f"  FAIL: move test - original_after_move={original_after} (expected 0)", file=sys.stderr)
                    
                    test_count += 1
                    test_idx += 1
                    i += 4
                else:
                    i += 1
            
            # Test constructor with long: op=long, input=, long_value=, bignum=
            elif tc['op'] == 'long':
                if (i + 3 < len(output_lines) and 
                    output_lines[i].startswith('op=long') and
                    output_lines[i+1].startswith('input=') and 
                    output_lines[i+2].startswith('long_value=') and 
                    output_lines[i+3].startswith('bignum=')):
                    
                    input_val = normalize(output_lines[i+1][6:])
                    long_val_str = output_lines[i+2][11:].strip()
                    bignum_val = normalize(output_lines[i+3][7:])
                    
                    expected_input = normalize(tc['num'])
                    
                    # Verify input matches
                    if input_val != expected_input:
                        all_passed = False
                    
                    # Verify bignum constructed from long matches the input (as string)
                    # The long constructor should produce the same value as string constructor
                    try:
                        expected_long = int(expected_input)
                        expected_bignum = normalize(str(expected_long))
                        if bignum_val != expected_bignum:
                            all_passed = False
                            print(f"  FAIL: long constructor - bignum={bignum_val} (expected {expected_bignum})", file=sys.stderr)
                    except ValueError:
                        pass
                    
                    test_count += 1
                    test_idx += 1
                    i += 4
                else:
                    i += 1
            else:
                i += 1
        
        return all_passed and test_count > 0
        
    except (subprocess.TimeoutExpired, Exception) as e:
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 test_bignum.py <test_file1> [test_file2] ...")
        sys.exit(1)
    
    executable = "./bignum" if os.path.exists("./bignum") else "bignum"
    all_passed = True
    
    for test_file_path in sys.argv[1:]:
        result = test_file(test_file_path, executable)
        print(f"{test_file_path}: {'pass' if result else 'fail'}")
        if not result:
            all_passed = False
    
    sys.exit(0 if all_passed else 1)

if __name__ == "__main__":
    main()
