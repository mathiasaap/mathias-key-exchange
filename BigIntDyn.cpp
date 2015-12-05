#include "BigIntDyn.h"

BigInt::BigInt()
	{
	ALLOCATED_WORDS = 1;
	WORDS_WRITTEN = 1;
	DATA = new uint32_t[ALLOCATED_WORDS + 1]();
	}


BigInt::BigInt(uint32_t* DATA, unsigned int ALLOCATED_WORDS, unsigned int WORDS_WRITTEN)
{
	this->DATA = new uint32_t[ALLOCATED_WORDS + 1]();
	memcpy_s(this->DATA, sizeof(uint32_t)*ALLOCATED_WORDS, DATA, sizeof(uint32_t)*ALLOCATED_WORDS);
	this->ALLOCATED_WORDS = ALLOCATED_WORDS;
	this->WORDS_WRITTEN = WORDS_WRITTEN;
}

BigInt::BigInt(BigInt& other)
{
	this->ALLOCATED_WORDS = other.ALLOCATED_WORDS;
	this->WORDS_WRITTEN = other.WORDS_WRITTEN;
	DATA = new uint32_t[ALLOCATED_WORDS + 1]();
	memcpy_s(this->DATA, sizeof(uint32_t)*ALLOCATED_WORDS, other.DATA, sizeof(uint32_t)*ALLOCATED_WORDS);

}


void BigInt::ReallocateData()
{
	uint32_t* NEW_DATA_BUF = new uint32_t[ALLOCATED_WORDS * 2 + 1]();
	memcpy_s(NEW_DATA_BUF, sizeof(uint32_t)*(ALLOCATED_WORDS * 2 + 1), DATA, sizeof(uint32_t)*(ALLOCATED_WORDS + 1));
	ALLOCATED_WORDS *= 2;

	delete[] DATA;
	DATA = NEW_DATA_BUF;


}

BigInt BigInt::operator*(const BigInt& other)
{
	BigInt result;
	for (int i = 0; i < other.words_written(); i++)
	{
		uint32_t rest = 0;
		for (int j = 0; j < WORDS_WRITTEN; j++)
		{
			uint64_t product = DATA[j];
			product *= other.DATA[i];
			result.add_rest(product&UINT32_MAX, i + j);
			result.add_rest(rest, i + j);
			rest = ((product & 0xFFFFFFFF00000000) >> 0x20);
		
		}
		if (rest != 0)
		{
			result.add_rest(rest, i + WORDS_WRITTEN);
		}
	}
	return BigInt(result);
}

BigInt BigInt::operator*(const uint32_t& other)
{
	BigInt result;
	/*	uint32_t rest = 0;
		for (int j = 0; j < WORDS_WRITTEN; j++)
		{
			uint64_t product = DATA[j];
			product *= other;
			result.add_rest(product&UINT32_MAX, i + j);
			result.add_rest(rest, i + j);
			rest = ((product & 0xFFFFFFFF00000000) >> 0x20);

		}
		if (rest != 0)
		{
			result.add_rest(rest, i + WORDS_WRITTEN);
		}*/
	return BigInt(result);
}

BigInt BigInt::operator/(const uint32_t& other)
{
	
	uint32_t *new_data = new uint32_t[ALLOCATED_WORDS+1]();

	uint64_t rest = 0;
	for (int i = WORDS_WRITTEN-1; i >= 0; i--)
	{
		new_data[i]=((rest+DATA[i]) / other);
		rest = ((rest + DATA[i]) % other)<32;

	}
	uint32_t new_words_written = WORDS_WRITTEN;
	while (!new_data[new_words_written])
	{
		new_words_written--;
	}
	
	return BigInt(new_data,ALLOCATED_WORDS,new_words_written);
}

BigInt BigInt::operator%(const BigInt& other)
{
	BigInt newnum(*this);
	BigInt other_temp(other.DATA, other.ALLOCATED_WORDS, other.WORDS_WRITTEN);
	while (newnum >= other_temp)
	{
		other_temp << 1;
	}

	while (newnum >= other)
	{
		while (!(newnum >= other_temp))
		{
			other_temp >> 1;
		}
		newnum -= other_temp;
	}
	return BigInt(newnum);
}

uint32_t BigInt::operator%(const uint32_t& other)
{
	return this->DATA[0] - other*operator/(other).DATA[0];
}

