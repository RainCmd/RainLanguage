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

Real3 Real3::GetNormalized() const
{
	real sm = GetSqrMagnitude();
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real3(x / l, y / l, z / l);
	}
	return Real3(0, 0, 0);
}

real Real3::GetSqrMagnitude() const
{
	return Dot(*this, *this);
}

real Real3::GetMagnitude() const
{
	return MathReal::Sqrt(GetSqrMagnitude());
}

void Real3::SetMagnitude(const real& value)
{
	real sm = GetSqrMagnitude();
	if (sm > 0)
	{
		real s = value / MathReal::Sqrt(sm);
		x *= s;
		y *= s;
		z *= s;
	}
}

Real3 Real3::Lerp(const Real3& a, const Real3& b, const real& l)
{
	return a + (b - a) * l;
}

Real3 Real3::Max(const Real3& a, const Real3& b)
{
	return Real3(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y), MathReal::Max(a.z, b.z));
}

Real3 Real3::Min(const Real3& a, const Real3& b)
{
	return Real3(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y), MathReal::Min(a.z, b.z));
}

real Real3::Angle(const Real3& a, const Real3& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(a.GetSqrMagnitude() * b.GetSqrMagnitude());
	return MathReal::Acos(d);
}

Real3 Real3::Cross(const Real3& a, const Real3& b)
{
	return Real3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

real Real3::Dot(const Real3& a, const Real3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
