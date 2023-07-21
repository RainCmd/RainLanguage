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
/// ������С��λ
/// </summary>
constexpr integer DECIMAL = 16;
/// <summary>
/// ������
/// </summary>
struct RAINLANGUAGE Real
{
private:
	integer value;
	friend class MathReal;
public:
	Real();
	/// <summary>
	/// ����ת������
	/// </summary>
	/// <param name="value">����ֵ</param>
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
	/// ������ת����
	/// </summary>
	explicit operator integer();
	/// <summary>
	/// ֱ��ת��ָ������
	/// </summary>
	/// <param name="value">����</param>
	/// <returns>�������ڴ���ͬ�Ķ�����</returns>
	static Real CreateReal(integer value);
};
typedef Real real;
#else
typedef double real;
#endif

typedef int int32;
typedef wchar_t character;

/// <summary>
/// ���������罻�����ַ���
/// </summary>
struct RAINLANGUAGE RainString
{
	/// <summary>
	/// �ַ��׵�ַ
	/// ע�⣺������ָ���Ǵ�������д������ģ���ô�������Ҫ��������һ���ڴ棬���Ҳ�Ҫ����ָ�봫�������
	/// </summary>
	const character* value;
	/// <summary>
	/// �ַ�������
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
	/// �����ַ���
	/// </summary>
	/// <param name="value">�ַ�����\0��β</param>
	/// <returns>�����õ��ַ���</returns>
	inline static RainString Create(const character* value)
	{
		uint32 length = 0;
		while (value[length]) length++;
		return RainString(value, length);
	}
};
/// <summary>
/// ������������ʱ����ľ�̬����
/// </summary>
/// <returns></returns>
RAINLANGUAGE void ClearStaticCache();