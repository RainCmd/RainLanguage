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
constexpr integer DECIMAL = 16;
struct RAINLANGUAGE Real
{
private:
	integer value;
	friend class MathReal;
public:
	Real();
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
	explicit operator integer();
	static Real CreateReal(integer value);
};
typedef Real real;
#else
typedef double real;
#endif

typedef int int32;
typedef wchar_t character;

class RAINLANGUAGE RainLibrary
{
public:
	RainLibrary() = default;
	virtual ~RainLibrary() {};
};
typedef const RainLibrary* (*LibraryLoader)(const character* name, uint32 length);
RAINLANGUAGE uint8* Serialize(const RainLibrary* library, uint32& size);
RAINLANGUAGE const RainLibrary* Deserialize(uint8* data, uint32 size);

