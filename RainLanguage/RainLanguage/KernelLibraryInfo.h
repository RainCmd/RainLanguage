#pragma once
#include "RainLanguage.h"
#include "Library.h"
#include "Collections/List.h"
#include "Type.h"
#include "KernelDeclarations.h"

class Kernel;
class Coroutine;
typedef String(*KernelInvoker)(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);
/**
* space kernel
*	enum CoroutineState
*		Unstart
*		Running
*		Completed
*		Aborted
*		Invalid
*	struct bool
*		string ToString()
*	struct byte
*		string ToString()
*	struct char
*		string ToString()
*	struct integer
*		string ToString()
*	struct real
*		string ToString()
*	struct real2
*		real2 Normalized()
*		real Magnitude()
*		real SqrMagnitude()
*	struct real3
*		real3 Normalized()
*		real Magnitude()
*		real SqrMagnitude()
*	struct real4
*		real4 Normalized()
*		real Magnitude()
*		real SqrMagnitude()
*	internal struct enum
*		string ToString() Declaration									//函数名后的Declaration是编译器添加的参数，对用户透明
*	struct type
*		bool IsPublic()
*		Reflection.ReadonlyStrings GetAttributes()
*		string GetName()
*		type GetParent()
*		Reflection.ReadonlyTypes GetInherits()
*		Reflection.ReadonlyMemberConstructors GetConstructors()			//只返回当前类型中定义的构造函数
*		Reflection.ReadonlyMemberVariables GetVariables()				//只返回当前类型中定义的字段
*		Reflection.ReadonlyMemberFunctions GetFunctions()				//只返回当前类型中定义的函数
*		Reflection.Space GetSpace()
*		Reflection.TypeCode GetTypeCode()
*		bool IsAssignable(type)											//判断当前类型的对象是否可赋值给目标类型的变量
*		bool IsValid()
*		integer[] GetEnumElements()
*		Reflection.ReadonlyTypes GetParameters()						//委托的参数类型列表
*		Reflection.ReadonlyTypes GetReturns()							//委托或携程的返回值类型列表
*		handle CreateDelegate(Reflection.Function)														//创建全局函数的委托对象，返回当前类型的委托对象
*		handle CreateDelegate(Reflection.Native)														//创建本地函数的委托对象，返回当前类型的委托对象
*		handle CreateDelegate(Reflection.MemberFunction, handle target)									//创建成员函数的委托对象，返回当前类型的委托对象
*		handle StartCoroutine(Reflection.Function function, handle[] parameters)						//创建并执行一个携程，返回当前类型的携程对象
*		handle StartCoroutine(Reflection.MemberFunction function, handle target, handle[] parameters)	//创建并执行一个携程，返回当前类型的携程对象
*		integer GetArrayRank()
*		type GetArrayElementType()
*	struct string							//GC字段加上自身
*		integer GetLength()
*		integer GetStringID()
*		bool ToBool()
*		integer ToInteger()
*		real ToReal()
*	struct entity							//GC字段加上自身
*		integer GetEntityID()
*	class handle
*		integer GetHandleID()
*		string ToString()
*		type GetType()
*	internal class interface handle
*	class delegate handle
*	class coroutine handle
*		Start(bool immediately, bool ignoreWait)
*		Abort()
*		CoroutineState GetState()
*		string GetExitCode()
*		bool IsPause()
*		Pause()
*		Resume()
*	class array handle
*		integer GetLength()
*
*	bool operator < (integer, integer)
*	bool operator < (real, real)
*	bool operator <= (integer, integer)
*	bool operator <= (real, real)
*	bool operator > (integer, integer)
*	bool operator > (real, real)
*	bool operator >= (integer, integer)
*	bool operator >= (real, real)
*	bool operator == (bool, bool)
*	bool operator == (integer, integer)
*	bool operator == (real, real)
*	bool operator == (real2, real2)
*	bool operator == (real3, real3)
*	bool operator == (real4, real4)
*	bool operator == (string, string)
*	bool operator == (handle, handle)
*	bool operator == (entity, entity)
*	bool operator == (delegate, delegate)
*	bool operator != (bool, bool)
*	bool operator != (integer, integer)
*	bool operator != (real, real)
*	bool operator != (real2, real2)
*	bool operator != (real3, real3)
*	bool operator != (real4, real4)
*	bool operator != (string, string)
*	bool operator != (handle, handle)
*	bool operator != (entity, entity)
*	bool operator != (delegate, delegate)
*	bool operator & (bool, bool)
*	integer operator & (integer, integer)
*	bool operator | (bool, bool)
*	integer operator | (integer, integer)
*	bool operator ^ (bool, bool)
*	integer operator ^ (integer, integer)
*	integer operator << (integer, integer)
*	integer operator >> (integer, integer)
*	integer operator + (integer, integer)
*	real operator + (real, real)
*	real2 operator + (real2, real2)
*	real3 operator + (real3, real3)
*	real4 operator + (real4, real4)
*	string operator + (string, string)
*	string operator + (string, bool)
*	string operator + (string, char)
*	string operator + (string, integer)
*	string operator + (string, real)
*	string operator + (string, handle)		//如果通过反射调用则会对handle的ToString做实调用，这是因为子类的ToString函数中可能有wait操作，而反射逻辑是在c++中执行的，无法保存执行栈来等待ToString返回
*	string operator + (bool, string)
*	string operator + (char, string)
*	string operator + (integer, string)
*	string operator + (real, string)
*	string operator + (handle, string)		//如果通过反射调用则会对handle的ToString做实调用，这是因为子类的ToString函数中可能有wait操作，而反射逻辑是在c++中执行的，无法保存执行栈来等待ToString返回
*	integer operator - (integer, integer)
*	real operator - (real, real)
*	real2 operator - (real2, real2)
*	real3 operator - (real3, real3)
*	real4 operator - (real4, real4)
*	integer operator * (integer, integer)
*	real operator * (real, real)
*	real2 operator * (real2, real)
*	real3 operator * (real3, real)
*	real4 operator * (real4, real)
*	real2 operator * (real, real2)
*	real3 operator * (real, real3)
*	real4 operator * (real, real4)
*	real2 operator * (real2, real2)
*	real3 operator * (real3, real3)
*	real4 operator * (real4, real4)
*	integer operator / (integer, integer)
*	real operator / (real, real)
*	real2 operator / (real2, real)
*	real3 operator / (real3, real)
*	real4 operator / (real4, real)
*	real2 operator / (real, real2)
*	real3 operator / (real, real3)
*	real4 operator / (real, real4)
*	real2 operator / (real2, real2)
*	real3 operator / (real3, real3)
*	real4 operator / (real4, real4)
*	integer operator % (integer, integer)
*	bool operator ! (bool)
*	bool operator ~ (bool)
*	integer operator ~ (integer)
*	integer operator + (integer)
*	real operator + (real)
*	real2 operator + (real2)
*	real3 operator + (real3)
*	real4 operator + (real4)
*	integer operator - (integer)
*	real operator - (real)
*	real2 operator - (real2)
*	real3 operator - (real3)
*	real4 operator - (real4)
*	operator ++ (integer)			//反射调用该函数无效果，这里只是个声明
*	operator ++ (real)				//反射调用该函数无效果，这里只是个声明
*	operator -- (integer)			//反射调用该函数无效果，这里只是个声明
*	operator -- (real)				//反射调用该函数无效果，这里只是个声明
*
*	space BitConvert
*		integer BytesConvertInteger(byte, byte, byte, byte, byte, byte, byte, byte)
*		real BytesConvertReal(byte, byte, byte, byte, byte, byte, byte, byte)
*		string BytesConvertString(byte[])
*		byte, byte, byte, byte, byte, byte, byte, byte IntegerConvertBytes(integer)
*		byte, byte, byte, byte, byte, byte, byte, byte RealConvertBytes(real)
*		byte[] StringConvertBytes(string)
*
*	space Math
*		real PI
*		real E
*		real Rad2Deg
*		real Deg2Rad
*
*		integer Abs(integer)
*		integer Clamp(integer, integer, integer)
*		integer GetRandomInt()
*		integer Max(integer, integer)
*		integer Min(integer, integer)
*		real Abs(real)
*		real Acos(real)
*		real Asin(real)
*		real Atan(real)
*		real Atan2(real, real)
*		integer Ceil(real)
*		real Clamp(real, real, real)
*		real Clamp01(real)
*		real Cos(real)
*		integer Floor(real)
*		real GetRandomReal()
*		real Lerp(real, real, real)
*		real Max(real, real)
*		real Min(real, real)
*		integer Round(real)
*		integer Sign(real)
*		real Sin(real)
*		real, real SinCos(real)
*		real Sqrt(real)
*		real Tan(real)
*		real Angle(real2, real2)
*		real Cross(real2, real2)
*		real Dot(real2, real2)
*		real2 Lerp(real2, real2, real)
*		real2 Max(real2, real2)
*		real2 Min(real2, real2)
*		real Angle(real3, real3)
*		real3 Cross(real3, real3)
*		real Dot(real3, real3)
*		real3 Lerp(real3, real3, real)
*		real3 Max(real3, real3)
*		real3 Min(real3, real3)
*		real Angle(real4, real4)
*		real Dot(real4, real4)
*		real4 Lerp(real4, real4, rea;)
*		real4 Max(real4, real4)
*		real4 Min(real4, real4)
*
*	space System
*		space Exceptions
*			string NullReference
*			string InvalidCoroutine
*			string OutOfRange
*			string InvalidTypeCode
*			string CoroutineNotCompleted
*			string DivideByZero
*			string InvalidCast
*
*		integer Collect(bool)
*		integer HeapTotalMemory()
*		integer CountHandle()
*		integer CountCoroutine()
*		integer EntityCount()
*		integer StringCount()
*		SetRandomSeed(integer)
*		Reflection.Assembly[] GetAssembles()
*
*	space Reflection
*		enum TypeCode
*			Invalid
*			Bool
*			Byte
*			Char
*			Integer
*			Real
*			Real2
*			Real3
*			Real4
*			Enum
*			Type
*			String
*			Entity
*			Handle
*			Interface
*			Delegate
*			Coroutine
*			Array
*		class ReadonlyStrings
*			string[] values
*			string GetElement(integer)
*			integer GetCount()
*		class ReadonlyTypes
*			type[] values
*			type GetElement(integer)
*			integer GetCount()
*		class ReadonlyVariables
*			Variable[] values
*			Variable GetElement(integer)
*			integer GetCount()
*		class ReadonlyMemberConstructors
*			MemberConstructor[] values
*			MemberConstructor GetElement(integer)
*			integer GetCount()
*		class ReadonlyMemberVariables
*			MemberVariable[] values
*			MemberVariable GetElement(integer)
*			integer GetCount()
*		class ReadonlyMemberFunctions
*			MemberFunction[] values
*			MemberFunction GetElement(integer)
*			integer GetCount()
*		class ReadonlyFunctions
*			Function[] values
*			Function GetElement(integer)
*			integer GetCount()
*		class ReadonlyNatives
*			Native[] values
*			Native GetElement(integer)
*			integer GetCount()
*		class ReadonlySpaces
*			Space[] values
*			Space GetElement(integer)
*			integer GetCount()
*		class Variable
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			Space GetSpace()
*			string GetName()
*			type GetVariableType()
*			handle GetValue()
*			SetValue(handle)
*		class MemberConstructor
*			ReadonlyTypes parameters
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			type GetDeclaringType()
*			ReadonlyTypes GetParameters()
*			InvokeResult Invoke(handle[])
*		class MemberVariable
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			type GetDeclaringType()
*			string GetName()
*			type GetVariableType()
*			handle GetValue(handle)
*			SetValue(handle, handle)
*		class MemberFunction
*			ReadonlyTypes parameters
*			ReadonlyTypes returns
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			type GetDeclaringType()
*			string GetName()
*			ReadonlyTypes GetParameters()
*			ReadonlyTypes GetReturns()
*			InvokeResult Invoke(handle, handle[])
*		class Function
*			ReadonlyTypes parameters
*			ReadonlyTypes returns
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			Space GetSpace()
*			string GetName()
*			ReadonlyTypes GetParameters()
*			ReadonlyTypes GetReturns()
*			InvokeResult Invoke(handle[])
*		class Native
*			ReadonlyTypes parameters
*			ReadonlyTypes returns
*			bool IsPublic()
*			ReadonlyStrings GetAttributes()
*			Space GetSpace()
*			string GetName()
*			ReadonlyTypes GetParameters()
*			ReadonlyTypes GetReturns()
*			handle[] Invoke(handle[])
*		class Space
*			ReadonlyStrings attributes
*			ReadonlySpaces children
*			ReadonlyTypes types
*			ReadonlyVariables variables
*			ReadonlyFunctions functions
*			ReadonlyNatives natives
*			ReadonlyStrings GetAttributes()
*			Space GetParent()
*			ReadonlySpaces GetChildren()
*			Assembly GetAssembly()
*			string GetName()
*			ReadonlyVariables GetVariables()
*			ReadonlyFunctions GetFunctions()
*			ReadonlyNatives GetNatives()
*			ReadonlyTypes GetTypes()
*		class Assembly Space
*/
class KernelLibraryInfo
{
public:
	struct Space
	{
		String name;
		Space* parent;
		List<Space*, true> children;
		List<uint32, true> variables;
		List<uint32, true> enums;
		List<uint32, true> structs;
		List<uint32, true> classes;
		List<uint32, true> interfaces;
		List<uint32, true> delegates;
		List<uint32, true> coroutines;
		List<uint32, true> functions;
		List<uint32, true> natives;
		inline Space(String name) :name(name), parent(NULL), children(0), variables(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), functions(0), natives(0) {}
		inline Space(String name, Space* parent) : name(name), parent(parent), children(0), variables(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), functions(0), natives(0)
		{
			parent->children.Add(this);
		}
	};
	struct Variable
	{
		String name;
		Type type;
		uint32 address;
		inline Variable(const String& name, const Type& type, uint32 address) : name(name), type(type), address(address) {}
	};
	struct GlobalVariable :Variable
	{
		inline GlobalVariable(const String& name, const Type& type, uint32 address) :Variable(name, type, address) {}
	};
	struct Enum
	{
		struct Element
		{
			String name;
			integer value;
			inline Element(const String& name, integer value) :name(name), value(value) {}
		};
		String name;
		List<Element> elements;
		inline Enum(const String& name, const List<Element>& elements) : name(name), elements(elements) {}
	};
	struct Struct
	{
		String name;
		uint32 size;
		uint8 alignment;
		List<Variable> variables;
		List<uint32, true> functions;
		inline Struct(const String& name, uint32 size, uint8 alignment, const List<Variable>& variables, const List<uint32, true>& functions) : name(name), size(size), alignment(alignment), variables(variables), functions(functions) {}
	};
	struct Class
	{
		Declaration parent;
		List<Declaration, true> inherits;
		String name;
		uint32 size;
		uint8 alignment;
		List<uint32, true> constructors;
		List<Variable> variables;
		List<uint32, true> functions;
		inline Class(const Declaration& parent, const List<Declaration, true>& inherits, const String& name, uint32 size, uint8 alignment, const List<uint32, true>& constructors, const List<Variable>& variables, const List<uint32, true>& functions) : parent(parent), inherits(inherits), name(name), size(size), alignment(alignment), constructors(constructors), variables(variables), functions(functions) {}
	};
	struct Interface
	{
		struct Function :CallableInfo
		{
			String name;
		};
		List<Declaration, true> inherits;
		String name;
		List<Function, true> functions;
	};
	struct Delegate :CallableInfo
	{
		String name;
	};
	struct Coroutine
	{
		String name;
		TupleInfo returns;
	};
	struct Function :CallableInfo
	{
		String name;
		KernelInvoker invoker;
		inline Function(const String& name, const TupleInfo& returns, const TupleInfo& parameters, KernelInvoker invoker) :name(name), CallableInfo(returns, parameters), invoker(invoker) {}
	};
	Space* root;
	List<uint8, true> data;
	List<GlobalVariable> variables;
	List<Enum> enums;
	List<Struct> structs;
	List<Class> classes;
	List<Interface> interfaces;
	List<Delegate> delegates;
	List<Coroutine> coroutines;
	List<Function> functions;
	List<StringAddresses>dataStrings;
	KernelLibraryInfo(const KernelLibraryInfo&) = delete;
	KernelLibraryInfo(const KernelLibraryInfo&&) = delete;
	static const KernelLibraryInfo* GetKernelLibraryInfo();
private:
	KernelLibraryInfo();
	inline uint32 AddData(real value)
	{
		uint32 result = MemoryAlignment(data.Count(), MEMORY_ALIGNMENT_REAL);
		data.SetCount(result);
		data.Add((uint8*)&value, 8);
		return result;
	}
	uint32 AddData(const character* value);
};
