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
	const RainString libraryName;
	/// <summary>
	/// 函数名
	/// </summary>
	const RainString functionName;
	/// <summary>
	/// 库本地代码段地址
	/// </summary>
	uint32 address;

	RainStackFrame(const RainString& libraryName, const RainString& functionName, const uint32& address) : libraryName(libraryName), functionName(functionName), address(address) {}
};

/// <summary>
/// c++调用雨言
/// </summary>
class RAINLANGUAGE InvokerWrapper
{
private:
	uint64 instanceID;
	void* invoker;
public:
	InvokerWrapper();
	InvokerWrapper(void* invoker);
	InvokerWrapper(const InvokerWrapper& other);
	InvokerWrapper(InvokerWrapper&& other) noexcept;
	~InvokerWrapper();

	InvokerWrapper& operator=(const InvokerWrapper& other);

	/// <summary>
	/// 获取所属虚拟机
	/// </summary>
	/// <returns></returns>
	RainKernel* GetKernel();
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
	/// <returns>信息</returns>
	/// <exception>如果调用是无效状态会抛异常</exception>
	const RainString GetExitMessage() const;
	/// <summary>
	/// 开始执行任务
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
	/// <param name="error">异常信息</param>
	void Abort(const RainString& error) const;

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
	/// <returns>枚举类型对应的元素名称</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	const RainString GetEnumNameReturnValue(uint32 index) const;
	/// <summary>
	/// 获取字符串类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>字符串</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	const RainString GetStringReturnValue(uint32 index) const;
	/// <summary>
	/// 获取实体类型的返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	uint64 GetEntityReturnValue(uint32 index) const;

	/// <summary>
	/// 获取数组返回值长度
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <returns>返回值长度</returns>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	uint32 GetArrayReturnValueLength(uint32 index) const;
	/// <summary>
	/// 获取布尔类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetBoolArrayReturnValue(uint32 index, bool*& result) const;
	/// <summary>
	/// 获取字节类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetByteArrayReturnValue(uint32 index, uint8*& result) const;
	/// <summary>
	/// 获取字符类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetCharArrayReturnValue(uint32 index, character*& result) const;
	/// <summary>
	/// 获取整数类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetIntegerArrayReturnValue(uint32 index, integer*& result) const;
	/// <summary>
	/// 获取实数类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetRealArrayReturnValue(uint32 index, real*& result) const;
	/// <summary>
	/// 获取二维向量类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetReal2ArrayReturnValue(uint32 index, Real2*& result) const;
	/// <summary>
	/// 获取三维向量类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetReal3ArrayReturnValue(uint32 index, Real3*& result) const;
	/// <summary>
	/// 获取四维向量类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetReal4ArrayReturnValue(uint32 index, Real4*& result) const;
	/// <summary>
	/// 获取枚举类型值数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetEnumValueArrayReturnValue(uint32 index, integer*& result) const;
	/// <summary>
	/// 获取枚举类型名称数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetEnumNameArrayReturnValue(uint32 index, RainString*& result) const;
	/// <summary>
	/// 获取字符串类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetStringArrayReturnValue(uint32 index, RainString*& result) const;
	/// <summary>
	/// 获取实体类型数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果调用不是已完成状态或返回值类型不正确会抛异常</exception>
	void GetEntityArrayReturnValue(uint32 index, uint64*& result) const;

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
	/// 设置按枚举元素名设置枚举值，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="elementName">名称字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const RainString& elementName) const;
	/// <summary>
	/// 设置按枚举元素名设置枚举值，名称需要以\0结尾，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="elementName">名称字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const character* elementName) const;
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
	/// <param name="value">字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const RainString& value) const;
	/// <summary>
	/// 设置字符串类型参数，字符串需要以\0结尾
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">字符串</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const character* value) const;
	/// <summary>
	/// 设置实体类型参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="value">参数值</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEntityParameter(uint32 index, uint64 value) const;

