#pragma once
#include "VirtualMachineDefinitions.h"
#include "Vector.h"
#include "RainLibrary.h"

/// <summary>
/// ��ջ����
/// </summary>
struct RAINLANGUAGE RainStackFrame
{
	/// <summary>
	/// ����
	/// </summary>
	const RainString libraryName;
	/// <summary>
	/// �Ȿ�ش���ε�ַ
	/// </summary>
	uint32 address;

	RainStackFrame(const RainString& libraryName, const uint32& address) : libraryName(libraryName), address(address) {}
};

class Invoker;
/// <summary>
/// c++��������
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

	InvokerWrapper& operator=(const InvokerWrapper& other);

	/// <summary>
	/// ��ȡ����ʵ��ID
	/// </summary>
	/// <returns>���õ�ʵ��ID</returns>
	uint64 GetInstanceID() const;
	/// <summary>
	/// �ж��Ƿ���һ����Ч�ĵ���
	/// </summary>
	/// <returns>����Ч�ĵ���</returns>
	bool IsValid() const;
	/// <summary>
	/// ��ȡ��ǰ����״̬
	/// </summary>
	/// <returns>��ǰ����״̬</returns>
	InvokerState GetState() const;
	/// <summary>
	/// ��ȡ�쳣�˳�ʱ���˳���Ϣ
	/// </summary>
	/// <returns>��Ϣ</returns>
	/// <exception>�����������Ч״̬�����쳣</exception>
	const RainString GetExitMessage() const;
	/// <summary>
	/// ��ʼִ��Э��
	/// </summary>
	/// <param name="immediately">true:����ִ�� false:�´�Updateִ��</param>
	/// <param name="ignoreWait">����������wait�ؼ���</param>
	void Start(bool immediately, bool ignoreWait) const;
	/// <summary>
	/// �жϵ�ǰ�����Ƿ�����ͣ״̬
	/// </summary>
	/// <returns>����ͣ״̬</returns>
	/// <exception>������ò�������״̬�����쳣</exception>
	bool IsPause() const;
	/// <summary>
	/// ��ͣ��ǰ����
	/// </summary>
	/// <exception>������ò�������״̬�����쳣</exception>
	void Pause() const;
	/// <summary>
	/// �ָ���ǰ����
	/// </summary>
	/// <exception>������ò�������״̬�����쳣</exception>
	void Resume() const;
	/// <summary>
	/// �����쳣
	/// </summary>
	/// <param name="error">�쳣��Ϣ</param>
	void Abort(const RainString& error) const;

	/// <summary>
	/// ��ȡ�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	bool GetBoolReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�ֽ����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	uint8 GetByteReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�ַ����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	character GetCharReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	integer GetIntegerReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡʵ�����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	real GetRealReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real2 GetReal2ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real3 GetReal3ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real4 GetReal4ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡö�����͵�����ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>ö�����͵�����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	integer GetEnumValueReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡö�����Ͷ�Ӧ��Ԫ������
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>ö�����Ͷ�Ӧ��Ԫ������</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	const RainString GetEnumNameReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�ַ������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>�ַ���</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	const RainString GetStringReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡʵ�����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	uint64 GetEntityReturnValue(uint32 index) const;

	/// <summary>
	/// ���ò������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, bool value) const;
	/// <summary>
	/// �����ֽ����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, uint8 value) const;
	/// <summary>
	/// �����ַ����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, character value) const;
	/// <summary>
	/// �����������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, integer value) const;
	/// <summary>
	/// ����ʵ�����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, real value) const;
	/// <summary>
	/// ���ö�ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real2 value) const;
	/// <summary>
	/// ������ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real3 value) const;
	/// <summary>
	/// ������ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real4 value) const;
	/// <summary>
	/// ���ð�ö��Ԫ��������ö��ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="elementName">�����ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ������δ�ҵ������쳣</exception>
	void SetEnumNameParameter(uint32 index, const RainString& elementName) const;
	/// <summary>
	/// ���ð�ö��Ԫ��������ö��ֵ��������Ҫ��\0��β
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="elementName">�����ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ������δ�ҵ������쳣</exception>
	void SetEnumNameParameter(uint32 index, const character* elementName) const;
	/// <summary>
	/// ��ö������ֵ���ò���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetEnumValueParameter(uint32 index, integer value) const;
	/// <summary>
	/// �����ַ������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">�ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, const RainString& value) const;
	/// <summary>
	/// �����ַ������Ͳ������ַ�����Ҫ��\0��β
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">�ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, const character* value) const;
	/// <summary>
	/// ����ʵ�����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetEntityParameter(uint32 index, uint64 value) const;
};

