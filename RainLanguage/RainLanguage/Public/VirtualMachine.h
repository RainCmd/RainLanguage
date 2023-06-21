#pragma once
#include "VirtualMachineDefinitions.h"
#include "Vector.h"
#include "RainLibrary.h"

/// <summary>
/// 堆栈数据
/// </summary>
struct RAINLANGUAGE RainStackFrame
{
	/// <summary>
	/// 库名
	/// </summary>
	const character* libraryName;
	/// <summary>
	/// 库名长度
	/// </summary>
	uint32 libraryNameLength;
	/// <summary>
	/// 库本地代码段地址
	/// </summary>
	uint32 address;

	RainStackFrame(const character* libraryName, const uint32& libraryNameLength, const uint32& address)
		: libraryName(libraryName), libraryNameLength(libraryNameLength), address(address) {}
};

class Invoker;
/// <summary>
/// c++调用雨言
/// </summary>
class RAINLANGUAGE InvokerWrapper
{
private:
	uint64 instanceID;
	Invoker* invoker;
	void ValidAssert() const;
public:
	InvokerWrapper();
	InvokerWrapper(Invoker* invoker);
	InvokerWrapper(const InvokerWrapper& other);
	InvokerWrapper(InvokerWrapper&& other) noexcept;
	~InvokerWrapper();

	/// <summary>
	/// 获取调用实例ID
	/// </summary>
	/// <returns>调用的实例ID</returns>
	uint64 GetInstanceID() const;
	/// <summary>
	/// 判断是否是一个有效的调用
	/// </summary>
	/// <returns>是有效的调用</returns>
	bool IsValid() const;
	/// <summary>
	/// 获取当前调用状态
	/// </summary>
	/// <returns>当前调用状态</returns>
	InvokerState GetState() const;
	/// <summary>
	/// 获取异常退出时的退出信息
	/// </summary>
	/// <param name="length">信息长度</param>
	/// <returns>信息</returns>
	/// <exception>如果调用是无效状态会抛异常</exception>
	const character* GetExitMessage(uint32& length) const;
	/// <summary>
	/// 开始执行协程
	/// </summary>
	/// <param name="immediately">true:立即执行 false:下次Update执行</param>
	/// <param name="ignoreWait">忽略遇到的wait关键字</param>
	void Start(bool immediately, bool ignoreWait) const;
	/// <summary>
	/// 判断当前调用是否是暂停状态
	/// </summary>
	/// <returns>是暂停状态</returns>
	/// <exception>如果调用不是运行状态会抛异常</exception>
	bool IsPause() const;
	/// <summary>
	/// 暂停当前调用
	/// </summary>
	/// <exception>如果调用不是运行状态会抛异常</exception>
	void Pause() const;
	/// <summary>
	/// 恢复当前调用
	/// </summary>
	/// <exception>如果调用不是运行状态会抛异常</exception>
	void Resume() const;
	/// <summary>
	/// 触发异常
	/// </summary>
	/// <param name="chars">异常信息</param>
	/// <param name="length">异常信息长度</param>
	void Abort(const character* chars, uint32 length) const;

	/// <summary>
	/// 获取布尔类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	bool GetBoolReturnValue(uint32 index) const;
	/// <summary>
	/// 获取字节类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	uint8 GetByteReturnValue(uint32 index) const;
	/// <summary>
	/// 获取字符类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	character GetCharReturnValue(uint32 index) const;
	/// <summary>
	/// 获取整数类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	integer GetIntegerReturnValue(uint32 index) const;
	/// <summary>
	/// 获取实数类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	real GetRealReturnValue(uint32 index) const;
	/// <summary>
	/// 获取二维向量类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	Real2 GetReal2ReturnValue(uint32 index) const;
	/// <summary>
	/// 获取三维向量类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	Real3 GetReal3ReturnValue(uint32 index) const;
	/// <summary>
	/// 获取四维向量类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	Real4 GetReal4ReturnValue(uint32 index) const;
	/// <summary>
	/// 获取枚举类型的整数值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>枚举类型的整数值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	integer GetEnumValueReturnValue(uint32 index) const;
	/// <summary>
	/// 获取枚举类型对应的元素名称
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="length">枚举类型对应的元素名称长度</param>
	/// <returns>枚举类型对应的元素名称</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	const character* GetEnumNameReturnValue(uint32 index, uint32& length) const;
	/// <summary>
	/// 获取字符串类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="length">字符串长度</param>
	/// <returns>字符串</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	const character* GetStringReturnValue(uint32 index, uint32& length) const;
	/// <summary>
	/// 获取实体类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	uint64 GetEntityReturnValue(uint32 index) const;