	/// <summary>
	/// 设置布尔数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const bool* values, uint32 length) const;
	/// <summary>
	/// 设置字节数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const uint8* values, uint32 length) const;
	/// <summary>
	/// 设置字符数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const character* values, uint32 length) const;
	/// <summary>
	/// 设置整数数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const integer* values, uint32 length) const;
	/// <summary>
	/// 设置实数数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const real* values, uint32 length) const;
	/// <summary>
	/// 设置二维向量数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const Real2* values, uint32 length) const;
	/// <summary>
	/// 设置三维向量数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const Real3* values, uint32 length) const;
	/// <summary>
	/// 设置四维向量数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const Real4* values, uint32 length) const;
	/// <summary>
	/// 设置枚举值数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumValueParameter(uint32 index, const integer* values, uint32 length) const;
	/// <summary>
	/// 设置枚举名数组参数，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const RainString* values, uint32 length) const;
	/// <summary>
	/// 设置枚举名数组参数，名称需要以\0结尾，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEnumNameParameter(uint32 index, const character** values, uint32 length) const;
	/// <summary>
	/// 设置字符串数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const RainString* values, uint32 length) const;
	/// <summary>
	/// 设置字符串数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetParameter(uint32 index, const character** values, uint32 length) const;
	/// <summary>
	/// 设置实体数组参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="values">参数数组</param>
	/// <param name="length">参数数组长度</param>
	/// <exception>如果调用不是未调用状态或参数类型不正确会抛异常</exception>
	void SetEntityParameter(uint32 index, const uint64* values, uint32 length) const;
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
	/// <returns>枚举参数名</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual const RainString GetEnumNameParameter(uint32 index) const = 0;
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
	/// <returns>字符串</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual const RainString GetStringParameter(uint32 index) const = 0;
	/// <summary>
	/// 获取实体参数
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>参数值</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual uint64 GetEntityParameter(uint32 index) const = 0;

