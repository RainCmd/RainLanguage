#include "../Public/Rain.h"

#ifdef FIXED_REAL
Real::Real() :value(0) {}

Real::Real(integer value) : value(value << DECIMAL) {}

Real Real::operator+() const { return CreateReal(value); }

Real Real::operator++() { return CreateReal(value += 1LL << DECIMAL); }

Real Real::operator++(int)
{
	Real result = CreateReal(value);
	value += 1LL << DECIMAL;
	return result;
}

Real Real::operator-() const { return CreateReal(-value); }

Real Real::operator--() { return CreateReal(value -= 1LL << DECIMAL); }

Real Real::operator--(int)
{
	Real result = CreateReal(value);
	value -= 1LL << DECIMAL;
	return result;
}

Real Real::operator+(const Real& other) const { return CreateReal(value + other.value); }

Real Real::operator+=(const Real& other) { return CreateReal(value += other.value); }

Real Real::operator-(const Real& other) const { return CreateReal(value - other.value); }

Real Real::operator-=(const Real& other) { return CreateReal(value -= other.value); }

Real Real::operator*(const Real& other) const { return CreateReal((value * other.value) >> DECIMAL); }

Real Real::operator*=(const Real& other) { return CreateReal(value = ((value * other.value) >> DECIMAL)); }

Real Real::operator/(const Real& other) const { return CreateReal((value / other.value) << DECIMAL); }

Real Real::operator/=(const Real& other) { return CreateReal(value = ((value / other.value) << DECIMAL)); }

Real Real::operator%(const Real& other) const { return CreateReal(value % other.value); }

Real Real::operator%=(const Real& other) { return CreateReal(value %= other.value); }

bool Real::operator==(const Real& other) const { return value == other.value; }

bool Real::operator!=(const Real& other) const { return value != other.value; }

bool Real::operator>(const Real& other) const { return value > other.value; }

bool Real::operator>=(const Real& other) const { return value >= other.value; }

bool Real::operator<(const Real& other) const { return value < other.value; }

bool Real::operator<=(const Real& other) const { return value <= other.value; }

Real::operator integer() { return value >> DECIMAL; }

Real Real::CreateReal(integer value) { return *(Real*)&value; }
#endif