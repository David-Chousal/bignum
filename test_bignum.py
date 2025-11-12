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

def test_file(test_file_path, executable="./bignum"):
    """Test a single test data file. Returns True if all tests pass."""
    if not os.path.exists(test_file_path) or not os.path.exists(executable):
        return False
    
    with open(test_file_path, 'r') as f:
        test_lines = [line.strip() for line in f if line.strip() and not line.strip().startswith('#')]
    
    if not test_lines:
        return True
    
    try:
        result = subprocess.run([executable], input='\n'.join(test_lines) + '\nq\n',
                               text=True, capture_output=True, timeout=30)
        
        output_lines = result.stdout.split('\n')
        all_passed = True
        test_count = 0
        
        for i in range(len(output_lines) - 1):
            if output_lines[i].startswith('orig=') and output_lines[i+1].startswith('bn='):
                orig = normalize(output_lines[i][5:])
                bn = normalize(output_lines[i+1][3:])
                if orig != bn:
                    all_passed = False
                test_count += 1
        
        return all_passed and test_count > 0
        
    except (subprocess.TimeoutExpired, Exception):
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