	/// <summary>
	/// 获取参数数组长度
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <returns>数组长度</returns>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual uint32 GetArrayParameterLength(uint32 index) const = 0;
	/// <summary>
	/// 获取布尔数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetBoolArrayParameter(uint32 index, bool*& result) const = 0;
	/// <summary>
	/// 获取字节数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetByteArrayParameter(uint32 index, uint8*& result) const = 0;
	/// <summary>
	/// 获取字符数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetCharArrayParameter(uint32 index, character*& result) const = 0;
	/// <summary>
	/// 获取整数数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetIntegerArrayParameter(uint32 index, integer*& result) const = 0;
	/// <summary>
	/// 获取实数数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetRealArrayParameter(uint32 index, real*& result) const = 0;
	/// <summary>
	/// 获取二维向量数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetReal2ArrayParameter(uint32 index, Real2*& result) const = 0;
	/// <summary>
	/// 获取三维向量数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetReal3ArrayParameter(uint32 index, Real3*& result) const = 0;
	/// <summary>
	/// 获取四维向量数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetReal4ArrayParameter(uint32 index, Real4*& result) const = 0;
	/// <summary>
	/// 获取枚举值数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetEnumArrayValueParameter(uint32 index, integer*& result) const = 0;
	/// <summary>
	/// 获取枚举名数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetEnumArrayNameParameter(uint32 index, RainString*& result) const = 0;
	/// <summary>
	/// 获取字符串数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetStringArrayParameter(uint32 index, RainString*& result) const = 0;
	/// <summary>
	/// 获取实体数组参数值
	/// </summary>
	/// <param name="index">参数索引</param>
	/// <param name="result">用于填充返回值数组的地址</param>
	/// <exception>如果参数类型不正确会抛异常</exception>
	virtual void GetEntityArrayParameter(uint32 index, uint64*& result) const = 0;

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
	/// 以枚举元素名设置返回值，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="elementName">枚举元素名字符串</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const RainString& elementName) = 0;
	/// <summary>
	/// 以枚举元素名设置返回值，名称需要以\0结尾，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="elementName">枚举元素名字符串</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* elementName) = 0;
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
	/// <param name="value">字符串</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, const RainString& value) = 0;
	/// <summary>
	/// 设置字符串返回值，字符串需要以\0结尾
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">字符串</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, const character* value) = 0;
	/// <summary>
	/// 设置实体返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="value">返回值</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEntityReturnValue(uint32 index, uint64 value) = 0;

	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, bool* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, uint8* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, character* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, integer* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, real* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real2* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real3* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, Real4* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, RainString* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值，名称需要以\0结尾，名称未找到会赋值为0
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumNameReturnValue(uint32 index, character** values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEnumValueReturnValue(uint32 index, integer* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, RainString* values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetReturnValue(uint32 index, character** values, uint32 length) = 0;
	/// <summary>
	/// 设置数组返回值
	/// </summary>
	/// <param name="index">返回值索引</param>
	/// <param name="values">返回值首地址</param>
	/// <param name="length">返回值数量</param>
	/// <exception>如果返回值类型不正确会抛异常</exception>
	virtual void SetEntityReturnValue(uint32 index, uint64* values, uint32 length) = 0;

	/// <summary>
	/// 设置异常信息
	/// </summary>
	/// <param name="error">异常信息字符串 </param>
	virtual void SetException(const RainString& error) = 0;
};

/// <summary>
/// 虚拟机启动参数
/// </summary>
struct RAINLANGUAGE StartupParameter
{
	/// <summary>
	/// 启动加载的库列表
	/// </summary>
	const RainLibrary** libraries;
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
	/// 任务初始容积
	/// </summary>
	uint32 taskCapacity;
	/// <summary>
	/// 执行栈初始容积
	/// </summary>
	uint32 executeStackCapacity;
	/// <summary>
	/// 任务异常的回调函数
	/// </summary>
	OnExceptionExit onExceptionExit;
	/// <summary>
	/// 调试数据加载器
	/// </summary>
	ProgramDatabaseLoader programDatabaseLoader;

	StartupParameter(const RainLibrary** libraries, uint32 libraryCount, integer seed, uint32 stringCapacity, uint32 entityCapacity, EntityAction onReferenceEntity, EntityAction onReleaseEntity, LibraryLoader libraryLoader, NativeCallerLoader nativeCallerLoader, uint32 heapCapacity, uint32 heapGeneration, uint32 taskCapacity, uint32 executeStackCapacity, OnExceptionExit onExceptionExit, ProgramDatabaseLoader programDatabaseLoader)
		: libraries(libraries), libraryCount(libraryCount), seed(seed), stringCapacity(stringCapacity), entityCapacity(entityCapacity), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(heapCapacity), heapGeneration(heapGeneration), taskCapacity(taskCapacity), executeStackCapacity(executeStackCapacity), onExceptionExit(onExceptionExit), programDatabaseLoader(programDatabaseLoader) {}
	StartupParameter(const RainLibrary** libraries, uint32 libraryCount, EntityAction onReferenceEntity, EntityAction onReleaseEntity, LibraryLoader libraryLoader, NativeCallerLoader nativeCallerLoader, OnExceptionExit onExceptionExit, ProgramDatabaseLoader programDatabaseLoader) : libraries(libraries), libraryCount(libraryCount), seed(0), stringCapacity(8), entityCapacity(8), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(0xff), heapGeneration(8), taskCapacity(8), executeStackCapacity(0xff), onExceptionExit(onExceptionExit), programDatabaseLoader(programDatabaseLoader) {}
};

/// <summary>
/// 虚拟机状态
/// </summary>
struct RAINLANGUAGE RainKernelState
{
	/// <summary>
	/// 当前任务数量
	/// </summary>
	uint32 taskCount;
	/// <summary>
	/// 当前字符串数量
	/// </summary>
	uint32 stringCount;
	/// <summary>
	/// 当前实体数量
	/// </summary>
	uint32 entityCount;
	/// <summary>
	/// 当前托管对象数量
	/// </summary>
	uint32 handleCount;
	/// <summary>
	/// 当前托管堆大小
	/// </summary>
	uint32 heapSize;

	RainKernelState(const uint32& taskCount, const uint32& stringCount, const uint32& entityCount, const uint32& handleCount, const uint32& heapSize) : taskCount(taskCount), stringCount(stringCount), entityCount(entityCount), handleCount(handleCount), heapSize(heapSize) {}
};

/// <summary>
/// 类型列表
/// </summary>
struct RAINLANGUAGE RainTypes
{
private:
	RainType* types;
	uint32 count;
public:
	RainTypes(RainType* types, uint32 count) :types(types), count(count) {}
	RainTypes(const RainTypes&) = delete;
	RainTypes(RainTypes& other);
	RainTypes(RainTypes&& other)noexcept;
	RainTypes& operator=(RainTypes& other);
	/// <summary>
	/// 获取类型数量
	/// </summary>
	/// <returns>类型数量</returns>
	inline uint32 Count() const { return count; }
	/// <summary>
	/// 获取类型
	/// </summary>
	/// <param name="index">下标</param>
	/// <returns>类型</returns>
	inline const RainType operator[](uint32 index) const { return types[index]; }
};

/// <summary>
/// 雨言的函数句柄
/// </summary>
class RAINLANGUAGE RainFunction
{
	uint32 library;
	uint32 index;
	void* share;
public:
	RainFunction();
	RainFunction(uint32 library, uint32 index, void* share);
	~RainFunction();

	RainFunction& operator=(const RainFunction& other);

	/// <summary>
	/// 判断是否是个有效的句柄
	/// </summary>
	/// <returns>是个有效的句柄</returns>
	bool IsValid() const;
	/// <summary>
	/// 创建一个函数的调用
	/// </summary>
	/// <returns>函数的调用</returns>
	InvokerWrapper CreateInvoker() const;
	/// <summary>
	/// 获取该函数的参数列表
	/// </summary>
	/// <returns>类型列表</returns>
	RainTypes GetParameters() const;
	/// <summary>
	/// 获取函数的返回值列表
	/// </summary>
	/// <returns>类型列表</returns>
	RainTypes GetReturns() const;

};

/// <summary>
/// 函数句柄列表
/// </summary>
struct RAINLANGUAGE RainFunctions
{
private:
	RainFunction* functions;
	uint32 count;
public:
	RainFunctions(RainFunction* functions, uint32 count) :functions(functions), count(count) {}
	RainFunctions(const RainFunctions&) = delete;
	RainFunctions(RainFunctions& other);
	RainFunctions(RainFunctions&& other) noexcept;
	RainFunctions& operator=(RainFunctions& other);
	/// <summary>
	/// 获取句柄数量
	/// </summary>
	/// <returns>句柄数量</returns>
	inline uint32 Count() const { return count; }
	/// <summary>
	/// 获取函数句柄
	/// </summary>
	/// <param name="index">下标</param>
	/// <returns>函数句柄</returns>
	inline const RainFunction operator[](uint32 index) const { return functions[index]; }
	~RainFunctions();
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
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配第一个名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="allowNoPublic">允许查找私有函数</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction FindFunction(const RainString& name, bool allowNoPublic) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配第一个名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="allowNoPublic">允许查找私有函数</param>
	/// <returns>函数句柄</returns>
	virtual const RainFunction FindFunction(const character* name, bool allowNoPublic) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配所有名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="allowNoPublic">允许查找私有函数</param>
	/// <returns>函数句柄</returns>
	virtual RainFunctions FindFunctions(const RainString& name, bool allowNoPublic) = 0;
	/// <summary>
	/// 查找函数，只能查找全局的公开函数，使用'.'分隔库名、空间名和函数名，没有'.'分隔则会遍历所有已加载库的公开全局函数，匹配所有名称相等的函数
	/// 查找失败会返回一个无效的句柄
	/// </summary>
	/// <param name="name">函数名</param>
	/// <param name="allowNoPublic">允许查找私有函数</param>
	/// <returns>函数句柄</returns>
	virtual RainFunctions FindFunctions(const character* name, bool allowNoPublic) = 0;
	/// <summary>
	/// 获取当前状态数据
	/// </summary>
	/// <returns>状态数据</returns>
	virtual const RainKernelState GetState() = 0;
	/// <summary>
	/// 触发托管堆垃圾回收
	/// </summary>
	/// <param name="full">进行完整的垃圾回收</param>
	/// <returns>本次垃圾回收释放的托管堆大小</returns>
	virtual uint32 GC(bool full) = 0;

	/// <summary>
	/// 更新虚拟机
	/// </summary>
	virtual void Update() = 0;
};


RAINLANGUAGE RainKernel* CreateKernel(const StartupParameter& parameter);

RAINLANGUAGE void Delete(RainKernel*& kernel);