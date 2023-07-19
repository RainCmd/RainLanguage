#pragma once
#include "../Language.h"

#ifdef FIXED_REAL
constexpr integer ONE = 1LL << DECIMAL;
constexpr integer MASK_DECIMAL = (1LL << DECIMAL) - 1LL;
#define PI real::CreateReal(205887)
#define HALF_PI real::CreateReal(205887>>1)
#define DOUBLE_PI real::CreateReal(205887<<1)
#define E real::CreateReal(178145)
class MathReal
{
public:
	static real inline Max(const real& a, const real& b) { return a.value > b.value ? a : b; }
	static real inline Min(const real& a, const real& b) { return a.value < b.value ? a : b; }
	static integer inline Sign(const real& value) { return value.value > 0 ? 1 : value.value < 0 ? -1 : 0; }
	static integer inline Round(const real& value)
	{
		const integer HALF = 1LL << (DECIMAL - 1);
		if (value.value > 0) return (value.value + HALF) >> DECIMAL;
		else return ((value.value - HALF - 1) >> DECIMAL) + 1;
	}
	static integer inline Ceil(const real& value)
	{
		if (value.value > 0)return ((value.value - 1) >> DECIMAL) + 1;
		else return (value.value + MASK_DECIMAL) >> DECIMAL;
	}
	static integer inline Floor(const real& value)
	{
		if (value.value > 0)return value.value >> DECIMAL;
		else return ((value.value - MASK_DECIMAL) >> DECIMAL) + 1;
	}
	static real inline Abs(const real& value) { return value.value < 0 ? -value : value; }
	static real inline Lerp(const real& a, const real& b, const real& l) { return a + (b - a) * l; }
	static real inline Clamp(const real& value, const real& min, const real& max)
	{
		if (value.value < min.value)return min;
		else if (value.value > max.value)return max;
		else return value;
	}
	static real inline Clamp01(const real& value)
	{
		if (value.value < 0)return real::CreateReal(0);
		else if (value.value > ONE)return real::CreateReal(ONE);
		else return value;
	}
	static real Sqrt(const real& value);
	static real inline Sin(const real& value)
	{
		int sign = 1;
		real angle = value;
		angle %= DOUBLE_PI;
		if (angle < 0)
		{
			angle = DOUBLE_PI + angle;
		}
		if ((angle > HALF_PI) && (angle <= PI))
		{
			angle = PI - angle;
		}
		else if ((angle > PI) && (angle <= (PI + HALF_PI)))
		{
			angle -= PI;
			sign = -1;
		}
		else if (angle > (PI + HALF_PI))
		{
			angle = DOUBLE_PI - angle;
			sign = -1;
		}

		real sqr = angle * angle;
		real result = real::CreateReal(498) * sqr;
		result -= real::CreateReal(10882);
		result *= sqr;
		result++;
		result *= angle;
		return result * sign;
	}
	static real inline Cos(const real& value) { return Sin(HALF_PI - value); }
	static real inline Tan(const real& value) { return Sin(value) / Cos(value); }
	static real Asin(const real& value);
	static real inline Acos(const real& value) { return HALF_PI - Asin(value); }
	static real inline Atan(const real& value)
	{
		if (value > 60) return HALF_PI;
		else if (value < -60) return -HALF_PI;
		return Asin(value / Sqrt(real::CreateReal(ONE) + value * value));
	}
	static real inline Atan2(const real& y, const real& x)
	{
		if (x == 0 && y == 0) return 0;
		else if (x > 0) return Atan(y / x);
		else if (x < 0)
			if (y >= 0) return PI + Atan(y / x);
			else return Atan(x / y) - PI;
		else return y >= 0 ? HALF_PI : -HALF_PI;
	}
};

#else
#include <math.h>
#define PI 3.14159265358979323846
#define E 2.71828182845904523536
class MathReal
{
public:
	static real inline Max(const real& a, const real& b) { return a > b ? a : b; }
	static real inline Min(const real& a, const real& b) { return a < b ? a : b; }
	static integer inline Sign(const real& value) { return value > 0 ? 1 : value < 0 ? -1 : 0; }
	static integer inline Round(const real& value) { return lround(value); }
	static integer inline Ceil(const real& value) { return (integer)ceil(value); }
	static integer inline Floor(const real& value) { return (integer)floor(value); }
	static real inline Abs(const real& value) { return value < 0 ? -value : value; }
	static real inline Lerp(const real& a, const real& b, const real& l) { return a + (b - a) * l; }
	static real inline Clamp(const real& value, const real& min, const real& max) { return value < min ? min : value > max ? max : value; }
	static real inline Clamp01(const real& value) { return value < 0 ? 0 : value > 1 ? 1 : value; }
	static real inline Sqrt(const real& value) { return sqrt(value); }
	static real inline Sin(const real& value) { return sin(value); }
	static real inline Cos(const real& value) { return cos(value); }
	static real inline Tan(const real& value) { return tan(value); }
	static real inline Asin(const real& value) { return asin(value); }
	static real inline Acos(const real& value) { return acos(value); }
	static real inline Atan(const real& value) { return atan(value); }
	static real inline Atan2(const real& y, const real& x) { return atan2(y, x); }
};
#endif