	/// <summary>
	/// 设置布尔类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, bool value) const;
	/// <summary>
	/// 设置字节类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, uint8 value) const;
	/// <summary>
	/// 设置字符类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, character value) const;
	/// <summary>
	/// 设置整数类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, integer value) const;
	/// <summary>
	/// 设置实数类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, real value) const;
	/// <summary>
	/// 设置二维向量类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, Real2 value) const;
	/// <summary>
	/// 设置三维向量类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, Real3 value) const;
	/// <summary>
	/// 设置四维向量类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, Real4 value) const;
	/// <summary>
	/// 设置按枚举元素名设置枚举值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="chars">名称字符串</param>
	/// <param name="length">名称字符串长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确或名称未找到会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const character* chars, uint32 length) const;
	/// <summary>
	/// 设置按枚举元素名设置枚举值，名称需要以\0结尾
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="chars">名称字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确或名称未找到会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const character* chars) const;
	/// <summary>
	/// 以枚举整数值设置参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumValueParameter(uint32 index, integer value) const;
	/// <summary>
	/// 设置字符串类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="chars">字符串</param>
	/// <param name="length">字符串长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const character* chars, uint32 length) const;
	/// <summary>
	/// 设置字符串类型参数，字符串需要以\0结尾
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="chars">字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const character* chars) const;
	/// <summary>
	/// 设置实体类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEntityParameter(uint32 index, uint64 value) const;
};

/// <summary>
/// 雨言对c++的调用
/// </summary>
class RAINLANGUAGE CallerWrapper
{
public:
	CallerWrapper() = default;
	virtual ~CallerWrapper() {}

	/// <summary>
	/// 获取布尔参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual bool GetBoolParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取字节参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual uint8 GetByteParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取字符参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual character GetCharParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取整数参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual integer GetIntegerParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取实数参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual real GetRealParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取二维向量参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual Real2 GetReal2Parameter(uint32 index) const = 0;
	/// <summary>
	/// 获取三维向量参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual Real3 GetReal3Parameter(uint32 index) const = 0;
	/// <summary>
	/// 获取四维向量参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual Real4 GetReal4Parameter(uint32 index) const = 0;
	/// <summary>
	/// 获取枚举参数名
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="length">枚举参数名长度</param>
	/// <returns>枚举参数名</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual const character* GetEnumNameParameter(uint32 index, uint32& length) const = 0;
	/// <summary>
	/// 获取枚举参数整数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual integer GetEnumValueParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取字符串参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="length">字符串长度</param>
	/// <returns>字符串</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual const character* GetStringParameter(uint32 index, uint32& length) const = 0;
	/// <summary>
	/// 获取实体参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual uint64 GetEntityParameter(uint32 index) const = 0;

	/// <summary>
	/// 设置布尔返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, bool value) = 0;
	/// <summary>
	/// 设置字节返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, uint8 value) = 0;
	/// <summary>
	/// 设置字符返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, character value) = 0;
	/// <summary>
	/// 设置整数返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// 设置实数返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, real value) = 0;
	/// <summary>
	/// 设置二维向量返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real2 value) = 0;
	/// <summary>
	/// 设置三维向量返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real3 value) = 0;
	/// <summary>
	/// 设置四维向量返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real4 value) = 0;
	/// <summary>
	/// 以枚举元素名设置返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="chars">枚举元素名字符串</param>
	/// <param name="length">枚举元素名字符串长度</param>
	/// <exception>如果返回值类型不正确或元素名未找到会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	/// <summary>
	/// 以枚举元素名设置返回值，名称需要以\0结尾
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="chars">枚举元素名字符串</param>
	/// <exception>如果返回值类型不正确或元素名未找到会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars) = 0;
	/// <summary>
	/// 设置枚举返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumValueReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// 设置字符串返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="chars">字符串</param>
	/// <param name="length">字符串长度</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	/// <summary>
	/// 设置字符串返回值，字符串需要以\0结尾
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="chars">字符串</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, const character* chars) = 0;
	/// <summary>
	/// 设置实体返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEntityReturnValue(uint32 index, uint64 value) = 0;

	/// <summary>
	/// 设置异常信息
	/// </summary>
	/// <param name="chars">异常信息字符串 </param>
	/// <param name="length">异常信息字符串长度</param>
	virtual void SetException(const character* chars, uint32 length) = 0;
};