/// <summary>
/// ���Զ�c++�ĵ���
/// </summary>
class RAINLANGUAGE CallerWrapper
{
public:
	CallerWrapper() = default;
	virtual ~CallerWrapper() {}

	/// <summary>
	/// ��ȡ��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual bool GetBoolParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ֽڲ���ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual uint8 GetByteParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ַ�����ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual character GetCharParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual integer GetIntegerParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡʵ������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual real GetRealParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real2 GetReal2Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real3 GetReal3Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real4 GetReal4Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡö�ٲ�����
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>ö�ٲ�����</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual const RainString GetEnumNameParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡö�ٲ�������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual integer GetEnumValueParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ַ�������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>�ַ���</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual const RainString GetStringParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡʵ�����
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual uint64 GetEntityParameter(uint32 index) const = 0;

	/// <summary>
	/// ���ò�������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, bool value) = 0;
	/// <summary>
	/// �����ֽڷ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, uint8 value) = 0;
	/// <summary>
	/// �����ַ�����ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, character value) = 0;
	/// <summary>
	/// ������������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// ����ʵ������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, real value) = 0;
	/// <summary>
	/// ���ö�ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real2 value) = 0;
	/// <summary>
	/// ������ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real3 value) = 0;
	/// <summary>
	/// ������ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real4 value) = 0;
	/// <summary>
	/// ��ö��Ԫ�������÷���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="elementName">ö��Ԫ�����ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ��Ԫ����δ�ҵ������쳣</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const RainString& elementName) = 0;
	/// <summary>
	/// ��ö��Ԫ�������÷���ֵ��������Ҫ��\0��β
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="elementName">ö��Ԫ�����ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ��Ԫ����δ�ҵ������쳣</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* elementName) = 0;
	/// <summary>
	/// ����ö�ٷ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetEnumValueReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// �����ַ�������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">�ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, const RainString& value) = 0;
	/// <summary>
	/// �����ַ�������ֵ���ַ�����Ҫ��\0��β
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">�ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, const character* value) = 0;
	/// <summary>
	/// ����ʵ�巵��ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetEntityReturnValue(uint32 index, uint64 value) = 0;

	/// <summary>
	/// �����쳣��Ϣ
	/// </summary>
	/// <param name="error">�쳣��Ϣ�ַ��� </param>
	virtual void SetException(const RainString& error) = 0;
};

/// <summary>
/// �������������
/// </summary>
struct RAINLANGUAGE StartupParameter
{
	/// <summary>
	/// �������صĿ��б�
	/// </summary>
	const RainLibrary* libraries;
	/// <summary>
	/// �������صĿ�����
	/// </summary>
	uint32 libraryCount;
	/// <summary>
	/// �������
	/// </summary>
	integer seed;
	/// <summary>
	/// �ַ��������ʼ�ݻ�
	/// </summary>
	uint32 stringCapacity;
	/// <summary>
	/// ʵ������ʼ�ݻ�
	/// </summary>
	uint32 entityCapacity;
	/// <summary>
	/// ��������ú��ͷ�ʵ��ʱ�Ļص�����
	/// </summary>
	EntityAction onReferenceEntity, onReleaseEntity;
	/// <summary>
	/// �������
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// ���ص��õļ�����
	/// </summary>
	NativeCallerLoader nativeCallerLoader;
	/// <summary>
	/// �йܶѳ�ʼ�ݻ���������������
	/// </summary>
	uint32 heapCapacity, heapGeneration;
	/// <summary>
	/// Э�̳�ʼ�ݻ�
	/// </summary>
	uint32 coroutineCapacity;
	/// <summary>
	/// ִ��ջ��ʼ�ݻ�
	/// </summary>
	uint32 executeStackCapacity;
	/// <summary>
	/// Э���쳣�Ļص�����
	/// </summary>
	OnExceptionExit onExceptionExit;

	StartupParameter(const RainLibrary* libraries, const uint32& libraryCount, const integer& seed, const uint32& stringCapacity, const uint32& entityCapacity, const EntityAction& onReferenceEntity, const EntityAction& onReleaseEntity, const LibraryLoader& libraryLoader, const NativeCallerLoader& nativeCallerLoader, const uint32& heapCapacity, const uint32& heapGeneration, const uint32& coroutineCapacity, const uint32& executeStackCapacity, const OnExceptionExit& onExceptionExit)
		: libraries(libraries), libraryCount(libraryCount), seed(seed), stringCapacity(stringCapacity), entityCapacity(entityCapacity), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(heapCapacity), heapGeneration(heapGeneration), coroutineCapacity(coroutineCapacity), executeStackCapacity(executeStackCapacity), onExceptionExit(onExceptionExit) {}
};

