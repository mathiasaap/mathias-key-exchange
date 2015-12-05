#include "BigIntDyn.h"
#include <cstdlib>
#include <fstream>
#include <cmath>

std::ostream& operator<<(std::ostream& os, const BigInt& obj)
{
	os << obj.toString();
	return os;
}

int main()
{
	BigInt number,number2,number3,number4;



	number = 2561895;
	number2 = 2464815;
	number3 = number*number2;

	number4 = 77784;
	number2 = number3%number4;

	
	std::cout << "Modulus is so slow: " << number2 << std::endl;

	return 0;
}