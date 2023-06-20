#pragma once
#include "Rain.h"

/// <summary>
/// 二维向量
/// </summary>
struct RAINLANGUAGE Real2
{
	real x, y;
	explicit Real2(const real& x, const real& y);

	Real2 operator - () const;
	Real2 operator + (const Real2& other) const;
	Real2 operator - (const Real2& other) const;
	/// <summary>
	/// 各分量相乘
	/// </summary>
	Real2 operator * (const Real2& other) const;
	Real2 operator * (const real& other) const;
	friend Real2 operator * (const real& value, const Real2& vector);
	/// <summary>
	/// 各分量相除
	/// </summary>
	Real2 operator / (const Real2& other) const;
	Real2 operator / (const real& other) const;
	friend Real2 operator / (const real& value, const Real2& vector);
	bool operator == (const Real2& other) const;
	bool operator != (const Real2& other) const;
};
struct RAINLANGUAGE Real3
{
	real x, y, z;
	explicit Real3(const real& x, const real& y, const real& z);

	Real3 operator - () const;
	Real3 operator + (const Real3& other) const;
	Real3 operator - (const Real3& other) const;
	/// <summary>
	/// 各分量相乘
	/// </summary>
	Real3 operator * (const Real3& other) const;
	Real3 operator * (const real& other) const;
	friend Real3 operator * (const real& value, const Real3& vector);
	/// <summary>
	/// 各分量相除
	/// </summary>
	Real3 operator / (const Real3& other) const;
	Real3 operator / (const real& other) const;
	friend Real3 operator / (const real& value, const Real3& vector);
	bool operator == (const Real3& other) const;
	bool operator != (const Real3& other) const;
};
struct RAINLANGUAGE Real4
{
	real x, y, z, w;
	explicit Real4(const real& x, const real& y, const real& z, const real& w);

	Real4 operator - () const;
	Real4 operator + (const Real4& other) const;
	Real4 operator - (const Real4& other) const;
	/// <summary>
	/// 各分量相乘
	/// </summary>
	Real4 operator * (const Real4& other) const;
	Real4 operator * (const real& other) const;
	friend Real4 operator * (const real& value, const Real4& vector);
	/// <summary>
	/// 各分量相除
	/// </summary>
	Real4 operator / (const Real4& other) const;
	Real4 operator / (const real& other) const;
	friend Real4 operator / (const real& value, const Real4& vector);
	bool operator == (const Real4& other) const;
	bool operator != (const Real4& other) const;
};