BigInt& BigInt::operator++()
{
	for (unsigned i = 0; i < ALLOCATED_WORDS+1; i++)
	{
		if (i == ALLOCATED_WORDS)ReallocateData();

		if (DATA[i] == UINT32_MAX)
		{
			DATA[i] = 0;
		}
		else
		{
			DATA[i]++;
			if (i > WORDS_WRITTEN) WORDS_WRITTEN = i;
			return *this;
		}
	}
	return *this;
}
BigInt& BigInt::operator--()
{
	for (unsigned i = 0; i < ALLOCATED_WORDS; i++)
	{
		if (DATA[i] == 0)
		{
			DATA[i] = UINT32_MAX;
		}
		else
		{
			DATA[i]--;
			break;
		}
	}
	return *this;
}
BigInt& BigInt::operator+=(const BigInt& other)
{
	unsigned int rest = 0;
	for (int i = 0; i < other.words_written(); i++)
	{
		if (i == ALLOCATED_WORDS)ReallocateData();

		uint64_t sum = DATA[i];
		sum += other.DATA[i];
		sum += rest;

		if (sum > UINT32_MAX)
		{
			DATA[i] = sum&UINT32_MAX;
			rest = ((sum & 0xFFFFFFFF00000000) >> 0x20);
		}
		else
		{
			DATA[i] = sum;
			rest = 0;
		}

	}
	if (other.words_written() > WORDS_WRITTEN)
	{
		WORDS_WRITTEN = other.words_written();
	}
	if (rest > 0)
	{
		return add_rest(rest, other.words_written());
	}


	return *this;
}

BigInt& BigInt::operator-=(const BigInt& other)
{
	check_words_written();
	if (!operator>=(other))
	{ 
		*this = 0;
		WORDS_WRITTEN = 1;
		return *this;
	}
	for (int i = 0; i < WORDS_WRITTEN; i++)
	{
		uint64_t sum = DATA[i];
		if (sum<other.DATA[i])
		{
			DATA[i + 1]--;
			sum += UINT32_MAX + 1;
		}
		DATA[i] = sum - other.DATA[i];

	}
	
	check_words_written();
	return *this;
}
BigInt& BigInt::operator+=(const unsigned int& other)
{
	return add_rest(other, 0);
}

BigInt& BigInt::add_rest(const unsigned int& other, unsigned start)
{
	unsigned int rest = other;
	for (int i = start; i < ALLOCATED_WORDS + 1; i++)
	{
		if (i == ALLOCATED_WORDS)ReallocateData();
		if (i+1 > WORDS_WRITTEN)
		{
			WORDS_WRITTEN = i+1;
		}

		uint64_t sum = DATA[i];
		sum += rest;
		if (sum > UINT32_MAX)
		{
			DATA[i] = sum&UINT32_MAX;
			rest = ((sum & 0xFFFFFFFF00000000) >> 0x20);
		}
		else
		{
			DATA[i] = sum;
			return *this;
		}


	}
	return *this;

}

