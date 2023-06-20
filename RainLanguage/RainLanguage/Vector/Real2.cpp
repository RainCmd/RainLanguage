#include "../Public/Vector.h"
#include "../Real/MathReal.h"

Real2::Real2(const real& x, const real& y) :x(x), y(y) {}

Real2 Real2::operator-() const
{
	return Real2(-x, -y);
}

Real2 Real2::operator+(const Real2& other) const
{
	return Real2(x + other.x, y + other.y);
}

Real2 Real2::operator-(const Real2& other) const
{
	return Real2(x - other.x, y - other.y);
}

Real2 Real2::operator*(const Real2& other) const
{
	return Real2(x * other.x, y * other.y);
}

Real2 Real2::operator*(const real& other) const
{
	return Real2(x * other, y * other);
}

Real2 operator*(const real& value, const Real2& vector)
{
	return Real2(value * vector.x, value * vector.y);
}

Real2 Real2::operator/(const Real2& other) const
{
	return Real2(x / other.x, y / other.y);
}

Real2 Real2::operator/(const real& other) const
{
	return Real2(x / other, y / other);
}

Real2 operator/(const real& value, const Real2& vector)
{
	return Real2(value / vector.x, value / vector.y);
}

bool Real2::operator==(const Real2& other) const
{
	return x == other.x && y == other.y;
}

bool Real2::operator!=(const Real2& other) const
{
	return x != other.x || y != other.y;
}
