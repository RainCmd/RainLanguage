#pragma once

#ifdef RAINLANGUAGE_EXPORTS
#define RAINLANGUAGE _declspec(dllexport)
#else
#define RAINLANGUAGE _declspec(dllimport)
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

/// <summary>
/// 虚拟机与外界交互的字符串
/// </summary>
struct RAINLANGUAGE RainString
{
	/// <summary>
	/// 字符首地址
	/// 注意：如果这个指针是从虚拟机中传出来的，那么本地如果要保存则复制一份内存，并且不要将该指针传回虚拟机
	/// </summary>
	const character* value;
	/// <summary>
	/// 字符串长度
	/// </summary>
	uint32 length;
	inline RainString(const character* value, uint32 length) : value(value), length(length) {}
	inline RainString(const RainString& other) : value(other.value), length(other.length) {}
	inline RainString& operator=(const RainString& other)
	{
		value = other.value;
		length = other.length;
		return *this;
	}
	/// <summary>
	/// 创建字符串
	/// </summary>
	/// <param name="value">字符串，\0结尾</param>
	/// <returns>交互用的字符串</returns>
	inline static RainString Create(const character* value)
	{
		uint32 length = 0;
		while (value[length]) length++;
		return RainString(value, length);
	}
};
/// <summary>
/// 清理编译和运行时缓存的静态变量
/// </summary>
/// <returns></returns>
RAINLANGUAGE void ClearStaticCache();