/// <summary>
/// �����״̬
/// </summary>
struct RAINLANGUAGE RainKernelState
{
	/// <summary>
	/// ��ǰЭ������
	/// </summary>
	uint32 coroutineCount;
	/// <summary>
	/// ��ǰ�ַ�������
	/// </summary>
	uint32 stringCount;
	/// <summary>
	/// ��ǰʵ������
	/// </summary>
	uint32 entityCount;
	/// <summary>
	/// ��ǰ�йܶ�������
	/// </summary>
	uint32 handleCount;
	/// <summary>
	/// ��ǰ�йܶѴ�С
	/// </summary>
	uint32 heapSize;

	RainKernelState(const uint32& coroutineCount, const uint32& stringCount, const uint32& entityCount, const uint32& handleCount, const uint32& heapSize) : coroutineCount(coroutineCount), stringCount(stringCount), entityCount(entityCount), handleCount(handleCount), heapSize(heapSize) {}
};

/// <summary>
/// �����б�
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
	/// ��ȡ��������
	/// </summary>
	/// <returns>��������</returns>
	inline uint32 Count() const { return count; }
	/// <summary>
	/// ��ȡ����
	/// </summary>
	/// <param name="index">�±�</param>
	/// <returns>����</returns>
	inline const RainType operator[](uint32 index) const { return types[index]; }
};

/// <summary>
/// ���Եĺ������
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
	/// �ж��Ƿ��Ǹ���Ч�ľ��
	/// </summary>
	/// <returns>�Ǹ���Ч�ľ��</returns>
	bool IsValid() const;
	/// <summary>
	/// ����һ�������ĵ���
	/// </summary>
	/// <returns>�����ĵ���</returns>
	InvokerWrapper CreateInvoker() const;
	/// <summary>
	/// ��ȡ�ú����Ĳ����б�
	/// </summary>
	/// <returns>�����б�</returns>
	RainTypes GetParameters() const;
	/// <summary>
	/// ��ȡ�����ķ���ֵ�б�
	/// </summary>
	/// <returns>�����б�</returns>
	RainTypes GetReturns() const;

};

/// <summary>
/// ��������б�
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
	/// ��ȡ�������
	/// </summary>
	/// <returns>�������</returns>
	inline uint32 Count() const { return count; }
	/// <summary>
	/// ��ȡ�������
	/// </summary>
	/// <param name="index">�±�</param>
	/// <returns>�������</returns>
	inline const RainFunction operator[](uint32 index) const { return functions[index]; }
	~RainFunctions();
};

/// <summary>
/// ���������
/// </summary>
class RAINLANGUAGE RainKernel
{
public:
	RainKernel() = default;
	virtual ~RainKernel() {};

	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ���һ��������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="allowNoPublic">�������˽�к���</param>
	/// <returns>�������</returns>
	virtual const RainFunction FindFunction(const RainString& name, bool allowNoPublic) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ���һ��������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="allowNoPublic">�������˽�к���</param>
	/// <returns>�������</returns>
	virtual const RainFunction FindFunction(const character* name, bool allowNoPublic) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ������������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="allowNoPublic">�������˽�к���</param>
	/// <returns>�������</returns>
	virtual RainFunctions FindFunctions(const RainString& name, bool allowNoPublic) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ������������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="allowNoPublic">�������˽�к���</param>
	/// <returns>�������</returns>
	virtual RainFunctions FindFunctions(const character* name, bool allowNoPublic) = 0;
	/// <summary>
	/// ��ȡ��ǰ״̬����
	/// </summary>
	/// <returns>״̬����</returns>
	virtual const RainKernelState GetState() = 0;
	/// <summary>
	/// �����йܶ���������
	/// </summary>
	/// <param name="full">������������������</param>
	/// <returns>�������������ͷŵ��йܶѴ�С</returns>
	virtual uint32 GC(bool full) = 0;

	/// <summary>
	/// ���������
	/// </summary>
	virtual void Update() = 0;
};


RAINLANGUAGE RainKernel* CreateKernel(const StartupParameter& parameter);
