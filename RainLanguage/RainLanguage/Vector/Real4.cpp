#include "../Public/Vector.h"
#include "../Real/MathReal.h"

Real4::Real4(const real& x, const real& y, const real& z, const real& w) : x(x), y(y), z(z), w(w) {}

Real4 Real4::operator-() const
{
	return Real4(-x, -y, -z, -w);
}

Real4 Real4::operator+(const Real4& other) const
{
	return Real4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Real4 Real4::operator-(const Real4& other) const
{
	return Real4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Real4 Real4::operator*(const Real4& other) const
{
	return Real4(x * other.x, y * other.y, z * other.z, w * other.w);
}

Real4 Real4::operator*(const real& other) const
{
	return Real4(x * other, y * other, z * other, w * other);
}

Real4 operator*(const real& value, const Real4& vector)
{
	return Real4(value * vector.x, value * vector.y, value * vector.z, value * vector.z);
}

Real4 Real4::operator/(const Real4& other) const
{
	return Real4(x / other.x, y / other.y, z / other.z, w / other.w);
}

Real4 Real4::operator/(const real& other) const
{
	return Real4(x / other, y / other, z / other, w / other);
}

Real4 operator/(const real& value, const Real4& vector)
{
	return Real4(value / vector.x, value / vector.y, value / vector.z, value / vector.w);
}

bool Real4::operator==(const Real4& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Real4::operator!=(const Real4& other) const
{
	return x != other.x || y != other.y || z != other.z || w != other.w;
}
