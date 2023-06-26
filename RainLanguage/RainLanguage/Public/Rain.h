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

