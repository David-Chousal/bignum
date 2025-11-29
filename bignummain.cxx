#include <string>
#include <iostream>
#include <exception>
#include <sstream>
#include "bignum.h"

using namespace std;
using namespace csen79;

/*
 * Main program that reads test commands from stdin and executes them.
 * Input format:
 *   - "<num1> <num2>" - test addition (operator+)
 *   - "copy <num>" - test copy constructor
 *   - "move <num>" - test move assignment operator
 *   - "long <num>" - test constructor with long argument
 * 
 * No hardcoded test data - all data comes from external files via stdin.
 */
int main(void) {
	std::string line;
	
	// Read lines from stdin (fed by test script from external files)
	while (getline(cin, line)) {
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#')
			continue;
		
		// Check for quit command
		if (line[0] == 'q' || line[0] == 'Q')
			break;
		
		stringstream ss(line);
		string first, second;
		
		// Read first token
		if (!(ss >> first))
			continue;
		
		try {
			// Check if first token is a command
			if (first == "copy" && ss >> second) {
				// Test copy constructor: "copy <num>"
				BigNum original(second);
				BigNum copied(original);  // Test copy constructor
				
				cout << "op=copy" << endl;
				cout << "orig=" << second << endl;
				cout << "copied=" << copied << endl;
				cout << "original_after_copy=" << original << endl;
			}
			else if (first == "move" && ss >> second) {
				// Test move assignment operator: "move <num>"
				BigNum original(second);
				BigNum moved;
				moved = std::move(original);  // Test move assignment operator
				
				cout << "op=move" << endl;
				cout << "orig=" << second << endl;
				cout << "moved=" << moved << endl;
				cout << "original_after_move=" << original << endl;
			}
			else if (first == "long" && ss >> second) {
				// Test constructor with long: "long <num>"
				long long_val = std::stol(second);
				BigNum from_long(long_val);  // Test constructor with long
				
				cout << "op=long" << endl;
				cout << "input=" << second << endl;
				cout << "long_value=" << long_val << endl;
				cout << "bignum=" << from_long << endl;
			}
			else if (ss >> second) {
				// Test addition: two numbers (first and second are both numbers)
				BigNum a(first);
				BigNum b(second);
				BigNum result = a + b;  // Test operator+
				
				cout << "op=add" << endl;
				cout << "a=" << first << endl;
				cout << "b=" << second << endl;
				cout << "sum=" << result << endl;
			}
		} catch (std::bad_alloc const &e) {
			cerr << "ERROR: failed to allocate memory" << endl;
		} catch (std::exception const &e) {
			cerr << "ERROR: " << e.what() << endl;
		}
	}

	return EXIT_SUCCESS;
}