/// <summary>
/// 虚拟机启动参数
/// </summary>
struct RAINLANGUAGE StartupParameter
{
	/// <summary>
	/// 启动加载的库列表
	/// </summary>
	RainLibrary* libraries;
	/// <summary>
	/// 启动加载的库数量
	/// </summary>
	uint32 libraryCount;
	/// <summary>
	/// 随机种子
	/// </summary>
	integer seed;
	/// <summary>
	/// 字符串代理初始容积
	/// </summary>
	uint32 stringCapacity;
	/// <summary>
	/// 实体代理初始容积
	/// </summary>
	uint32 entityCapacity;
	/// <summary>
	/// 虚拟机引用和释放实体时的回调函数
	/// </summary>
	EntityAction onReferenceEntity, onReleaseEntity;
	/// <summary>
	/// 库加载器
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// 本地调用的加载器
	/// </summary>
	NativeCallerLoader nativeCallerLoader;
	/// <summary>
	/// 托管堆初始容积和年轻代最大年龄
	/// </summary>
	uint32 heapCapacity, heapGeneration;
	/// <summary>
	/// 协程初始容积
	/// </summary>
	uint32 coroutineCapacity;
	/// <summary>
	/// 执行栈初始容积
	/// </summary>
	uint32 executeStackCapacity;
	/// <summary>
	/// 协程异常的回调函数
	/// </summary>
	OnExceptionExit onExceptionExit;

	StartupParameter(RainLibrary* libraries, const uint32& libraryCount, const integer& seed, const uint32& stringCapacity, const uint32& entityCapacity, const EntityAction& onReferenceEntity, const EntityAction& onReleaseEntity, const LibraryLoader& libraryLoader, const NativeCallerLoader& nativeCallerLoader, const uint32& heapCapacity, const uint32& heapGeneration, const uint32& coroutineCapacity, const uint32& executeStackCapacity, const OnExceptionExit& onExceptionExit)
		: libraries(libraries), libraryCount(libraryCount), seed(seed), stringCapacity(stringCapacity), entityCapacity(entityCapacity), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(heapCapacity), heapGeneration(heapGeneration), coroutineCapacity(coroutineCapacity), executeStackCapacity(executeStackCapacity), onExceptionExit(onExceptionExit) {}
};

/// <summary>
/// 雨言的函数句柄
/// </summary>
struct RAINLANGUAGE RainFunction
{
private:
	uint32 library;
	uint32 function;
public:
	/// <summary>
	/// 判断是否是个有效的句柄
	/// </summary>
	/// <returns>是个有效的句柄</returns>
	inline bool IsValid() { return library != 0xFFFFFFFF; }
};

/// <summary>
/// 雨言虚拟机
/// </summary>
class RAINLANGUAGE RainKernel
{
public:
	RainKernel() = default;
	virtual ~RainKernel() {};

	/// <summary>
	/// 创建一个调用
	/// </summary>
	/// <param name="function">函数句柄</param>
	/// <returns>调用</returns>
	virtual InvokerWrapper CreateInvoker(const RainFunction& function) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配第一个名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="nameLength">函数名长度</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction FindFunction(const character* name, uint32 nameLength) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配第一个名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction FindFunction(const character* name) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配所有名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="nameLength">函数名长度</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction* FindFunctions(const character* name, uint32 nameLength, uint32& count) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配所有名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction* FindFunctions(const character* name, uint32& count) = 0;

	/// <summary>
	/// 更新虚拟机
	/// </summary>
	virtual void Update() = 0;
};


RAINLANGUAGE RainKernel* CreateKernel(const StartupParameter& parameter);
