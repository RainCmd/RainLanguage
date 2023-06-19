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

Real2 Real2::GetNormalized() const
{
	real sm = GetSqrMagnitude();
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real2(x / l, y / l);
	}
	return Real2(0, 0);
}

real Real2::GetSqrMagnitude() const
{
	return Dot(*this, *this);
}

real Real2::GetMagnitude() const
{
	return MathReal::Sqrt(GetSqrMagnitude());
}

void Real2::SetMagnitude(const real& value)
{
	real sm = GetSqrMagnitude();
	if (sm > 0)
	{
		real s = value / MathReal::Sqrt(sm);
		x *= s;
		y *= s;
	}
}

Real2 Real2::Lerp(const Real2& a, const Real2& b, const real& l)
{
	return a + (b - a) * l;
}

Real2 Real2::Max(const Real2& a, const Real2& b)
{
	return Real2(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y));
}

Real2 Real2::Min(const Real2& a, const Real2& b)
{
	return Real2(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y));
}

real Real2::Angle(const Real2& a, const Real2& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(a.GetSqrMagnitude() * b.GetSqrMagnitude());
	return MathReal::Acos(d);
}

real Real2::Cross(const Real2& a, const Real2& b)
{
	return a.x * b.y - b.x * a.y;
}

real Real2::Dot(const Real2& a, const Real2& b)
{
	return a.x * b.x + a.y * b.y;
}
