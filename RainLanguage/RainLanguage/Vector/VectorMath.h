#pragma once
#include "../Public/Vector.h"
#include "../Real/MathReal.h"

inline Real2 Lerp(const Real2& a, const Real2& b, const real& l)
{
	return a + (b - a) * l;
}
inline Real3 Lerp(const Real3& a, const Real3& b, const real& l)
{
	return a + (b - a) * l;
}
inline Real4 Lerp(const Real4& a, const Real4& b, const real& l)
{
	return a + (b - a) * l;
}
inline Real2 Max(const Real2& a, const Real2& b)
{
	return Real2(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y));
}
inline Real3 Max(const Real3& a, const Real3& b)
{
	return Real3(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y), MathReal::Max(a.z, b.z));
}
inline Real4 Max(const Real4& a, const Real4& b)
{
	return Real4(MathReal::Max(a.x, b.x), MathReal::Max(a.y, b.y), MathReal::Max(a.z, b.z), MathReal::Max(a.w, b.w));
}
inline Real2 Min(const Real2& a, const Real2& b)
{
	return Real2(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y));
}
inline Real3 Min(const Real3& a, const Real3& b)
{
	return Real3(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y), MathReal::Min(a.z, b.z));
}
inline Real4 Min(const Real4& a, const Real4& b)
{
	return Real4(MathReal::Min(a.x, b.x), MathReal::Min(a.y, b.y), MathReal::Min(a.z, b.z), MathReal::Min(a.w, b.w));
}
inline real Dot(const Real2& a, const Real2& b)
{
	return a.x * b.x + a.y * b.y;
}
inline real Dot(const Real3& a, const Real3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline real Dot(const Real4& a, const Real4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
inline real SqrMagnitude(const Real2& value)
{
	return Dot(value, value);
}
inline real SqrMagnitude(const Real3& value)
{
	return Dot(value, value);
}
inline real SqrMagnitude(const Real4& value)
{
	return Dot(value, value);
}
inline real Magnitude(const Real2& value)
{
	return MathReal::Sqrt(SqrMagnitude(value));
}
inline real Magnitude(const Real3& value)
{
	return MathReal::Sqrt(SqrMagnitude(value));
}
inline real Magnitude(const Real4& value)
{
	return MathReal::Sqrt(SqrMagnitude(value));
}
inline real Angle(const Real2& a, const Real2& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(SqrMagnitude(a) * SqrMagnitude(b));
	return MathReal::Acos(d);
}
inline real Angle(const Real3& a, const Real3& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(SqrMagnitude(a) * SqrMagnitude(b));
	return MathReal::Acos(d);
}
inline real Angle(const Real4& a, const Real4& b)
{
	real d = Dot(a, b);
	d /= MathReal::Sqrt(SqrMagnitude(a) * SqrMagnitude(b));
	return MathReal::Acos(d);
}
inline Real2 Normalized(const Real2& value)
{
	real sm = SqrMagnitude(value);
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real2(value.x / l, value.y / l);
	}
	return Real2(0, 0);
}
inline Real3 Normalized(const Real3& value)
{
	real sm = SqrMagnitude(value);
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real3(value.x / l, value.y / l, value.z / l);
	}
	return Real3(0, 0, 0);
}
inline Real4 Normalized(const Real4& value)
{
	real sm = SqrMagnitude(value);
	if (sm > 0)
	{
		real l = MathReal::Sqrt(sm);
		return Real4(value.x / l, value.y / l, value.z / l, value.w / l);
	}
	return Real4(0, 0, 0, 0);
}
inline real Cross(const Real2& a, const Real2& b)
{
	return a.x * b.y - b.x * a.y;
}
inline Real3 Cross(const Real3& a, const Real3& b)
{
	return Real3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}