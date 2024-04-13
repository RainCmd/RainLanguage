#include "MathReal.h"

#ifdef FIXED_REAL

int TAB_64[] = {
		63,  0, 58,  1, 59, 47, 53,  2,
		60, 39, 48, 27, 54, 33, 42,  3,
		61, 51, 37, 40, 49, 18, 28, 20,
		55, 30, 34, 11, 43, 14, 22,  4,
		62, 57, 46, 52, 38, 26, 32, 41,
		50, 36, 17, 19, 29, 10, 13, 21,
		56, 45, 25, 31, 35, 16,  9, 12,
		44, 24, 15,  8, 23,  7,  6,  5 };
int Log2(integer value)
{
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value |= value >> 32;
	return TAB_64[((uint64)(value - (value >> 1)) * 0x07EDD5E59A4E28C2) >> 58];
}
real MathReal::Sqrt(const real& value)
{
	if (value.value > 0)
	{
		int i = Log2(value.value);
		i -= DECIMAL_POINT;
		bool b = i < 0;
		if (b) i = -i;
		i >>= 1;
		if (b) i = -i;
		i += DECIMAL_POINT;
		integer tmp = 1LL << i;
		while (i > 0)
		{
			integer sqr = (tmp * tmp) >> DECIMAL_POINT;
			if (sqr == value.value) return real::CreateReal(tmp);
			else
			{
				i--;
				if (sqr > value.value)
				{
					tmp -= 1LL << i;
				}
				else
				{
					tmp += 1LL << i;
				}
			}
		}
		return real::CreateReal(tmp);
	}
	else return value;
}

real MathReal::Asin(const real& value)
{
	bool isMoreThan45 = false;
	bool isNegForSin = false;
	real result = value;
	if (result.value < 0)
	{
		isNegForSin = true;
		result = -result;
	}
	if (result > real::CreateReal(46333) && result.value != 0)
	{
		isMoreThan45 = true;
		result = Sqrt(real::CreateReal(ONE) - result * result);
	}
	real x = 0;
	real res = result;
	real coe = result;
	for (int i = 1; i <= 3; i++)
	{
		x += 2;
		coe = coe * (x - 1) * (x - 1) / x / (x + 1) * result * result;
		res += coe;
	}
	if (isMoreThan45)
		res = HALF_PI - res;
	if (isNegForSin)
		res = -res;
	return res;
}

#endif