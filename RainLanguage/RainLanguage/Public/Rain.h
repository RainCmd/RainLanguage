#pragma once

#ifdef RAINLANGUAGE_EXPORTS
#define RAINLANGUAGE _declspec(dllexport)
#else
#define RAINLANGUAGE //_declspec(dllimport)
#endif

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef long long integer;
#ifdef FIXED_REAL
/// <summary>
/// 定点数小数位
/// </summary>
constexpr integer DECIMAL = 16;
/// <summary>
/// 定点数
/// </summary>
struct RAINLANGUAGE Real
{
private:
	integer value;
	friend class MathReal;
public:
	Real();
	/// <summary>
	/// 整数转定点数
	/// </summary>
	/// <param name="value">整数值</param>
	Real(integer value);
	Real operator + () const;
	Real operator ++ ();
	Real operator ++ (int);
	Real operator - () const;
	Real operator -- ();
	Real operator -- (int);
	Real operator + (const Real& other) const;
	Real operator += (const Real& other);
	Real operator - (const Real& other) const;
	Real operator -= (const Real& other);
	Real operator * (const Real& other) const;
	Real operator *= (const Real& other);
	Real operator / (const Real& other) const;
	Real operator /= (const Real& other);
	Real operator % (const Real& other) const;
	Real operator %= (const Real& other);
	bool operator == (const Real& other) const;
	bool operator != (const Real& other) const;
	bool operator > (const Real& other) const;
	bool operator >= (const Real& other) const;
	bool operator < (const Real& other) const;
	bool operator <= (const Real& other) const;
	/// <summary>
	/// 定点数转整数
	/// </summary>
	explicit operator integer();
	/// <summary>
	/// 直接转换指针类型
	/// </summary>
	/// <param name="value">整数</param>
	/// <returns>与整数内存相同的定点数</returns>
	static Real CreateReal(integer value);
};
typedef Real real;
#else
typedef double real;
#endif

typedef int int32;
typedef wchar_t character;