bool BigInt::operator>=(const BigInt& other)
{
	if (WORDS_WRITTEN > other.WORDS_WRITTEN)
	{
		return true;
	}
	else if (WORDS_WRITTEN < other.WORDS_WRITTEN)
	{
		return false;
	}

	for (int i = WORDS_WRITTEN - 1; i >= 0; i--)
	{
		if (this->DATA[i] == other.DATA[i] && i!=0)
		{
			continue;
		}
		else if (this->DATA[i] >= other.DATA[i])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

bool BigInt::operator=(const uint32_t& other)
{
	memset(DATA, 0, sizeof(uint32_t)*ALLOCATED_WORDS);
	DATA[0] = other;
	WORDS_WRITTEN = 1;
	return true;
}
bool BigInt::operator=(const BigInt& other)
{
	memset(DATA, 0, sizeof(uint32_t)*ALLOCATED_WORDS);

	for (int i = 0; i < other.allocated_words(); i++)
	{
		if (i == ALLOCATED_WORDS)ReallocateData();
		DATA[i] = other.DATA[i];

	}
	WORDS_WRITTEN = other.words_written();

	return true;
}
bool BigInt::operator==(const BigInt& other)
{
	if (WORDS_WRITTEN != other.words_written())
	{
		return false;
	}

	for (unsigned i = 0; i < WORDS_WRITTEN; i++)
	{
		if (this->DATA[i] != other.DATA[i])
		{
			return false;
		}
	}

	return true;
}
bool BigInt::operator==(const unsigned int& other)
{

	if (DATA[0] != other)
		return false;

	for (int i = 0; i < WORDS_WRITTEN; i++)
	{
		if (DATA[i] != 0)
		{
			return false;
		}
	}
	return true;
}
void BigInt::word_shiftr()
{
	for (int word = 1; word < WORDS_WRITTEN; word++)
	{
		memcpy_s(&DATA[word - 1], sizeof(uint32_t), &DATA[word], sizeof(uint32_t));
	}
	WORDS_WRITTEN--;
	clear_high_mem();
}

void BigInt::word_shiftl()
{
	if (WORDS_WRITTEN == ALLOCATED_WORDS)
	{
		ReallocateData();
	}
	for (int word = WORDS_WRITTEN; word > 0; word--)
	{
		memcpy_s(&DATA[word], sizeof(uint32_t), &DATA[word-1], sizeof(uint32_t));
	}
	DATA[0] = 0;
	WORDS_WRITTEN++;
}

BigInt& BigInt::operator >> (const unsigned int& other)
{
	unsigned int remaining = other;
	while (remaining >= 32)
	{
		word_shiftr();
		remaining -= 32;
	}
	if (remaining > 0){
		uint32_t mask = pow(2, remaining) - 1;
		uint32_t rest = 0;

		for (int i = WORDS_WRITTEN - 1; i >= 0; i--)
		{
			uint32_t nextrest = DATA[i] & mask;
			DATA[i] = (DATA[i] & (UINT32_MAX - mask)) >> remaining;
			DATA[i] += rest << (32 - remaining);
			rest = nextrest;

		}
	}
	check_words_written();
	return *this;
}

BigInt& BigInt::operator << (const unsigned int& other)
{
	unsigned int remaining = other;
	while (remaining >= 32)
	{
		word_shiftl();
		remaining -= 32;
	}
	if (remaining > 0){
		uint32_t mask = pow(2, 32-remaining) - 1;
		uint32_t rest = 0;

		for (int i = 0; i <WORDS_WRITTEN ||rest!=0; i++)
		{
			if (i == ALLOCATED_WORDS)
			{
				ReallocateData();
				WORDS_WRITTEN += 1;
			}
			uint32_t nextrest = DATA[i] & (UINT32_MAX-mask);
			DATA[i] = (DATA[i] & mask) << remaining;
			DATA[i] += (rest >> (32-remaining));
			rest = nextrest;

		}
	}
	return *this;
}

std::string BigInt::toString() const
{
	std::stringstream ss;

	for (int i = WORDS_WRITTEN - 1; i >= 0; i--)
	{
		uint32_t nextNum = ((DATA[i] & 0xFF000000) >> 0x18);

		if (nextNum < 0x10)
			ss << std::hex << 0;
		ss << std::hex << nextNum;

		nextNum = ((DATA[i] & 0xFF0000) >> 0x10);
		if (nextNum < 0x10)
			ss << std::hex << 0;
		ss << std::hex << nextNum;

		nextNum = ((DATA[i] & 0xFF00) >> 0x8);
		if (nextNum < 0x10)
			ss << std::hex << 0;
		ss << std::hex << nextNum;

		nextNum = (DATA[i] & 0xFF);
		if (nextNum < 0x10)
			ss << std::hex << 0;
		ss << std::hex << nextNum;


	}

	return ss.str();

}
void BigInt::clear_high_mem()
{
	memset(&DATA[WORDS_WRITTEN], 0, ALLOCATED_WORDS + 1 - WORDS_WRITTEN);
}

int BigInt::shift_count()
{
	for (int CUR_WORD = 0; CUR_WORD < WORDS_WRITTEN; CUR_WORD++)
	{
		uint32_t mask = 1;
		for (int i = 0; i < 32; i++)
		{
			if ((DATA[CUR_WORD] & mask) >> i != 0)
			{
				return CUR_WORD * 32 + i;
			}
			mask *= 2;
		}
	
	}
	return 0;
}

void BigInt::check_words_written()
{
	while (DATA[WORDS_WRITTEN - 1] == 0)
	{
		WORDS_WRITTEN--;
	}
}