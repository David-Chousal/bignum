#include <string>
#include <iostream>
#include <exception>
#include "bignum.h"

using namespace std;
using namespace csen79;

/*
 * Write your own "main" as tester.  Exercise all code you wrote.  Make sure to cover boundary condistions.
 * Come up with a way to fake "new" failures.  (call fault injection)
 */
int main(void) {
	std::string str;
	while (true) {
		cout << "#Enter number of 'q': ";
		cin >> str;
		cout << endl;
		if (str[0] == 'q' || str[0] == 'Q')
			break;
		try {
			BigNum bn(str);	// constructed with string
			// this output is meant for Python to execute
			cout << "orig=" << str << endl;
			cout << "bn=" << bn << endl;
			cout << "print('yes' if orig==bn else 'no')" << endl;
			cout << endl;
		} catch (std::bad_alloc const &e) {
			cerr << "failed to allocate memory for " << str << endl;
		}
	}

	return EXIT_SUCCESS;
}
