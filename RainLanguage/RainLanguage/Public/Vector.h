#pragma once
#include "Rain.h"

struct RAINLANGUAGE Real2
{
	real x, y;
	explicit Real2(const real& x, const real& y);

	Real2 operator - () const;
	Real2 operator + (const Real2& other) const;
	Real2 operator - (const Real2& other) const;
	Real2 operator * (const Real2& other) const;
	Real2 operator * (const real& other) const;
	friend Real2 operator * (const real& value, const Real2& vector);
	Real2 operator / (const Real2& other) const;
	Real2 operator / (const real& other) const;
	friend Real2 operator / (const real& value, const Real2& vector);
	bool operator == (const Real2& other) const;
	bool operator != (const Real2& other) const;

	Real2 GetNormalized() const;
	real GetSqrMagnitude() const;
	real GetMagnitude() const;
	void SetMagnitude(const real& value);

	static Real2 Lerp(const Real2& a, const Real2& b, const real& l);
	static Real2 Max(const Real2& a, const Real2& b);
	static Real2 Min(const Real2& a, const Real2& b);
	static real Angle(const Real2& a, const Real2& b);
	static real Cross(const Real2& a, const Real2& b);
	static real Dot(const Real2& a, const Real2& b);
};
struct RAINLANGUAGE Real3
{
	real x, y, z;
	explicit Real3(const real& x, const real& y, const real& z);

	Real3 operator - () const;
	Real3 operator + (const Real3& other) const;
	Real3 operator - (const Real3& other) const;
	Real3 operator * (const Real3& other) const;
	Real3 operator * (const real& other) const;
	friend Real3 operator * (const real& value, const Real3& vector);
	Real3 operator / (const Real3& other) const;
	Real3 operator / (const real& other) const;
	friend Real3 operator / (const real& value, const Real3& vector);
	bool operator == (const Real3& other) const;
	bool operator != (const Real3& other) const;

	Real3 GetNormalized() const;
	real GetSqrMagnitude() const;
	real GetMagnitude() const;
	void SetMagnitude(const real& value);

	static Real3 Lerp(const Real3& a, const Real3& b, const real& l);
	static Real3 Max(const Real3& a, const Real3& b);
	static Real3 Min(const Real3& a, const Real3& b);
	static real Angle(const Real3& a, const Real3& b);
	static Real3 Cross(const Real3& a, const Real3& b);
	static real Dot(const Real3& a, const Real3& b);
};
struct RAINLANGUAGE Real4
{
	real x, y, z, w;
	explicit Real4(const real& x, const real& y, const real& z, const real& w);

	Real4 operator - () const;
	Real4 operator + (const Real4& other) const;
	Real4 operator - (const Real4& other) const;
	Real4 operator * (const Real4& other) const;
	Real4 operator * (const real& other) const;
	friend Real4 operator * (const real& value, const Real4& vector);
	Real4 operator / (const Real4& other) const;
	Real4 operator / (const real& other) const;
	friend Real4 operator / (const real& value, const Real4& vector);
	bool operator == (const Real4& other) const;
	bool operator != (const Real4& other) const;

	Real4 GetNormalized() const;
	real GetSqrMagnitude() const;
	real GetMagnitude() const;
	void SetMagnitude(const real& value);

	static Real4 Lerp(const Real4& a, const Real4& b, const real& l);
	static Real4 Max(const Real4& a, const Real4& b);
	static Real4 Min(const Real4& a, const Real4& b);
	static real Angle(const Real4& a, const Real4& b);
	static real Dot(const Real4& a, const Real4& b);
};
