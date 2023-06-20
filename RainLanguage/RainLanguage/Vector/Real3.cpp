#include "../Public/Vector.h"
#include "../Real/MathReal.h"

Real3::Real3(const real& x, const real& y, const real& z) : x(x), y(y), z(z) {}

Real3 Real3::operator-() const
{
	return Real3(-x, -y, -z);
}

Real3 Real3::operator+(const Real3& other) const
{
	return Real3(x + other.x, y + other.y, z + other.z);
}

Real3 Real3::operator-(const Real3& other) const
{
	return Real3(x - other.x, y - other.y, z - other.z);
}

Real3 Real3::operator*(const Real3& other) const
{
	return Real3(x * other.x, y * other.y, z * other.z);
}

Real3 Real3::operator*(const real& other) const
{
	return Real3(x * other, y * other, z * other);
}

Real3 operator*(const real& value, const Real3& vector)
{
	return Real3(value * vector.x, value * vector.y, value * vector.z);
}

Real3 Real3::operator/(const Real3& other) const
{
	return Real3(x / other.x, y / other.y, z / other.z);
}

Real3 Real3::operator/(const real& other) const
{
	return Real3(x / other, y / other, z / other);
}

Real3 operator/(const real& value, const Real3& vector)
{
	return Real3(value / vector.x, value / vector.y, value / vector.z);
}

bool Real3::operator==(const Real3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

bool Real3::operator!=(const Real3& other) const
{
	return x != other.x || y != other.y || z != other.z;
}
