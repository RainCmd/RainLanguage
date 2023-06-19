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

Real4 Real4::GetNormalized() const
{
	real sm = GetSqrMagnitude();
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real4(x / l, y / l, z / l, w / l);
	}
	return Real4(0, 0, 0, 0);
}

real Real4::GetSqrMagnitude() const
{
	return Dot(*this, *this);
}

real Real4::GetMagnitude() const
{
	return MathReal::Sqrt(GetSqrMagnitude());
}

void Real4::SetMagnitude(const real& value)
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

Real4 Real4::Lerp(const Real4& a, const Real4& b, const real& l)
{
	return a + (b - a) * l;
}

Real4 Real4::Max(const Real4& a, const Real4& b)
{
	return Real4(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y), MathReal::Max(a.z, b.z), MathReal::Max(a.w, b.w));
}

Real4 Real4::Min(const Real4& a, const Real4& b)
{
	return Real4(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y), MathReal::Min(a.z, b.z), MathReal::Min(a.w, b.w));
}

real Real4::Angle(const Real4& a, const Real4& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(a.GetSqrMagnitude() * b.GetSqrMagnitude());
	return MathReal::Acos(d);
}

real Real4::Dot(const Real4& a, const Real4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
