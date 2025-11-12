/*
 * Sin-Yaw Wang <swang24@scu.edu>
 * For CSEN79 Exercice
 */
#include <iostream>
#include <cstring>	// memset, memcpy
#include <limits>	// numeric_limits
#include <exception>	// bad_alloc
#include <algorithm>	// reverse
#include <string>
#include "bignum.h"

namespace csen79 {

// trivial and lazy implementations.  consider enhancing these.
BigNum::~BigNum() { delete [] digits; }
BigNum::BigNum(const BigNum &rhs) : digits(nullptr), high(0), sign(1), capacity(0) {
	this->operator=(rhs);
}

BigNum& BigNum::operator=(const BigNum &rhs) { 
	if (this != &rhs) {
		this->deepCopy(rhs);
	}
	return *this; 
}

BigNum& BigNum::operator=(BigNum &&rhs) {
	if (this != &rhs) {
		// Free current resources
		delete [] digits;
		
		// Move resources from rhs
		digits = rhs.digits;
		high = rhs.high;
		sign = rhs.sign;
		capacity = rhs.capacity;
		
		// RHS in empty state
		rhs.digits = nullptr;
		rhs.high = 0;
		rhs.sign = 1;
		rhs.capacity = 0;
	}
	return *this;
}

// Constructor that accepts a long argument
BigNum::BigNum(const long &n) : digits(nullptr), high(0), sign(1), capacity(0) {
	if (n == 0) {
		//Already initialized
		return;
	}

	// Handle sign
	long num = n;
	if (num < 0) {
		sign = -1;
		num = -num;  // Positive number
	}
	
	// Convert to base 256 representation
	// Allocate enough space
	int idx = 0;
	while (num > 0) {
		checkCapacity(idx);
		digits[idx] = static_cast<store_t>(num % StoreCap);
		num /= StoreCap;
		if (high <= idx + 1)
			high = idx + 1;
		++idx;
	}
}

// Addition operator (friend)
BigNum operator+(const BigNum &lhs, const BigNum &rhs) {
	BigNum result;
	
	if (lhs.sign == rhs.sign) {
		// Same signs: add coefficients of the same order
		// BigNum is stored as a polynomial: digits[0] + digits[1]*256 + digits[2]*256^2 + ...
		result.sign = lhs.sign;
		BigNum::buffer_t carry = 0;
		int maxHigh;
		if (lhs.high > rhs.high) {
			maxHigh = lhs.high;
		} else {
			maxHigh = rhs.high;
		}
		
		// Loop through all coefficient positions
		for (int i = 0; i < maxHigh || carry > 0; ++i) {
			result.checkCapacity(i);
			
			// Add coefficients of the same order
			BigNum::buffer_t sum = carry;  // Any carry from previous order
			if (i < lhs.high) {
				sum += lhs.digits[i];  // Coefficient from LHS
			}
			if (i < rhs.high) {
				sum += rhs.digits[i];  // Coefficient from RHS
			}
			
			// If sum exceeds the base (256), carry to the next higher order
			result.digits[i] = static_cast<BigNum::store_t>(sum % BigNum::StoreCap);
			carry = sum / BigNum::StoreCap;  // Carry the excess to next order
			if (result.high <= i + 1) result.high = i + 1;
		}
	} else {
		// Different signs: subtract smaller from larger
		bool lhsLarger = true;
		if (lhs.high < rhs.high) {
			lhsLarger = false;
		} else if (lhs.high == rhs.high) {
			// Compare from most significant to least
			for (int i = lhs.high - 1; i >= 0; --i) {
				if (lhs.digits[i] < rhs.digits[i]) {
					lhsLarger = false;
					break;
				}
				if (lhs.digits[i] > rhs.digits[i]) {
					lhsLarger = true;
					break;
				}
			}
		}
		
		// Subtract smaller from larger
		const BigNum::store_t *larger;
		const BigNum::store_t *smaller;
		int largerHigh;
		int smallerHigh;
		
		if (lhsLarger) {
			larger = lhs.digits;
			smaller = rhs.digits;
			largerHigh = lhs.high;
			smallerHigh = rhs.high;
			result.sign = lhs.sign;
		} else {
			larger = rhs.digits;
			smaller = lhs.digits;
			largerHigh = rhs.high;
			smallerHigh = lhs.high;
			result.sign = rhs.sign;
		}
		
		BigNum::buffer_t borrow = 0;
		for (int i = 0; i < largerHigh; ++i) {
			result.checkCapacity(i);
			BigNum::buffer_t largeDigit = larger[i];
			BigNum::buffer_t smallDigit;
			if (i < smallerHigh) {
				smallDigit = smaller[i];
			} else {
				smallDigit = 0;
			}
			
			BigNum::buffer_t diff;
			if (largeDigit < smallDigit + borrow) {
				diff = largeDigit + BigNum::StoreCap - smallDigit - borrow;
			} else {
				diff = largeDigit - smallDigit - borrow;
			}
			result.digits[i] = static_cast<BigNum::store_t>(diff);
			if (largeDigit < smallDigit + borrow) {
				borrow = 1;
			} else {
				borrow = 0;
			}
			if (result.high <= i + 1) result.high = i + 1;
		}
		
		// Remove leading zeros
		while (result.high > 0 && result.digits[result.high - 1] == 0) {
			--result.high;
		}
	}
	
	if (result.high == 0) {
		result.sign = 1;  // Zero
	}
	
	return result;
}

// Deep copy
void BigNum::deepCopy(const BigNum &rhs) {
	// Free existing resources
	delete [] digits;
	
	// Copy variables
	high = rhs.high;
	sign = rhs.sign;
	capacity = rhs.capacity;
	
	// Allocate and copy digits array
	if (rhs.digits != nullptr && capacity > 0) {
		digits = new (std::nothrow) store_t[capacity];
		if (digits == nullptr) {
			throw std::bad_alloc();
		}
		memcpy(digits, rhs.digits, sizeof(store_t) * capacity);
	} else {
		digits = nullptr;
	}
}

// Formatted output
std::ostream& operator<<(std::ostream &os, const BigNum &n) {
	if (n.high == 0) {
		os << "0";
		return os;
	}
	
	if (n.sign < 0) {
		os << "-";
	}
	
	// Convert to decimal
	// Temp array to store decimal digits
	// Start with the most significant base-256 digit
	std::string decimal;
	
	// Process each digit
	// Build the decimal representation by repeatedly dividing by 10
	
	// Working copy
	BigNum temp = n;
	temp.sign = 1;  // Absolute value
	
	// Convert to decimal
	while (temp.high > 0) {
		// Divide by 10 and get remainder
		BigNum::buffer_t remainder = 0;
		for (int i = temp.high - 1; i >= 0; --i) {
			BigNum::buffer_t dividend = remainder * BigNum::StoreCap + temp.digits[i];
			temp.digits[i] = static_cast<BigNum::store_t>(dividend / 10);
			remainder = dividend % 10;
		}
		
		// Add remainder as next decimal digit
		decimal += static_cast<char>('0' + remainder);
		
		// Remove leading zeros
		while (temp.high > 0 && temp.digits[temp.high - 1] == 0) {
			--temp.high;
		}
	}
	
	// Reverse the decimal string
	std::reverse(decimal.begin(), decimal.end());
	os << decimal;
	
	return os;
}


}