#pragma once
#include <cstdint>
#include<cmath>

#include <iostream>
#include <sstream>
#include <iomanip>

class BigInt
{
private: 
	uint32_t* DATA;
	unsigned int ALLOCATED_WORDS;
	unsigned int WORDS_WRITTEN;

	void ReallocateData();
	void clear_high_mem();
	void check_words_written();
	void word_shiftr();
	void word_shiftl();

public:
	BigInt();
	BigInt(BigInt& other);
	BigInt(uint32_t* DATA, unsigned int ALLOCATED_WORDS, unsigned int WORDS_WRITTEN);

	BigInt& operator++();
	BigInt& operator--();
	BigInt& operator+=(const BigInt& other);
	BigInt& operator-=(const BigInt& other);
	BigInt& operator+=(const unsigned int& other);
	BigInt& add_rest(const unsigned int& other, unsigned start);
	BigInt& operator>>(const unsigned int& other);
	BigInt& operator<<(const unsigned int& other);
	BigInt operator*(const BigInt& other);
	BigInt operator%(const BigInt& other);
	BigInt operator*(const uint32_t& other);
	BigInt operator/(const uint32_t& other);
	uint32_t operator%(const uint32_t& other);

	bool BigInt::operator=(const uint32_t& other);
	bool BigInt::operator=(const BigInt& other);
	bool operator==(const BigInt& other);
	bool operator==(const unsigned int& other);
	bool operator>=(const BigInt& other);
	
	std::string toString() const;
	int shift_count();



	inline unsigned allocated_words()const{ return ALLOCATED_WORDS; }
	inline unsigned words_written()const{ return WORDS_WRITTEN; }

	inline ~BigInt(){ delete[] DATA; }
};

